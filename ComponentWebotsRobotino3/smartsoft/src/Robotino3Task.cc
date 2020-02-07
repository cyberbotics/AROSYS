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

#include "Robotino3Task.hh"

#include "ComponentWebotsRobotino3.hh"

#include <iostream>

Robotino3Task::Robotino3Task(SmartACE::SmartComponent *comp) :
  Robotino3TaskCore(comp),
  mThread(),
  mThreadRunning(false),
  mWebotsShouldQuit(false)
{
  std::cout << "constructor Robotino3Task\n";
}
Robotino3Task::~Robotino3Task()
{
  std::cout << "destructor Robotino3Task\n";
}

double check_velocity(double speed, double max_speed)
{
  if (speed > max_speed)
    speed = max_speed;
  if (speed < -max_speed)
    speed = -max_speed;
  return speed;
}

int Robotino3Task::on_entry()
{
  // do initialization procedures here, which are called once, each time the task is started
  // it is possible to return != 0 (e.g. when initialization fails) then the task is not executed further

  if (!COMP->webotsRobot)
    return -1;

  // acquisition
  COMP->Robotino3Mutex.acquire();

  // get timestep from the world and match the one in SmartMDSD component
  webotsTimeStep = COMP->webotsRobot->getBasicTimeStep();
  int coeff = S_TO_MS / (webotsTimeStep * COMP->connections.robotino3Task.periodicActFreq);
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
  webotsMotor0 = COMP->webotsRobot->getMotor("wheel0_joint");
  webotsMotor1 = COMP->webotsRobot->getMotor("wheel1_joint");
  webotsMotor2 = COMP->webotsRobot->getMotor("wheel2_joint");

  webotsMotor0->setPosition(INFINITY);
  webotsMotor1->setPosition(INFINITY);
  webotsMotor2->setPosition(INFINITY);

  webotsMotor0->setVelocity(0);
  webotsMotor1->setVelocity(0);
  webotsMotor2->setVelocity(0);

  motorMaxSpeed = webotsMotor0->getMaxVelocity();  // in rad/s

  // release
  COMP->Robotino3Mutex.release();

  return 0;
}

int Robotino3Task::on_execute()
{
  // this method is called from an outside loop,
  // hence, NEVER use an infinite loop (like "while(1)") here inside!!!
  // also do not use blocking calls which do not result from smartsoft kernel

  // std::cout << "Hello from Robotino3Task " << std::endl;

  if (mWebotsShouldQuit)
    return -1;

  if (mThreadRunning || !COMP->webotsRobot)
    return 0;

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

  // get values from port NavigationVelocityServiceIn
  vX = COMP->vX;  // in m/s
  vY = COMP->vY;  // in m/s
  vW = COMP->vW;  // in rad/s

  // set velocities in rad/s for motors and check limits
  // because of the orientation of the robot, vX and vY are inverted
  // conversion matrix from paper, section 4: http://ftp.itam.mx/pub/alfredo/ROBOCUP/SSLDocs/PapersTDPs/omnidrive.pdf

   vMotor0 = check_velocity(-0.5 * vY + 0.866 * vX - WHEEL_GAP * vW, motorMaxSpeed);
   vMotor1 = check_velocity(-0.5 * vY - 0.866 * vX - WHEEL_GAP * vW, motorMaxSpeed);
   vMotor2 = check_velocity(vY - WHEEL_GAP * vW, motorMaxSpeed);

  // set GPS values for port BaseStateServiceOut
  if (GPSFound)
  {
    const double *GPS_value = webotsGPS->getValues();
    basePosition.set_x(GPS_value[2], 1.0);
    basePosition.set_y(GPS_value[0], 1.0);
    basePosition.set_z(GPS_value[1], 1.0);
    baseState.set_base_position(basePosition);
  }
  else
  {
    basePosition.set_x(0.0, 1.0);
    basePosition.set_y(0.0, 1.0);
    basePosition.set_z(0.0, 1.0);
    baseState.set_base_position(basePosition);
  }

  // print data to debug
  // std::cout << " " << std::endl;
  // std::cout << "GPS_x : " << GPS_value[2]<< std::endl;
  // std::cout << "GPS_y : " << GPS_value[0]<< std::endl;
  // std::cout << "GPS_z : " << GPS_value[1]<< std::endl;

  // set IMU values for port BaseStateServiceOut
  // Webots use the NED convention, see https://cyberbotics.com/doc/reference/inertialunit
  // Smartsoft use ???, see ???
  // ROS use ENU convention, https://www.ros.org/reps/rep-0103.html
  // be aware of this in your calculation
  if (IMUFound)
  {
    const double *IMU_value = webotsIMU->getRollPitchYaw();
    basePosition.set_base_roll(IMU_value[0]);
    basePosition.set_base_azimuth(IMU_value[2]);
    basePosition.set_base_elevation(IMU_value[1]);
    baseState.set_base_position(basePosition);
  }
  else
  {
    basePosition.set_base_roll(0.0);
    basePosition.set_base_azimuth(0.0);
    basePosition.set_base_elevation(0.0);
    baseState.set_base_position(basePosition);
  }

  // print data to debug
  // std::cout << " " << std::endl;
  // std::cout << "IMU_roll  : " << IMU_value[0]<< std::endl;
  // std::cout << "IMU_pitch : " << IMU_value[1]<< std::endl;
  // std::cout << "IMU_yaw   : " << IMU_value[2]<< std::endl;

  // pass values to motors in Webots side
  webotsMotor0->setVelocity(vMotor0);
  webotsMotor1->setVelocity(vMotor1);
  webotsMotor2->setVelocity(vMotor2);

  // send baseState update to the port
  baseStateServiceOutPut(baseState);

  // start robot step thread
  mThreadRunning = true;
  if (mThread.joinable())
    mThread.join();
  mThread = std::thread(&Robotino3Task::runStep, this, COMP->webotsRobot);

  // release
  COMP->Robotino3Mutex.release();

  // it is possible to return != 0 (e.g. when the task detects errors), then the outer loop breaks and the task stops
  return 0;
}

int Robotino3Task::on_exit()
{
  // use this method to clean-up resources which are initialized in on_entry() and needs to be freed before the on_execute() can
  // be called again
  delete COMP->webotsRobot;
  return 0;
}

void Robotino3Task::runStep(webots::Robot *robot)
{
  mWebotsShouldQuit = robot->step(webotsTimeStep) == -1.0;
  mThreadRunning = false;
}
