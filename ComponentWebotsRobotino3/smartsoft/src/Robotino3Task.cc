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
#include "Robotino3Task.hh"
#include "ComponentWebotsRobotino3.hh"

#include <iostream>

#include <webots/Device.hpp>
#include <webots/Node.hpp>

Robotino3Task::Robotino3Task(SmartACE::SmartComponent *comp)
:	Robotino3TaskCore(comp)
{
	std::cout << "constructor Robotino3Task\n";
}
Robotino3Task::~Robotino3Task()
{
	std::cout << "destructor Robotino3Task\n";
}


double check_velocity(double speed, double max_speed){
	if(speed >  max_speed)  speed =  max_speed;
	if(speed < -max_speed)  speed = -max_speed;
	return speed;
}


int Robotino3Task::on_entry()
{
	// do initialization procedures here, which are called once, each time the task is started
	// it is possible to return != 0 (e.g. when initialization fails) then the task is not executed further

	// Acquisition
	COMP->Robotino3Mutex.acquire();

	if (!COMP->webotsRobot)
		  return -1;

	// get timestep from the world
	webotsTimeStep = COMP->webotsRobot->getBasicTimeStep();

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
			} else if (webotsDevice->getNodeType() == webots::Node::INERTIAL_UNIT) {
				IMUFound = true;
				IMUName = webotsDevice->getName();
				std::cout<<"Device #"<<i<<" called "<<webotsDevice->getName()<<" is a IMU."<<std::endl;
			}

			if (GPSFound && IMUFound)
				break;
		}

		// enable GPS and IMU if found
		if (GPSFound){
			webotsGPS = COMP->webotsRobot->getGPS(GPSName);
			webotsGPS->enable(webotsTimeStep);
		} else
			std::cout  << "No GPS found, data sent to `baseStateServiceOut` will be (0,0,0)." << std::endl;

		if (IMUFound){
			webotsIMU = COMP->webotsRobot->getInertialUnit(IMUName);
			webotsIMU->enable(webotsTimeStep);
		} else
			std::cout  << "No IMU found, data sent to `baseStateServiceOut` will be (0,0,0)." << std::endl;


	// set Motors (name from PROTO definition in Webots)
	webotsMotor0 = COMP->webotsRobot->getMotor("wheel0_joint");
	webotsMotor1 = COMP->webotsRobot->getMotor("wheel1_joint");
	webotsMotor2 = COMP->webotsRobot->getMotor("wheel2_joint");

	webotsMotor0->setPosition(INFINITY);
	webotsMotor1->setPosition(INFINITY);
	webotsMotor2->setPosition(INFINITY);

	webotsMotor0->setVelocity(0);
	webotsMotor1->setVelocity(0);
	webotsMotor2->setVelocity(0);

	motorMaxSpeed = webotsMotor0->getMaxVelocity(); // in rad/s

	// release
	COMP->Robotino3Mutex.release();

	return 0;
}


int Robotino3Task::on_execute()
{
	// this method is called from an outside loop,
	// hence, NEVER use an infinite loop (like "while(1)") here inside!!!
	// also do not use blocking calls which do not result from smartsoft kernel

	//std::cout << "Hello from Robotino3Task " << std::endl;

	double vX = 0.0;
	double vY = 0.0;
	double vW = 0.0;
	double vMotor0 = 0.0;
	double vMotor1 = 0.0;
	double vMotor2 = 0.0;
	CommBasicObjects::CommBaseState baseState;
	CommBasicObjects::CommBasePose basePosition;


	// acquisition
	COMP->Robotino3Mutex.acquire();

	// get values from port
	vX = COMP->velX; // in m/s
	vY = COMP->velY; // in m/s
	vW = COMP->velW; // in rad/s

	std::cout << " " << std::endl;
	std::cout << "[Robotino-Task] Get data" << std::endl;
	std::cout << "vX : " << vX << std::endl;
	std::cout << "vY : " << vY << std::endl;
	std::cout << "vW : " << vW << std::endl;

	// set velocities in rad/s for motors and check limits
	// because of the orientation of the robot, vX and vY are inverted
	// conversion matrix from paper, section 4: http://ftp.itam.mx/pub/alfredo/ROBOCUP/SSLDocs/PapersTDPs/omnidrive.pdf

	// Test 1, se déplace vers la gauche
	//vMotor0 = check_velocity(-0.5*vY+0.866*vX+WHEEL_GAP*vW, motorMaxSpeed);
	//vMotor1 = check_velocity(-0.5*vY-0.866*vX+WHEEL_GAP*vW, motorMaxSpeed);
	//vMotor2 = check_velocity(     vY         +WHEEL_GAP*vW, motorMaxSpeed);

	// Test 2, se déplace en diagonal vers le haut à droite
	vMotor0 = check_velocity(-0.5*vX+0.866*vY+WHEEL_GAP*vW, motorMaxSpeed);
	vMotor1 = check_velocity(-0.5*vX-0.866*vY+WHEEL_GAP*vW, motorMaxSpeed);
	vMotor2 = check_velocity(     vX         +WHEEL_GAP*vW, motorMaxSpeed);

	std::cout << " " << std::endl;
	std::cout << "[Robotino-Task] Set speed" << std::endl;
	std::cout << "vMotor0 : " << vMotor0 << std::endl;
	std::cout << "vMotor1 : " << vMotor1 << std::endl;
	std::cout << "vMotor2 : " << vMotor2 << std::endl;

	// controller code that is in "while loop" if run from Simulator should be inside "if statement" below,
	// otherwise the values will not be updated
	if (COMP->webotsRobot->step(webotsTimeStep) != -1) {

		// Set GPS values for port BaseStateServiceOut
		if (GPSFound) {
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
		} else {
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
		if (IMUFound) {
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
		} else {
			basePosition.set_base_roll(0.0);
			basePosition.set_base_azimuth(0.0);
			basePosition.set_base_elevation(0.0);
			baseState.set_base_position(basePosition);
		}

		// Pass values to motors in Webots side
		webotsMotor0->setVelocity(vMotor0);
		webotsMotor1->setVelocity(vMotor1);
		webotsMotor2->setVelocity(vMotor2);
	}
	else
		return -1;

	// release
	COMP->Robotino3Mutex.release();

	// send baseState update to the port
	baseStateServiceOutPut(baseState);

	// it is possible to return != 0 (e.g. when the task detects errors), then the outer loop breaks and the task stops
	return 0;
}


int Robotino3Task::on_exit()
{
	delete COMP->webotsRobot;

	// use this method to clean-up resources which are initialized in on_entry() and needs to be freed before the on_execute() can be called again
	return 0;
}
