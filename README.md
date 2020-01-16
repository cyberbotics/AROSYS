# RobMoSys

## Components
The following three components are work in progress to create new components for RobMoSys in order to be able to use them in the Webots' simulator.

#### ComponentLaserObstacleAvoid
This component will calculate the velocity commands using sensor's data to avoid obstacles.
* `input:` a sensor (such as a lidar)
* `output:` velocity commands (such as a linear velocity and a turnrate)
The Pioneer 3-DX robot and a lidar (Sick LMS 291) are used for tests.

#### ComponentWebots
This component allow the user to use Webots simulator instead of Gazebo, PlayerStage, etc.. In its predeploy script, it is possible to specify the world used and his path as well as the option used at launch. It is also possible to modify the install path of Webots if needed. There exists a pre-requirement to use Webots. You have to execute this command one time in a Terminal and restart the computer/Virtual Machine. Don't forget to adapt the path if necessary.
`echo -e "export WEBOTS_HOME=/home/smartsoft/SOFTWARE/webots" >> ~/.profile`

#### ComponentWebotsLidar
This component configures the sensors in SmartSoft according to Webots prototype sensors and provides the sensor's data.

#### ComponentWebotsPioneer3DX
This component configures in SmartSoft the Pioneer 3-DX robot from Adept according to Webots prototype robot.

#### ComponentWebotsRobotino3
[Status] `Working, need improvement`

This component configures in SmartSoft the Robotino 3 robot from Festo according to Webots prototype robot.

#### ComponentWebotsTiago
[Status] `Need to be created`

This component configures in SmartSoft all versions of TIAGo's  robot from Pal Robotics according to Webots prototype robot.



## System

#### SystemWebotsPioneer3DXNavigation
This system shows a complete example of communication between a [lidar sensor](https://cyberbotics.com/doc/guide/lidar-sensors) (`ComponentWebotsLidar`) used by a [Pioneer 3-DX](https://cyberbotics.com/doc/guide/pioneer-3dx) robot (`ComponentWebotsPioneer3DX`) to navigate in a square arena filled with obstacles.

#### SystemWebotsRobotino3Navigation
This system shows a complete example of communication between a [lidar sensor](https://cyberbotics.com/doc/guide/lidar-sensors) (`ComponentWebotsLidar`) used by a [Robotino 3](https://cyberbotics.com/doc/guide/robotino3) robot (`ComponentWebotsPioneer3DX`) to navigate in a square arena filled with obstacles.
