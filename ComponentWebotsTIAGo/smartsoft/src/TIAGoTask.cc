//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain
// The SmartSoft Toolchain has been developed by:
//  
// Service Robotics Research Center
// University of Applied Sciences Ulm
// Prittwitzstr. 10
// 89075 Ulm (Germany)
//
// Information about the SmartSoft MDSD Toolchain is available at:
// www.servicerobotik-ulm.de
//
// This file is generated once. Modify this file to your needs. 
// If you want the toolchain to re-generate this file, please 
// delete it before running the code generator.
//--------------------------------------------------------------------------
#include "TIAGoTask.hh"
#include "ComponentWebotsTIAGo.hh"

#include <iostream>

TIAGoTask::TIAGoTask(SmartACE::SmartComponent *comp) 
:	TIAGoTaskCore(comp)
{
	std::cout << "constructor TIAGoTask\n";
}
TIAGoTask::~TIAGoTask() 
{
	std::cout << "destructor TIAGoTask\n";
}


void check_velocity(double& leftSpeed, double& rightSpeed, double max_speed){
  if(leftSpeed >  max_speed)  leftSpeed  =  max_speed;
  if(leftSpeed < -max_speed)  leftSpeed  = -max_speed;

  if(rightSpeed >  max_speed) rightSpeed =  max_speed;
  if(rightSpeed < -max_speed) rightSpeed = -max_speed;
}


int TIAGoTask::on_entry()
{
	// do initialization procedures here, which are called once, each time the task is started
	// it is possible to return != 0 (e.g. when initialization fails) then the task is not executed further

	// Acquisition
	COMP->TIAGoMutex.acquire();

	if (!COMP->webotsRobot)
		return -1;

	// get timestep from the world and match the one in SmartMDSD component
	webotsTimeStep = COMP->webotsRobot->getBasicTimeStep();
	int coeff = S_TO_MS/(webotsTimeStep*COMP->connections.tIAGoTask.periodicActFreq);
	webotsTimeStep *= coeff;

	// set GPS and IMU
	GPSFound = false;
	IMUFound = false;
	std::string GPSName;
	std::string IMUName;
	webots::Device *webotsDevice = NULL;

	for(int i=0; i<COMP->webotsRobot->getNumberOfDevices(); i++) {
		webotsDevice = COMP->webotsRobot->getDeviceByIndex(i);
		if (webotsDevice->getNodeType() == webots::Node::GPS) {
			GPSFound = true;
			GPSName = webotsDevice->getName();
			std::cout<<"Device #"<<i<<" called "<<webotsDevice->getName()<<" is a GPS."<<std::endl;
		}
		if (webotsDevice->getNodeType() == webots::Node::INERTIAL_UNIT) {
			IMUFound = true;
			IMUName = webotsDevice->getName();
			std::cout<<"Device #"<<i<<" called "<<webotsDevice->getName()<<" is a IMU."<<std::endl;
		}
		if(GPSFound && IMUFound)
			break;
	}

	// enable GPS and IMU if found
	if(GPSFound){
		webotsGPS = COMP->webotsRobot->getGPS(GPSName);
		webotsGPS->enable(webotsTimeStep);
	}
	else
		std::cout  << "No GPS found, data sent to `baseStateServiceOut` will be (0,0,0)." << std::endl;

	if(IMUFound){
		webotsIMU = COMP->webotsRobot->getInertialUnit(IMUName);
		webotsIMU->enable(webotsTimeStep);
	}
	else
		std::cout  << "No IMU found, data sent to `baseStateServiceOut` will be (0,0,0)." << std::endl;


	// set Motors (name from PROTO definition in Webots)
	webotsLeftMotor  = COMP->webotsRobot->getMotor("wheel_left_joint");
	webotsRightMotor = COMP->webotsRobot->getMotor("wheel_right_joint");

	webotsLeftMotor ->setPosition(INFINITY);
	webotsRightMotor->setPosition(INFINITY);

	webotsLeftMotor ->setVelocity(0);
	webotsRightMotor->setVelocity(0);

	motorMaxSpeed = webotsLeftMotor->getMaxVelocity(); // in rad/s


	// release
	COMP->TIAGoMutex.release();

	return 0;
}


