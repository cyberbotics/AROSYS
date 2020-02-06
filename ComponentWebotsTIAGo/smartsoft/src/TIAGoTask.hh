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

#ifndef _TIAGOTASK_HH
#define _TIAGOTASK_HH

#include "TIAGoTaskCore.hh"

#include <webots/Device.hpp>
#include <webots/GPS.hpp>
#include <webots/InertialUnit.hpp>
#include <webots/Motor.hpp>
#include <webots/Node.hpp>
#include <webots/Robot.hpp>

#include "CommBasicObjects/CommBaseState.hh"

#include <thread>

#define S_TO_MS 1000.0  // Conversion factor

// TIAGo specification
#define WHEEL_GAP 0.404      // in meter
#define WHEEL_RADIUS 0.1955  // in meter

class TIAGoTask : public TIAGoTaskCore
{
private:
  int webotsTimeStep;
  bool GPSFound;
  bool IMUFound;
  double motorMaxSpeed;  // in rad/s
  webots::GPS *webotsGPS;
  webots::Motor *webotsRightMotor;
  webots::Motor *webotsLeftMotor;
  webots::InertialUnit *webotsIMU;

  // threading stuff
  std::thread mThread;
  bool mThreadRunning;
  bool mWebotsShouldQuit;
  void runStep(webots::Robot *robot);

public:
  TIAGoTask(SmartACE::SmartComponent *comp);
  virtual ~TIAGoTask();

  virtual int on_entry();
  virtual int on_execute();
  virtual int on_exit();
};

#endif
