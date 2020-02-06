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

#ifndef _LASERTASK_HH
#define _LASERTASK_HH

#include "CommBasicObjects/CommBasePose.hh"
#include "CommBasicObjects/CommBaseState.hh"
#include "CommBasicObjects/CommBaseVelocity.hh"
#include "CommBasicObjects/CommMobileLaserScan.hh"
#include "LaserTaskCore.hh"

#include <webots/Lidar.hpp>
#include <webots/Robot.hpp>

#include <thread>

// modify these parameters for unit consistency
#define M_TO_CM 100.0      // conversion factor
#define M_TO_MM 1000.0     // conversion factor
#define S_TO_MS 1000.0     // conversion factor
#define UNIT_FACTOR 100.0  // value is expressed in 0.01 degree units
#define SHORT_LIMIT 65535  // max value
#define MEASURE_UNIT 1.0   // set the internal length unit of the scan in mm
#define NEAR_OBSTACLE 400  // in mm (from ComponentLaserObstacleAvoid of SmartMDSD)
#define FAR_OBSTACLE 700   // in mm (from ComponentLaserObstacleAvoid of SmartMDSD)

class LaserTask : public LaserTaskCore
{
private:
  int webotsTimeStep;
  int horizontalResolution;
  unsigned int numberValidPoints;
  webots::Lidar *webotsLidar;

  // communication objects
  CommBasicObjects::CommBaseState baseState;
  CommBasicObjects::CommMobileLaserScan scan;

  // threading stuff
  std::thread mThread;
  bool mThreadRunning;
  bool mWebotsShouldQuit;
  void runStep(webots::Robot *robot);

public:
  unsigned long scanCount;

  LaserTask(SmartACE::SmartComponent *comp);
  virtual ~LaserTask();

  virtual int on_entry();
  virtual int on_execute();
  virtual int on_exit();
};

#endif
