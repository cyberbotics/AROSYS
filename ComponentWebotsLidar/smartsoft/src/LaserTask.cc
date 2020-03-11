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

#include "LaserTask.hh"

#include "ComponentWebotsLidar.hh"

#include <climits>
#include <cstdint>
#include <iostream>

#include <webots/Device.hpp>
#include <webots/Node.hpp>

LaserTask::LaserTask(SmartACE::SmartComponent *comp) :
  LaserTaskCore(comp),
  mThread(),
  mThreadRunning(false),
  mWebotsShouldQuit(false)
{
  std::cout << "constructor LaserTask\n";
  scanCount = 0;
}
LaserTask::~LaserTask()
{
  std::cout << "destructor LaserTask\n";
}

int LaserTask::on_entry()
{
  // do initialization procedures here, which are called once, each time the task is started
  // it is possible to return != 0 (e.g. when initialization fails) then the task is not executed further

  if (!COMP->webotsRobot)
    return -1;

  // acquisition
  COMP->mutex.acquire();

  // get timestep from the world and match the one in SmartMDSD component
  webotsTimeStep = COMP->webotsRobot->getBasicTimeStep();
  int coeff = S_TO_MS / (webotsTimeStep * COMP->connections.laserTask.periodicActFreq);
  webotsTimeStep *= coeff;

  // connect to the sensor from Webots
  webotsLidar = NULL;
  for (int i = 0; i < COMP->webotsRobot->getNumberOfDevices(); i++)
  {
    webots::Device *webotsDevice = COMP->webotsRobot->getDeviceByIndex(i);

    if (webotsDevice->getNodeType() == webots::Node::LIDAR)
    {
      std::string lidarName = webotsDevice->getName();
      webotsLidar = COMP->webotsRobot->getLidar(lidarName);
      webotsLidar->enable(webotsTimeStep);
      webotsLidar->enablePointCloud();
      std::cout << "Device #" << i << " called " << lidarName << " is a lidar." << std::endl;
      // set Webots sensor's properties to SmartMDSD model
      // useful doc:
      // http://servicerobotik-ulm.de/drupal/doxygen/components_commrep/classCommBasicObjects_1_1CommMobileLaserScan.html
      horizontalResolution = webotsLidar->getHorizontalResolution();
      numberValidPoints = webotsLidar->getNumberOfPoints();
      scan.set_scan_size(numberValidPoints);
      scan.set_scan_update_count(scanCount);
      scan.set_scan_integer_field_of_view(-horizontalResolution * UNIT_FACTOR / 2.0, horizontalResolution * UNIT_FACTOR);
      // pay attention to limits as min/max_distance variables are short type (max value is 65535)
      if (webotsLidar->getMaxRange() * M_TO_MM > SHORT_LIMIT)
      {
        std::cout << "The lidar range is bigger than 65.535 meters and will be set to 65 meters." << std::endl;
        scan.set_max_distance(65 * M_TO_MM);
      }
      else
        scan.set_max_distance(webotsLidar->getMaxRange() * M_TO_MM);
      scan.set_min_distance(webotsLidar->getMinRange() * M_TO_MM);
      scan.set_scan_length_unit(MEASURE_UNIT);
      break;
    }
  }

  // release
  COMP->mutex.release();

  if (!webotsLidar)
  {
    std::cout << "No lidar found, no data is sent." << std::endl;
    return -1;
  }

  return 0;
}

int LaserTask::on_execute()
{
  // this method is called from an outside loop,
  // hence, NEVER use an infinite loop (like "while(1)") here inside!!!
  // also do not use blocking calls which do not result from smartsoft kernel

  if (mWebotsShouldQuit)
    return -1;

  if (mThreadRunning || !COMP->webotsRobot)
    return 0;

  double basePosX = 0.0;
  double basePosY = 0.0;
  double basePosZ = 0.0;
  double basePosAzim = 0.0;
  double basePosElev = 0.0;
  double basePosRoll = 0.0;

  // acquisition
  COMP->mutex.acquire();

  // get base state from port
  Smart::StatusCode baseStatus = this->baseStateServiceInGetUpdate(baseState);

  // check if the transmission worked
  if (baseStatus != Smart::SMART_OK)
    std::cerr << "Error: receiving BaseState: " << baseStatus << std::endl;
  else
    std::cout << "BaseState received" << std::endl;

  basePosX = baseState.get_base_position().get_x(1.0);
  basePosY = baseState.get_base_position().get_y(1.0);
  basePosZ = baseState.get_base_position().get_z(1.0);
  basePosAzim = baseState.get_base_position().get_base_azimuth();
  basePosElev = baseState.get_base_position().get_base_elevation();
  basePosRoll = baseState.get_base_position().get_base_roll();
  scan.set_base_state(baseState);

  // print data to debug
  // std::cout << " " << std::endl;
  // std::cout << "basePosX " << basePosX << std::endl;
  // std::cout << "basePosY " << basePosY << std::endl;
  // std::cout << "basePosZ " << basePosZ << std::endl;
  // std::cout << "basePosAzim " << basePosAzim << std::endl;
  // std::cout << "basePosElev " << basePosElev << std::endl;
  // std::cout << "basePosRoll " << basePosRoll << std::endl;

  if (webotsLidar && webotsLidar->getRangeImage())
  {
    // time settings and update scan count
    timeval _receiveTime;
    ::gettimeofday(&_receiveTime, 0);
    scan.set_scan_time_stamp(CommBasicObjects::CommTimeStamp(_receiveTime));
    scan.set_scan_update_count(scanCount);

    // get sensor's values from Webots side
    const float *rangeImageVector;
    rangeImageVector = (const float *)(void *)webotsLidar->getRangeImage();  // in m

    // pass sensor's values to SmartMDSD side
    for (unsigned int i = 0; i < numberValidPoints; ++i)
    {
      // Pay attention to :
      //   o limits as min/max_distance variables are short type (max value is 65535)
      //   o same remark for the distance (max value is 65535)
      //   o Webots array for lidar value is inverted with the one in Smartsoft
      unsigned int dist = (unsigned int)(rangeImageVector[numberValidPoints - 1 - i] * M_TO_MM);
      scan.set_scan_index(i, i);
      scan.set_scan_integer_distance(i, dist);  // in mm

      // Print to debug
      // if (i%6==0) // to not display all
      //  std::cout << "["<<i<<"] " << dist << std::endl;
    }
    scan.set_scan_valid(true);
  }
  else
    scan.set_scan_valid(false);

  // send out laser scan through port
  laserServiceOutPut(scan);
  ++scanCount;
  scan.set_scan_valid(false);

  // start robot step thread
  mThreadRunning = true;
  if (mThread.joinable())
    mThread.join();
  mThread = std::thread(&LaserTask::runStep, this, COMP->webotsRobot);

  // release
  COMP->mutex.release();

  // it is possible to return != 0 (e.g. when the task detects errors), then the outer loop breaks and the task stops
  return 0;
}

int LaserTask::on_exit()
{
  // use this method to clean-up resources which are initialized in on_entry() and needs to be freed before the on_execute() can
  // be called again
  delete COMP->webotsRobot;
  return 0;
}

void LaserTask::runStep(webots::Robot *robot)
{
  mWebotsShouldQuit = robot->step(webotsTimeStep) == -1.0;
  mThreadRunning = false;
}
