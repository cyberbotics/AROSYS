//--------------------------------------------------------------------------
// Code generated by the SmartSoft MDSD Toolchain
// The SmartSoft Toolchain has been developed by:
//
// Service Robotics Research Center
// University of Applied Sciences Ulm
// Prittwitzstr. 10
// 89075 Ulm (Germany)
//
// Information about the SmartSoft MDSD Toolchain is available at:
// www.servicerobotik-ulm.de
//
// This file is generated once. Modify this file to your needs.
// If you want the toolchain to re-generate this file, please
// delete it before running the code generator.
//--------------------------------------------------------------------------
#include "ComponentWebotsLidarCore.hh"

// constructor
ComponentWebotsLidarCore::ComponentWebotsLidarCore()
{
  std::cout << "constructor ComponentWebotsLidarCore\n";

  webotsRobot = NULL;

  // assign this controller to the correct robot in Webots
  char *robotName = std::getenv("WEBOTS_ROBOT_NAME");
  if (!robotName) {
    std::cout  << "WEBOTS_ROBOT_NAME not defined" << std::endl;
    FILE *f = fopen("robotName.txt", "rb");
    if (!f) {
      std::cout  << "'robotName.txt' file not found." << std::endl;
      return;
    }
    char name[256];
    int ret = fscanf(f, "%[^\n]", name);
    if (ret == 0) {
      std::cout  << "First line of the 'robotName.txt' file is empty." << std::endl;
      return;
    }
    char environment[256] = "WEBOTS_ROBOT_NAME=";
    putenv(strcat(environment, name));
  }

  // create Robot Instance
  webotsRobot = new webots::Robot();
}
