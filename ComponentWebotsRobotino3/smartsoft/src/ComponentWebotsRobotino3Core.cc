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

#include "ComponentWebotsRobotino3Core.hh"

// constructor
ComponentWebotsRobotino3Core::ComponentWebotsRobotino3Core()
{
  std::cout << "constructor ComponentWebotsRobotino3Core\n";

  webotsRobot = NULL;

  // assign this controller to the correct robot in Webots
  char *robotName = std::getenv("WEBOTS_ROBOT_NAME");
  if (!robotName)
  {
    FILE *f = fopen("robotName.txt", "rb");
    if (!f)
    {
      std::cout << "'robotName.txt' file not found." << std::endl;
      return;
    }
    char name[256];
    int ret = fscanf(f, "%[^\n]", name);
    if (ret == 0)
    {
      std::cout << "First line of the 'robotName.txt' file is empty." << std::endl;
      return;
    }
    char environment[256] = "WEBOTS_ROBOT_NAME=";
    putenv(strcat(environment, name));
  }

  // create Robot instance
  webotsRobot = new webots::Robot();
}
