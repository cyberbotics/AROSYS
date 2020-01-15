# RobMoSys

## Components
The following three components are work in progress to create new components for RobMoSys in order to be able to use them in the Webots' simulator.

#### ComponentLaserObstacleAvoidTest
[Status] `Working`

This component will calculate the velocity commands using sensor's data to avoid obstacles.
* `input:` a sensor (such as a lidar)
* `output:` velocity commands (such as a linear velocity and a turnrate)
The Pioneer 3-DX robot and a lidar (Sick LMS 291) are used for tests.

#### ComponentWebots
[Status] `Test in progress`

This component allow the user to use Webots simulator. In its predeploy script, it is possible to specify the world used and his path as well as the option used at launch. It is also possible to modify the install path of Webots if needed. There exists a pre-requirement to use Webots. You have to execute this command one time in a Terminal and restart the computer/Virtual Machine. Don't forget to adapt the path if necessary.
`echo -e "export WEBOTS_HOME=/home/smartsoft/SOFTWARE/webots" >> ~/.profile`

#### ComponentWebotsLidarTest
[Status] `Working`

This component configures the sensors in SmartSoft according to Webot prototype sensors and provides the sensor's data.

#### ComponentWebotsPioneer3DX
[Status] `Test in progress`

This component configures in SmartSoft the Pioneer 3-DX robot from Adept according to Webot prototype robot.

#### ComponentWebotsSimulator
[Status] `Working`

This component allow the user to use the Webot simulator instead of Gazebo, PlayerStage, etc.



## System

##### SystemLidarOnlyTest
[Status] `Working`

This system is to test the `ComponentWebotsLidarTest`'s behaviour.
It will gradually be supplemented by other components to increase the complexity of the system.
The aim of the latter is then to be made clean so that the resulting system can be shared under the name of `SystemWebotsLidarCommunication`.


#### SystemWebotsPioneer3DXNavigation
[Status] `Working`

This system shows a complete example of communication between lidar components used by a Pioneer 3-DX robot to navigate in a square arena full of obstacle.
