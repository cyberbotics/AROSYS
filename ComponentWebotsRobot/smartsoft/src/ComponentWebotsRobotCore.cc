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
  std::cout << "constructor ComponentWebotsRobotCore\n";

  webotsRobot = NULL;

  std::cout << "BEGIN" << std::endl;
  std::ifstream file_input("example.json");
  Json::Reader reader;
  reader.parse(file_input, configuration);
  std::cout << configuration << std::endl;

  // assign this controller to the correct robot in Webots
  char *robotName = std::getenv("WEBOTS_ROBOT_NAME");
  if (!robotName)
  {
    char environment[256] = "WEBOTS_ROBOT_NAME=";
    putenv(strcat(environment, configuration["name"].asCString()));  //TODO: check existence: root.isMember("name")
  }

  std::cout << "END" << std::endl;

  // create Robot instance
  webotsRobot = new webots::Robot();
}
