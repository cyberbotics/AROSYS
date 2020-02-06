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

#include "BumperTask.hh"

#include "ComponentWebotsBumper.hh"

#include "CommBasicObjects/CommBumperState.hh"

#include <iostream>

#include <webots/Device.hpp>
#include <webots/Node.hpp>

BumperTask::BumperTask(SmartACE::SmartComponent *comp) :
  BumperTaskCore(comp),
  mThread(),
  mThreadRunning(false),
  mWebotsShouldQuit(false)
{
  std::cout << "constructor BumperTask\n";
}
BumperTask::~BumperTask()
{
  std::cout << "destructor BumperTask\n";
}

int BumperTask::on_entry()
{
  // do initialization procedures here, which are called once, each time the task is started
  // it is possible to return != 0 (e.g. when initialization fails) then the task is not executed further

  if (!COMP->webotsRobot)
    return -1;

  // acquisition
  COMP->mutex.acquire();

  // get timestep from the world and match the one in SmartMDSD component
  webotsTimeStep = COMP->webotsRobot->getBasicTimeStep();
  int coeff = S_TO_MS / (webotsTimeStep * COMP->connections.bumperTask.periodicActFreq);
  webotsTimeStep *= coeff;

  // connect to the sensor from Webots
  webotsTouchSensor = NULL;
  for (int i = 0; i < COMP->webotsRobot->getNumberOfDevices(); i++)
  {
    webots::Device *webotsDevice = COMP->webotsRobot->getDeviceByIndex(i);

    if (webotsDevice->getNodeType() == webots::Node::TOUCH_SENSOR)
    {
      std::string bumperName = webotsDevice->getName();
      webots::TouchSensor *sensor = COMP->webotsRobot->getTouchSensor(bumperName);
      if (sensor->getType() == webots::TouchSensor::BUMPER)
      {
        webotsTouchSensor = sensor;
        webotsTouchSensor->enable(webotsTimeStep);
        std::cout << "Device #" << i << " called " << bumperName << " is a bumper." << std::endl;
        break;
      }
    }
  }

  // release
  COMP->mutex.release();

  if (!webotsTouchSensor)
  {
    std::cout << "No bumper found, no data sent." << std::endl;
    return -1;
  }

  return 0;
}

int BumperTask::on_execute()
{
  // this method is called from an outside loop,
  // hence, NEVER use an infinite loop (like "while(1)") here inside!!!
  // also do not use blocking calls which do not result from smartsoft kernel

  if (mWebotsShouldQuit)
    return -1;

  if (mThreadRunning || !COMP->webotsRobot)
    return 0;

  // acquisition
  COMP->mutex.acquire();

  CommBasicObjects::CommBumperEventState bumperEventState;
  if (webotsTouchSensor->getValue() == 1.0)
    bumperEventState.setNewState(CommBasicObjects::BumperStateType::BUMPER_PRESSED);
  else
    bumperEventState.setNewState(CommBasicObjects::BumperStateType::BUMPER_NOT_PRESSED);

  // send out bumper state through port
  bumperEventServiceOutPut(bumperEventState);

  // start robot step thread
  mThreadRunning = true;
  if (mThread.joinable())
    mThread.join();
  mThread = std::thread(&BumperTask::runStep, this, COMP->webotsRobot);

  // release
  COMP->mutex.release();

  // it is possible to return != 0 (e.g. when the task detects errors), then the outer loop breaks and the task stops
  return 0;
}

int BumperTask::on_exit()
{
  // use this method to clean-up resources which are initialized in on_entry() and needs to be freed before the on_execute() can
  // be called again
  delete COMP->webotsRobot;
  return 0;
}

void BumperTask::runStep(webots::Robot *robot)
{
  mWebotsShouldQuit = robot->step(webotsTimeStep) == -1.0;
  mThreadRunning = false;
}
