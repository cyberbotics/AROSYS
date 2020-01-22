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
#include "Pioneer3DXTask.hh"
#include "ComponentWebotsPioneer3DX.hh"

#include <iostream>

Pioneer3DXTask::Pioneer3DXTask(SmartACE::SmartComponent *comp)
:	Pioneer3DXTaskCore(comp)
{
  std::cout << "constructor Pioneer3DXTask\n";
}
Pioneer3DXTask::~Pioneer3DXTask()
{
  std::cout << "destructor Pioneer3DXTask\n";
}


void check_velocity(double& leftSpeed, double& rightSpeed, double max_speed){
  if(leftSpeed >  max_speed)  leftSpeed  =  max_speed;
  if(leftSpeed < -max_speed)  leftSpeed  = -max_speed;

  if(rightSpeed >  max_speed) rightSpeed =  max_speed;
  if(rightSpeed < -max_speed) rightSpeed = -max_speed;
}


int Pioneer3DXTask::on_entry()
{
  // do initialization procedures here, which are called once, each time the task is started
  // it is possible to return != 0 (e.g. when initialization fails) then the task is not executed further

  // assign this controller to the correct robot in Webots
  char *robotName = std::getenv("WEBOTS_ROBOT_NAME");
  if (!robotName) {
    std::cout  << "WEBOTS_ROBOT_NAME not defined" << std::endl;
    FILE *f = fopen("robotName.txt", "rb");
    if (!f) {
      std::cout  << "'robotName.txt' file not found." << std::endl;
      return -1;
    }
    char name[256];
    int ret = fscanf(f, "%[^\n]", name);
    if (ret == 0) {
      std::cout  << "First line of the 'robotName.txt' file is empty." << std::endl;
      return -1;
    }
    char environment[256] = "WEBOTS_ROBOT_NAME=";
    putenv(strcat(environment, name));
  }

  // create Robot Instance
  webotsRobot = new webots::Robot();

  // get timestep from the world
  webotsTimeStep = webotsRobot->getBasicTimeStep();

  // set GPS and IMU
  GPSFound = false;
  IMUFound = false;
	int GPSIndex = 0;
	int IMUIndex = 0;
	std::string GPSName;
	std::string IMUName;
	webots::Device *webotsDevice = NULL;

	for(int i=0; i<webotsRobot->getNumberOfDevices(); i++) {
		webotsDevice = webotsRobot->getDeviceByIndex(i);
		if (webotsDevice->getNodeType() == webots::Node::GPS) {
			GPSIndex = i;
			GPSFound = true;
			GPSName = webotsDevice->getName();
			std::cout<<"Device #"<<i<<" called "<<webotsDevice->getName()<<" is a GPS."<<std::endl;
		}
		if (webotsDevice->getNodeType() == webots::Node::INERTIAL_UNIT) {
			IMUIndex = i;
			IMUFound = true;
			IMUName = webotsDevice->getName();
			std::cout<<"Device #"<<i<<" called "<<webotsDevice->getName()<<" is a IMU."<<std::endl;
		}
		if(GPSFound && IMUFound)
			break;
	}

	// enable GPS and IMU if found
	if(GPSFound){
		webotsGPS = webotsRobot->getGPS(GPSName);
		webotsGPS->enable(webotsTimeStep);
	}
	else
		std::cout  << "No GPS found, data sent to `baseStateServiceOut` will be (0,0,0)." << std::endl;

	if(IMUFound){
		webotsIMU = webotsRobot->getInertialUnit(IMUName);
		webotsIMU->enable(webotsTimeStep);
	}
	else
		std::cout  << "No IMU found, data sent to `baseStateServiceOut` will be (0,0,0)." << std::endl;


  // set Motors (name from PROTO definition in Webots)
  webotsLeftMotor  = webotsRobot->getMotor("left wheel");
  webotsRightMotor = webotsRobot->getMotor("right wheel");

  webotsLeftMotor ->setPosition(INFINITY);
  webotsRightMotor->setPosition(INFINITY);

  webotsLeftMotor ->setVelocity(0);
  webotsRightMotor->setVelocity(0);

  motorMaxSpeed = webotsLeftMotor->getMaxVelocity(); // in rad/s

  return 0;
}


int Pioneer3DXTask::on_execute()
{
  // this method is called from an outside loop,
  // hence, NEVER use an infinite loop (like "while(1)") here inside!!!
  // also do not use blocking calls which do not result from smartsoft kernel

  //std::cout << "Hello from PioneerTask " << std::endl;

  double speed = 0.0;
  double omega = 0.0;
  double leftSpeed  = 0.0;
  double rightSpeed = 0.0;
	CommBasicObjects::CommBaseState baseState;
	CommBasicObjects::CommBasePose basePosition;

  // Acquisition
  COMP->PioneerMutex.acquire();

  // Get values from port NavigationVelocityServiceIn
  speed = COMP->vX;
  omega = COMP->vW;

  // set velocities in rad/s for motors and check limits
  rightSpeed = (2.0*speed + omega*WHEEL_GAP)/(2.0*WHEEL_RADIUS);
  leftSpeed  = (2.0*speed - omega*WHEEL_GAP)/(2.0*WHEEL_RADIUS);
  check_velocity(leftSpeed, rightSpeed, motorMaxSpeed);

  // controller code that is in "while loop" if run from Simulator should be inside "if statement" below,
  // otherwise the values will not be updated
  if (webotsRobot->step(webotsTimeStep) != -1) {

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
  COMP->PioneerMutex.release();

  // send baseState update to the port
  baseStateServiceOutPut(baseState);

  // it is possible to return != 0 (e.g. when the task detects errors), then the outer loop breaks and the task stops
  return 0;
}


int Pioneer3DXTask::on_exit()
{
  delete webotsRobot;

  // use this method to clean-up resources which are initialized in on_entry() and needs to be freed before the on_execute() can be called again
  return 0;
}
