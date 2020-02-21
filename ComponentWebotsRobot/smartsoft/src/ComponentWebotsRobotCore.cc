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

#include "ComponentWebotsRobotCore.hh"

#include <fstream>

// constructor
ComponentWebotsRobotCore::ComponentWebotsRobotCore()
{
  mWebotsRobot = NULL;

  std::ifstream file_input("configuration.json");
  if (!file_input.is_open())
  {
    std::cerr << "Can't open 'configuration.json' file." << std::endl;
    return;
  }

  Json::Reader reader;
  if (!reader.parse(file_input, mConfiguration))
  {
    std::cerr << "Invalid 'configuration.json' file." << std::endl;
    return;
  }

  // assign this controller to the correct robot in Webots
  char *robotName = std::getenv("WEBOTS_ROBOT_NAME");
  if (!robotName)
  {
    if (!mConfiguration.isMember("name") || !mConfiguration["name"].isString())
    {
      std::cerr << "Missing or invalid 'name' key in 'configuration.json' file." << std::endl;
      return;
    }
    char environment[256] = "WEBOTS_ROBOT_NAME=";
    putenv(strcat(environment, mConfiguration["name"].asCString()));
  }

  // create Robot instance
  mWebotsRobot = new webots::Robot();
}
