# RobMoSys

## Components
The following three components are work in progress to create new components for RobMoSys in order to be able to use them in the Webots' simulator.

### ComponentLaserObstacleAvoidTest
This component will calculate the velocity commands using sensor's data to avoid obstacles.
* `input:` a sensor (such as a lidar)
* `output:` velocity commands (such as a linear velocity and a turnrate)
The Pioneer 3-DX robot and a lidar (Sick LMS 291) are used for tests.

### ComponentWebotsLidarTest
This component configures the sensors in SmartSoft according to Webot prototype sensors and provides the sensor's data.

### ComponentWebotsSimulator
This component allow the user to use the Webot simulator instead of Gazebo, PlayerStage, etc.



## System

### SystemLidarOnlyTest
This system is to test the ComponentWebotsLidarTest's behaviour.
It will gradually be supplemented by other components to increase the complexity of the system.
The aim of the latter is then to be made clean so that the resulting system can be shared under the name of SystemWebotsLidarCommunication.

### SystemWebotsLidarCommunication
This system shows a complete example of communication between the new components developed for the Webots' uses.
