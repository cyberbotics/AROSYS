// Copyright 1996-2020 Cyberbotics Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "RobotTask.hh"

#include "ComponentWebotsRobot.hh"

#include <iostream>

RobotTask::RobotTask(SmartACE::SmartComponent *comp) :
  RobotTaskCore(comp),
  mThread(),
  mThreadRunning(false),
  mWebotsShouldQuit(false)
{
  std::cout << "constructor RobotTask\n";
}
RobotTask::~RobotTask()
{
  std::cout << "destructor RobotTask\n";
}

void check_velocity(double &leftSpeed, double &rightSpeed, double max_speed)
{
  if (leftSpeed > max_speed)
    leftSpeed = max_speed;
  if (leftSpeed < -max_speed)
    leftSpeed = -max_speed;

  if (rightSpeed > max_speed)
    rightSpeed = max_speed;
  if (rightSpeed < -max_speed)
    rightSpeed = -max_speed;
}

int RobotTask::on_entry()
{
  // do initialization procedures here, which are called once, each time the task is started
  // it is possible to return != 0 (e.g. when initialization fails) then the task is not executed further

  if (!COMP->webotsRobot)
    return -1;

  // acquisition
  COMP->RobotMutex.acquire();

  // get timestep from the world and match the one in SmartMDSD component
  webotsTimeStep = COMP->webotsRobot->getBasicTimeStep();
  int coeff = S_TO_MS / (webotsTimeStep * COMP->connections.robotTask.periodicActFreq);
  webotsTimeStep *= coeff;

  // set GPS and IMU
  GPSFound = false;
  IMUFound = false;
  std::string GPSName;
  std::string IMUName;
  webots::Device *webotsDevice = NULL;

  for (int i = 0; i < COMP->webotsRobot->getNumberOfDevices(); i++)
  {
    webotsDevice = COMP->webotsRobot->getDeviceByIndex(i);

    if (webotsDevice->getNodeType() == webots::Node::GPS)
    {
      GPSFound = true;
      GPSName = webotsDevice->getName();
      std::cout << "Device #" << i << " called " << webotsDevice->getName() << " is a GPS." << std::endl;
    }
    if (webotsDevice->getNodeType() == webots::Node::INERTIAL_UNIT)
    {
      IMUFound = true;
      IMUName = webotsDevice->getName();
      std::cout << "Device #" << i << " called " << webotsDevice->getName() << " is a IMU." << std::endl;
    }
    if (GPSFound && IMUFound)
      break;
  }

  // enable GPS and IMU if found
  if (GPSFound)
  {
    webotsGPS = COMP->webotsRobot->getGPS(GPSName);
    webotsGPS->enable(webotsTimeStep);
  }
  else
    std::cout << "No GPS found, data sent to `baseStateServiceOut` will be (0,0,0)." << std::endl;

  if (IMUFound)
  {
    webotsIMU = COMP->webotsRobot->getInertialUnit(IMUName);
    webotsIMU->enable(webotsTimeStep);
  }
  else
    std::cout << "No IMU found, data sent to `baseStateServiceOut` will be (0,0,0)." << std::endl;

  // set Motors (name from PROTO definition in Webots)
  Json::Value velocityConfiguration = COMP->configuration["navigationVelocity"]; //TODO: check existence
  Json::Value::Members motorNames = velocityConfiguration.getMemberNames();
  for (int i=0; i < motorNames.size(); ++i) {
    webots::Motor *motor = COMP->webotsRobot->getMotor(motorNames[i]);
    if (motor) {
      motor->setPosition(INFINITY);
      motor->setVelocity(0);
      navigationMotors[motorNames[i]] = motor;
    }
  }

//  webotsLeftMotor = COMP->webotsRobot->getMotor("left wheel");
//  webotsRightMotor = COMP->webotsRobot->getMotor("right wheel");
//
//  webotsLeftMotor->setPosition(INFINITY);
//  webotsRightMotor->setPosition(INFINITY);
//
//  webotsLeftMotor->setVelocity(0);
//  webotsRightMotor->setVelocity(0);
//
//  motorMaxSpeed = webotsLeftMotor->getMaxVelocity();  // in rad/s

  // release
  COMP->RobotMutex.release();

  return 0;
}

