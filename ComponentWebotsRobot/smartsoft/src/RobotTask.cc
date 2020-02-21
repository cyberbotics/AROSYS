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

#define SECOND_TO_MILLISECONDS 1000.0  // conversion factor

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

void set_velocity_in_bound(webots::Motor *motor, double velocity)
{
  const double maxSpeed = motor->getMaxVelocity();
  if (velocity > maxSpeed)
    velocity = maxSpeed;
  if (velocity < -maxSpeed)
    velocity = -maxSpeed;
  motor->setVelocity(velocity);
}

int RobotTask::on_entry()
{
  if (!COMP->mWebotsRobot)
    return -1;

  // acquisition
  COMP->mRobotMutex.acquire();

  // get timestep from the world and match the one in SmartMDSD component
  webotsTimeStep = COMP->mWebotsRobot->getBasicTimeStep();
  int coeff = SECOND_TO_MILLISECONDS / (webotsTimeStep * COMP->connections.robotTask.periodicActFreq);
  webotsTimeStep *= coeff;

  // set GPS and IMU
  mWebotsGPS = NULL;
  mWebotsInertialUnit = NULL;
  webots::Device *webotsDevice = NULL;

  for (int i = 0; i < COMP->mWebotsRobot->getNumberOfDevices(); i++)
  {
    webotsDevice = COMP->mWebotsRobot->getDeviceByIndex(i);

    if (webotsDevice->getNodeType() == webots::Node::GPS)
    {
      mWebotsGPS = COMP->mWebotsRobot->getGPS(webotsDevice->getName());
      std::cout << "Device #" << i << " called " << webotsDevice->getName() << " is a GPS." << std::endl;
    }
    if (webotsDevice->getNodeType() == webots::Node::INERTIAL_UNIT)
    {
      mWebotsInertialUnit = COMP->mWebotsRobot->getInertialUnit(webotsDevice->getName());
      std::cout << "Device #" << i << " called " << webotsDevice->getName() << " is an InertialUnit." << std::endl;
    }
    if (mWebotsGPS && mWebotsInertialUnit)
      break;
  }

  // enable GPS and IMU if found
  if (mWebotsGPS)
    mWebotsGPS->enable(webotsTimeStep);
  else
    std::cerr << "No GPS found, data sent to `baseStateServiceOut` will be (0,0,0)." << std::endl;

  if (mWebotsInertialUnit)
    mWebotsInertialUnit->enable(webotsTimeStep);
  else
    std::cerr << "No InertialUnit found, data sent to `baseStateServiceOut` will be (0,0,0)." << std::endl;

  // get the required motor for the navigation according to the configuration file
  if (COMP->mConfiguration.isMember("navigationVelocity") && COMP->mConfiguration["navigationVelocity"].isObject())
  {
    const Json::Value velocityConfiguration = COMP->mConfiguration["navigationVelocity"];
    const Json::Value::Members motorNames = velocityConfiguration.getMemberNames();
    for (int i = 0; i < motorNames.size(); ++i)
    {
      webots::Motor *motor = COMP->mWebotsRobot->getMotor(motorNames[i]);
      if (motor)
      {
        motor->setPosition(INFINITY);
        motor->setVelocity(0);
        mWebotsNavigationMotors[motorNames[i]] = motor;
      }
    }
  }
  else
    std::cerr << "Missing or invalid 'navigationVelocity' key in 'configuration.json' file." << std::endl;

  // release
  COMP->mRobotMutex.release();

  return 0;
}

int RobotTask::on_execute()
{
  if (mWebotsShouldQuit)
    return -1;

  if (mThreadRunning || !COMP->mWebotsRobot)
    return 0;

  double speed = 0.0;
  double omega = 0.0;
  double leftSpeed = 0.0;
  double rightSpeed = 0.0;
  CommBasicObjects::CommBaseState baseState;
  CommBasicObjects::CommBasePose basePosition;

  // acquisition
  COMP->mRobotMutex.acquire();

  // set GPS values for port BaseStateServiceOut
  if (mWebotsGPS)
  {
    const double *GPS_value = mWebotsGPS->getValues();
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
  if (mWebotsInertialUnit)
  {
    const double *inertialInutValue = mWebotsInertialUnit->getRollPitchYaw();
    basePosition.set_base_roll(inertialInutValue[0]);
    basePosition.set_base_azimuth(inertialInutValue[2]);
    basePosition.set_base_elevation(inertialInutValue[1]);
    baseState.set_base_position(basePosition);

    // print data to debug
    // std::cout << " " << std::endl;
    // std::cout << "InertialUnit roll  : " << inertialInutValue[0]<< std::endl;
    // std::cout << "InertialUnit pitch : " << inertialInutValue[1]<< std::endl;
    // std::cout << "InertialUnit yaw   : " << inertialInutValue[2] << std::endl;
  }
  else
  {
    basePosition.set_base_roll(0.0);
    basePosition.set_base_azimuth(0.0);
    basePosition.set_base_elevation(0.0);
    baseState.set_base_position(basePosition);
  }

  // Pass values to motors in Webots side
  for (std::map<std::string, webots::Motor *>::iterator it = mWebotsNavigationMotors.begin();
       it != mWebotsNavigationMotors.end(); ++it)
  {
    const std::string name = it->first;
    const Json::Value coefficients = COMP->mConfiguration["navigationVelocity"][name];
    if (!coefficients.isArray() || coefficients.size() != 3 || !coefficients[0].isDouble() || !coefficients[1].isDouble() ||
        !coefficients[2].isDouble())
    {
      std::cerr << "Wrong value for the 'navigationVelocity." << name << "' key, the value should be a array of 3 doubles."
                << std::endl;
      break;
    }
    webots::Motor *motor = it->second;
    set_velocity_in_bound(motor, COMP->mVX * coefficients[0].asDouble() + COMP->mVY * coefficients[1].asDouble() +
                                   COMP->mOmega * coefficients[2].asDouble());
  }

  // send baseState update to the port
  baseStateServiceOutPut(baseState);

  // start robot step thread
  mThreadRunning = true;
  if (mThread.joinable())
    mThread.join();
  mThread = std::thread(&RobotTask::runStep, this, COMP->mWebotsRobot);

  // release
  COMP->mRobotMutex.release();

  return 0;
}

int RobotTask::on_exit()
{
  delete COMP->mWebotsRobot;
  return 0;
}

void RobotTask::runStep(webots::Robot *robot)
{
  mWebotsShouldQuit = robot->step(webotsTimeStep) == -1.0;
  mThreadRunning = false;
}
