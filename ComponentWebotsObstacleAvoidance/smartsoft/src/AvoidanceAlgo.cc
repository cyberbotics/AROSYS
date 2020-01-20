/*
* simple example library for obstacle avoidance based on laser scan.
*/

#include "AvoidanceAlgo.hh"

#include <CommBasicObjects/CommMobileLaserScan.hh>
#include <CommBasicObjects/CommNavigationVelocity.hh>


// calculates the linear velocity and turnrate according to sensor's data
void AvoidanceAlgo::runCycle(CommBasicObjects::CommMobileLaserScan scan,
                             double &outSpeedX,
                             double &outSpeedY,
                             double &outSpeedW) {

  // initialize variables
  double leftSpeed = 0.0;
  double rightSpeed = 0.0;

  double leftObstacle = 0.0;
  double frontLeftObstacle = 0.0;
  double frontObstacle = 0.0;
  double frontRightObstacle = 0.0;
  double rightObstacle = 0.0;
  double maxDist = DIST_THRESHOLD*M_TO_CM; // Points above are not used

  // define sector range
  int count = scan.get_scan_size();
  int sectorRange[N_SECTOR] = {0};
  int sectorSize = (int)((double)count - 2.0 * (double)UNUSED_POINT) / (double)N_SECTOR;

  for (int i = 0; i < N_SECTOR; i++)
    sectorRange[i] = UNUSED_POINT + (i + 1) * sectorSize;

  // use only valid points to detect obstacle according to the sector range
  for (int i=UNUSED_POINT; i<count-UNUSED_POINT-1; ++i) {

    double dist = scan.get_scan_distance(i);
    if (dist <= maxDist) {
      if                        (i < sectorRange[0])    leftObstacle       += (1.0 - dist / maxDist);
      if (sectorRange[0] <= i && i < sectorRange[1])    frontLeftObstacle  += (1.0 - dist / maxDist);
      if (sectorRange[1] <= i && i < sectorRange[2])    frontObstacle      += (1.0 - dist / maxDist);
      if (sectorRange[2] <= i && i < sectorRange[3])    frontRightObstacle += (1.0 - dist / maxDist);
      if (sectorRange[3] <= i && i < sectorRange[4]+1)  rightObstacle      += (1.0 - dist / maxDist);
    }
  }

  // normalize the obstacle detection
  leftObstacle       /= sectorSize;
  frontLeftObstacle  /= sectorSize;
  frontObstacle      /= sectorSize;
  frontRightObstacle /= sectorSize;
  rightObstacle      /= sectorSize;

  if (leftObstacle > rightObstacle && leftObstacle > NEAR) {

    const double speedFactor = (1.0 - FAST_DECREASE_FACTOR * leftObstacle) * MAX_SPEED / leftObstacle;
    leftSpeed = speedFactor * leftObstacle;
    rightSpeed = speedFactor * rightObstacle;

  } else if (frontLeftObstacle > frontRightObstacle && frontLeftObstacle > FAR) {

    const double speedFactor = (1.0 - SLOW_DECREASE_FACTOR * frontLeftObstacle) * MAX_SPEED / frontLeftObstacle;
    leftSpeed = speedFactor * frontLeftObstacle;
    rightSpeed = speedFactor * frontRightObstacle;

  } else if (frontRightObstacle > frontLeftObstacle && frontRightObstacle > FAR) {

    const double speedFactor = (1.0 - SLOW_DECREASE_FACTOR * frontRightObstacle) * MAX_SPEED / frontRightObstacle;
    leftSpeed = speedFactor * frontLeftObstacle;
    rightSpeed = speedFactor * frontRightObstacle;

  } else if (rightObstacle > leftObstacle && rightObstacle > NEAR) {

    const double speedFactor = (1.0 - FAST_DECREASE_FACTOR * rightObstacle) * MAX_SPEED / rightObstacle;
    leftSpeed = speedFactor * leftObstacle;
    rightSpeed = speedFactor * rightObstacle;

  } else if (frontObstacle > NEAR) {

    const double speedFactor = (1.0 - FAST_DECREASE_FACTOR * frontObstacle) * MAX_SPEED / frontObstacle;
    // more obstacles on the right, so make a left u-turn to avoid being stuck
    if (frontRightObstacle > frontLeftObstacle || rightObstacle > leftObstacle) {

      leftSpeed = 0.1*speedFactor * frontObstacle;
      rightSpeed = 5*speedFactor * frontObstacle;

    } else {

      leftSpeed = 5*speedFactor * frontObstacle;
      rightSpeed = 0.1*speedFactor * frontObstacle;
    }

  } else {
    leftSpeed = CRUISING_SPEED;
    rightSpeed = CRUISING_SPEED;
  }

  // send result
  outSpeedX = WHEEL_RADIUS*(rightSpeed+leftSpeed)/2.0;
  outSpeedY = 0.0; // if it is a two wheeled robot
  outSpeedW = WHEEL_RADIUS*(rightSpeed-leftSpeed)/WHEEL_GAP;
  if (outSpeedW < TOL)
    outSpeedW = 0.0;
}