int RobotTask::on_execute()
{
  // this method is called from an outside loop,
  // hence, NEVER use an infinite loop (like "while(1)") here inside!!!
  // also do not use blocking calls which do not result from smartsoft kernel

  // std::cout << "Hello from RobotTask " << std::endl;

  if (mWebotsShouldQuit)
    return -1;

  if (mThreadRunning || !COMP->webotsRobot)
    return 0;

  double speed = 0.0;
  double omega = 0.0;
  double leftSpeed = 0.0;
  double rightSpeed = 0.0;
  CommBasicObjects::CommBaseState baseState;
  CommBasicObjects::CommBasePose basePosition;

  // acquisition
  COMP->RobotMutex.acquire();

//  // get values from port NavigationVelocityServiceIn
//  speed = COMP->vX;
//  omega = COMP->vW;
//
//  // set velocities in rad/s for motors and check limits
//  rightSpeed = (2.0 * speed + omega * WHEEL_GAP) / (2.0 * WHEEL_RADIUS);
//  leftSpeed = (2.0 * speed - omega * WHEEL_GAP) / (2.0 * WHEEL_RADIUS);
  //check_velocity(leftSpeed, rightSpeed, motorMaxSpeed);

  // set GPS values for port BaseStateServiceOut
  if (GPSFound)
  {
    const double *GPS_value = webotsGPS->getValues();
    basePosition.set_x(GPS_value[2], 1.0);
    basePosition.set_y(GPS_value[0], 1.0);
    basePosition.set_z(GPS_value[1], 1.0);
    baseState.set_base_position(basePosition);

    // print data to debug
    // std::cout << " " << std::endl;
    // std::cout << "GPS_x : " << GPS_value[2]<< std::endl;
    // std::cout << "GPS_y : " << GPS_value[0]<< std::endl;
    // std::cout << "GPS_z : " << GPS_value[1]<< std::endl;
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
  if (IMUFound)
  {
    const double *IMU_value = webotsIMU->getRollPitchYaw();
    basePosition.set_base_roll(IMU_value[0]);
    basePosition.set_base_azimuth(IMU_value[2]);
    basePosition.set_base_elevation(IMU_value[1]);
    baseState.set_base_position(basePosition);

    // print data to debug
    // std::cout << " " << std::endl;
    // std::cout << "IMU_roll  : " << IMU_value[0]<< std::endl;
    // std::cout << "IMU_pitch : " << IMU_value[1]<< std::endl;
    // std::cout << "IMU_yaw   : " << IMU_value[2] << std::endl;
  }
  else
  {
    basePosition.set_base_roll(0.0);
    basePosition.set_base_azimuth(0.0);
    basePosition.set_base_elevation(0.0);
    baseState.set_base_position(basePosition);
  }

  // Pass values to motors in Webots side
  for (std::map<std::string, webots::Motor *>::iterator it=navigationMotors.begin(); it!=navigationMotors.end(); ++it) {
    const std::string name = it->first;
    webots::Motor *motor = it->second;
    motor->setVelocity(COMP->vX);
  }

  // send baseState update to the port
  baseStateServiceOutPut(baseState);

  // start robot step thread
  mThreadRunning = true;
  if (mThread.joinable())
    mThread.join();
  mThread = std::thread(&RobotTask::runStep, this, COMP->webotsRobot);

  // release
  COMP->RobotMutex.release();

  // it is possible to return != 0 (e.g. when the task detects errors), then the outer loop breaks and the task stops
  return 0;
}

int RobotTask::on_exit()
{
  // use this method to clean-up resources which are initialized in on_entry() and needs to be freed before the on_execute() can
  // be called again
  delete COMP->webotsRobot;
  return 0;
}

void RobotTask::runStep(webots::Robot *robot)
{
  mWebotsShouldQuit = robot->step(webotsTimeStep) == -1.0;
  mThreadRunning = false;
}
