/*
 * Michael PERRET
 * simple example library for obstacle avoidance based on laser scan.
 */

#include "AvoidanceAlgo.hh"

#include <CommBasicObjects/CommMobileLaserScan.hh>
#include <CommBasicObjects/CommNavigationVelocity.hh>

// Checks speed limits in rad/s
double AvoidanceAlgo::check_speed(double speed) {
  if (speed > MAX_SPEED)
	  speed = MAX_SPEED;
  if (speed < -MAX_SPEED)
	  speed = -MAX_SPEED;
  return speed;
}

// Calculates the linear velocity and turnrate according to sensor's information
void AvoidanceAlgo::run_cycle(CommBasicObjects::CommMobileLaserScan scan, double &out_speed, double &out_turnrate) {

	// Variables
	CommBasicObjects::CommNavigationVelocity vel;
	double new_speed = 0.0, left_speed = 0.0, right_speed = 0.0, new_turnrate = 0.0;

	const int count = scan.get_scan_size(); // 180
std::cout << "count: " << count << std::endl;

  // defines sector range
  int sector_range[N_SECTOR] = {0};
  int sector_size = (count - 2.0 * UNUSED_POINT - 1.0) / N_SECTOR; //29
std::cout << "sector_size: " << sector_size << std::endl;
  for (int i = 0; i < N_SECTOR; i++)
    sector_range[i] = UNUSED_POINT + (i + 1) * sector_size;

  // defines usefull points (above 80m / 8.0 = 10m, points not used)
  const float max_range = MAX_LIDAR_DIST*UNIT_FACTOR; // 80000mm ? *UNIT_FACTOR
std::cout << "max_range: " << max_range << std::endl;

  const double range_threshold = max_range / 8.0; // 10000mm
std::cout << "range_threshold: " << range_threshold << std::endl;

  //const float *urg04lx_values = NULL;

	// Initialize dynamic variables
  double left_obstacle = 0.0, right_obstacle = 0.0;
  double front_obstacle = 0.0, front_left_obstacle = 0.0, front_right_obstacle = 0.0;

for (int i = UNUSED_POINT; i < count - UNUSED_POINT - 1; ++i) {
  double dist = scan.get_scan_distance(i);
  if (dist>max_range)
    dist = max_range;

	if (dist < range_threshold) {
		std::cout << " " << std::endl;
		std::cout << "["<< i<<"] dist: " << dist << std::endl;
    if                         (i < sector_range[0])	left_obstacle 		   += (1.0 - dist / max_range);
    if (sector_range[0] <= i && i < sector_range[1])	front_left_obstacle  += (1.0 - dist / max_range);
    if (sector_range[1] <= i && i < sector_range[2])	front_obstacle 		   += (1.0 - dist / max_range);
    if (sector_range[2] <= i && i < sector_range[3])	front_right_obstacle += (1.0 - dist / max_range);
    if (sector_range[3] <= i && i < sector_range[4]+1)	right_obstacle 		 += (1.0 - dist / max_range);
  }
}
std::cout << " " << std::endl;
std::cout << "AVANT:" << std::endl;
std::cout << "   left_obs: " << left_obstacle 		<< std::endl;
std::cout << " f_left_obs: " << front_left_obstacle 	<< std::endl;
std::cout << "      f_obs: " << front_obstacle 		<< std::endl;
std::cout << "f_right_obs: " << front_right_obstacle << std::endl;
std::cout << "  right_obs: " << right_obstacle 		<< std::endl;

left_obstacle /= sector_size;
front_left_obstacle /= sector_size;
front_obstacle /= sector_size;
front_right_obstacle /= sector_size;
right_obstacle /= sector_size;

std::cout << " " << std::endl;
std::cout << "APRES:" << std::endl;
std::cout << "   left_obs: " << left_obstacle 		<< std::endl;
std::cout << " f_left_obs: " << front_left_obstacle 	<< std::endl;
std::cout << "      f_obs: " << front_obstacle 		<< std::endl;
std::cout << "f_right_obs: " << front_right_obstacle << std::endl;
std::cout << "  right_obs: " << right_obstacle 		<< std::endl;


	// Compute the speed according to the information on obstacles
  if (left_obstacle > right_obstacle && left_obstacle > NEAR_OBSTACLE_THRESHOLD) {
    const double speed_factor = (1.0 - FAST_DECREASE_FACTOR * left_obstacle) * MAX_SPEED / left_obstacle;
    left_speed = check_speed(speed_factor * left_obstacle);
    right_speed = check_speed(speed_factor * right_obstacle);

  } else if (front_left_obstacle > front_right_obstacle && front_left_obstacle > FAR_OBSTACLE_THRESHOLD) {
    const double speed_factor = (1.0 - SLOW_DECREASE_FACTOR * front_left_obstacle) * MAX_SPEED / front_left_obstacle;
    left_speed = check_speed(speed_factor * front_left_obstacle);
    right_speed = check_speed(speed_factor * front_right_obstacle);

  } else if (front_right_obstacle > front_left_obstacle && front_right_obstacle > FAR_OBSTACLE_THRESHOLD) {
    const double speed_factor = (1.0 - SLOW_DECREASE_FACTOR * front_right_obstacle) * MAX_SPEED / front_right_obstacle;
    left_speed = check_speed(speed_factor * front_left_obstacle);
    right_speed = check_speed(speed_factor * front_right_obstacle);

  } else if (right_obstacle > left_obstacle && right_obstacle > NEAR_OBSTACLE_THRESHOLD) {
    const double speed_factor = (1.0 - FAST_DECREASE_FACTOR * right_obstacle) * MAX_SPEED / right_obstacle;
    left_speed = check_speed(speed_factor * left_obstacle);
    right_speed = check_speed(speed_factor * right_obstacle);

  } else if (front_obstacle > NEAR_OBSTACLE_THRESHOLD) {
    const double speed_factor = (1.0 - FAST_DECREASE_FACTOR * front_obstacle) * MAX_SPEED / front_obstacle;
    // more obstacles on the right, so make a left u-turn to avoid being stuck
    if (front_right_obstacle > front_left_obstacle || right_obstacle > left_obstacle) {
      left_speed = -check_speed(speed_factor * front_obstacle);
      right_speed = check_speed(speed_factor * front_obstacle);
    } else {
      left_speed = check_speed(speed_factor * front_obstacle);
      right_speed = -check_speed(speed_factor * front_obstacle);
    }
    //wb_robot_step(1);

  } else {
    left_speed = CRUISING_SPEED;
    right_speed = CRUISING_SPEED;
  }

	std::cout << " left_speed: " << left_speed  << std::endl;
	std::cout << "right_speed: " << right_speed << std::endl;

	// reset dynamic variables to zero
  left_obstacle = 0.0;
  front_left_obstacle = 0.0;
  front_obstacle = 0.0;
  front_right_obstacle = 0.0;
  right_obstacle = 0.0;

	// Send commands to the robot
	out_speed = WHEEL_RADIUS*(right_speed+left_speed)/2.0;
	out_turnrate = WHEEL_RADIUS*(right_speed-left_speed)/WHEEL_GAP;
}
