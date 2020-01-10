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
	uint count = scan.get_scan_size();

	// Defines sector range
	int sector_range[N_SECTOR] = {0};
	int sector_size = (count-2.0*UNUSED_POINTS -1.0)/N_SECTOR;
	for (int i = 0; i<N_SECTOR; i++)
		sector_range[i] = UNUSED_POINTS + (i+1) * sector_size;

	// Defines useful points (above 80m/8.0 = 10m, points not used)
	const float max_range = MAX_LIDAR_DIST;
	const double range_threshold = max_range / DIVISION_FACTOR;
	const float *lidar_value = NULL;

	// Initialize dynamic variables
	double left_obs = 0.0, right_obs = 0.0;
	double front_obs = 0.0, front_left_obs = 0.0, front_right_obs = 0.0;

	for(int i = UNUSED_POINTS; i< count - UNUSED_POINTS-1; ++i){
		int dist = scan.get_scan_distance(i);
		if (i < sector_range[0]) // 20-48
			left_obs += (1.0-dist/max_range);
		if (sector_range[0]<=0 && i < sector_range[1]) // 48-76
			front_left_obs += (1.0-dist/max_range);
		if (sector_range[1]<=0 && i < sector_range[2]) // 76-104
			front_obs += (1.0-dist/max_range);
		if (sector_range[2]<=0 && i < sector_range[3]) // 104-132
			front_right_obs += (1.0-dist/max_range);
		if (sector_range[3]<=0 && i < sector_range[4]+1) // 132-160
			right_obs += (1.0-dist/max_range);
	}

	left_obs /= sector_size;
	front_left_obs /= sector_size;
	front_obs /= sector_size;
	front_right_obs /= sector_size;
	right_obs /= sector_size;

	std::cout << "   left_obs: " << left_obs 		<< std::endl;
	std::cout << " f_left_obs: " << front_left_obs 	<< std::endl;
	std::cout << "      f_obs: " << front_obs 		<< std::endl;
	std::cout << "f_right_obs: " << front_right_obs << std::endl;
	std::cout << "  right_obs: " << right_obs 		<< std::endl;


	// Compute the speed according to the information on obstacles
	// compute the speed according to the information on obstacles
	if (left_obs > right_obs && left_obs > NEAR_OBS_THRESHOLD) {
		const double speed_factor = (1.0 - FAST_DECR_FACTOR * left_obs) * MAX_SPEED / left_obs;
		left_speed = check_speed(speed_factor * left_obs);
		right_speed = check_speed(speed_factor * right_obs);
	}
	else if (front_left_obs > front_right_obs && front_left_obs > FAR_OBS_THRESHOLD) {
		const double speed_factor = (1.0 - SLOW_DECR_FACTOR * front_left_obs) * MAX_SPEED / front_left_obs;
		left_speed = check_speed(speed_factor * front_left_obs);
		right_speed = check_speed(speed_factor * front_right_obs);
	}
	else if (front_right_obs > front_left_obs && front_right_obs > FAR_OBS_THRESHOLD) {
		const double speed_factor = (1.0 - SLOW_DECR_FACTOR * front_right_obs) * MAX_SPEED / front_right_obs;
		left_speed = check_speed(speed_factor * front_left_obs);
		right_speed = check_speed(speed_factor * front_right_obs);
	}
	else if (right_obs > left_obs && right_obs > NEAR_OBS_THRESHOLD) {
		const double speed_factor = (1.0 - FAST_DECR_FACTOR * right_obs) * MAX_SPEED / right_obs;
		left_speed = check_speed(speed_factor * left_obs);
		right_speed = check_speed(speed_factor * right_obs);
	}
	else if (front_obs > NEAR_OBS_THRESHOLD) {
		const double speed_factor = (1.0 - FAST_DECR_FACTOR * front_obs) * MAX_SPEED / front_obs;
		// more obstacles on the right, so make a left u-turn to avoid being stuck
		if (front_right_obs > front_left_obs || right_obs > left_obs) {
			left_speed = -check_speed(speed_factor * front_obs);
			right_speed = check_speed(speed_factor * front_obs);
		}
		else {
			left_speed = check_speed(speed_factor * front_obs);
			right_speed = -check_speed(speed_factor * front_obs);
		}
	}
	else {
		left_speed = CRUISING_SPEED;
		right_speed = CRUISING_SPEED;
	}

	std::cout << " left_speed: " << left_speed  << std::endl;
	std::cout << "right_speed: " << right_speed << std::endl;

	// reset dynamic variables to zero
	left_obs = 0.0;
	front_left_obs = 0.0;
	front_obs = 0.0;
	front_right_obs = 0.0;
	right_obs = 0.0;

	// Send commands to the robot
	out_speed = WHEEL_RADIUS*(right_speed+left_speed)/2.0;
	out_turnrate = WHEEL_RADIUS*(right_speed-left_speed)/WHEEL_GAP;
}