int TIAGoTask::on_execute()
{
	// this method is called from an outside loop,
	// hence, NEVER use an infinite loop (like "while(1)") here inside!!!
	// also do not use blocking calls which do not result from smartsoft kernel
	
	//std::cout << "Hello from TIAGoTask " << std::endl;

	double speed = 0.0;
	double omega = 0.0;
	double leftSpeed  = 0.0;
	double rightSpeed = 0.0;
	CommBasicObjects::CommBaseState baseState;
	CommBasicObjects::CommBasePose basePosition;

	// Acquisition
	COMP->TIAGoMutex.acquire();

	// Get values from port NavigationVelocityServiceIn
	speed = COMP->vX;
	omega = COMP->vW;

	// set velocities in rad/s for motors and check limits
	rightSpeed = (2.0*speed + omega*WHEEL_GAP)/(2.0*WHEEL_RADIUS);
	leftSpeed  = (2.0*speed - omega*WHEEL_GAP)/(2.0*WHEEL_RADIUS);
	check_velocity(leftSpeed, rightSpeed, motorMaxSpeed);

	// controller code that is in "while loop" if run from Simulator should be inside "if statement" below,
	// otherwise the values will not be updated
	if (COMP->webotsRobot->step(webotsTimeStep) != -1) {

		// Set GPS values for port BaseStateServiceOut
		if(GPSFound){

			const double* GPS_value = webotsGPS->getValues();
			basePosition.set_x(GPS_value[0], 1.0);
			basePosition.set_y(GPS_value[1], 1.0);
			basePosition.set_z(GPS_value[2], 1.0);
			baseState.set_base_position(basePosition);

			// print data to debug
			std::cout << " " << std::endl;
			std::cout << "GPS_x : " << GPS_value[0]<< std::endl;
			std::cout << "GPS_y : " << GPS_value[1]<< std::endl;
			std::cout << "GPS_z : " << GPS_value[2]<< std::endl;
		}
		else
		{
			basePosition.set_x(0.0, 1.0);
			basePosition.set_y(0.0, 1.0);
			basePosition.set_z(0.0, 1.0);
			baseState.set_base_position(basePosition);
		}

		// Set IMU values for port BaseStateServiceOut
		// Webots use the NED convention, see https://cyberbotics.com/doc/reference/inertialunit
		// Smartsoft use ???, see ???
		// ROS use ENU convention, https://www.ros.org/reps/rep-0103.html
		// Be aware of this in your calculation
		if(IMUFound){

			const double* IMU_value = webotsIMU->getRollPitchYaw();
			basePosition.set_base_roll(IMU_value[0]);
			basePosition.set_base_azimuth(IMU_value[2]);
			basePosition.set_base_elevation(IMU_value[1]);
			baseState.set_base_position(basePosition);

			// print data to debug
			std::cout << " " << std::endl;
			std::cout << "IMU_roll  : " << IMU_value[0]<< std::endl;
			std::cout << "IMU_pitch : " << IMU_value[1]<< std::endl;
			std::cout << "IMU_yaw   : " << IMU_value[2]<< std::endl;
		}
		else
		{
			basePosition.set_base_roll(0.0);
			basePosition.set_base_azimuth(0.0);
			basePosition.set_base_elevation(0.0);
			baseState.set_base_position(basePosition);
		}

		// Pass values to motors in Webots side
		webotsLeftMotor  -> setVelocity(leftSpeed);
		webotsRightMotor -> setVelocity(rightSpeed);
	}
	else
		return -1;

	// release
	COMP->TIAGoMutex.release();

	// send baseState update to the port
	baseStateServiceOutPut(baseState);

	// it is possible to return != 0 (e.g. when the task detects errors), then the outer loop breaks and the task stops
	return 0;
}


int TIAGoTask::on_exit()
{
	delete COMP->webotsRobot;

  // use this method to clean-up resources which are initialized in on_entry() and needs to be freed before the on_execute() can be called again
  return 0;
}
