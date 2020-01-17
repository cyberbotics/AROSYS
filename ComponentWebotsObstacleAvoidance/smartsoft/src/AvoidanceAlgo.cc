/*
 * simple example library for obstacle avoidance based on laser scan.
 */

#include "AvoidanceAlgo.hh"

#include <CommBasicObjects/CommMobileLaserScan.hh>
#include <CommBasicObjects/CommNavigationVelocity.hh>


// Calculates the linear velocity and turnrate according to sensor's data
void AvoidanceAlgo::run_cycle(CommBasicObjects::CommMobileLaserScan scan,
							  double &out_speed_x, double &out_speed_y, double &out_speed_w) {

	// Initialize variables
	double left_speed = 0.0;
	double right_speed = 0.0;

	double left_obstacle = 0.0;
	double front_left_obstacle = 0.0;
	double front_obstacle = 0.0;
	double front_right_obstacle = 0.0;
	double right_obstacle = 0.0;
	double max_dist = DIST_THRESHOLD*M_TO_CM; // Points above are not used

	// Define sector range
	int count = scan.get_scan_size();
	int sector_range[N_SECTOR] = {0};
	int sector_size = (int)((double)count - 2.0 * (double)UNUSED_POINT) / (double)N_SECTOR;

	for (int i = 0; i < N_SECTOR; i++)
		sector_range[i] = UNUSED_POINT + (i + 1) * sector_size;

	// Use only valid points to detect obstacle according to the sector range
	for (int i=UNUSED_POINT; i<count-UNUSED_POINT-1; ++i) {

		double dist = scan.get_scan_distance(i);
		if (dist <= max_dist) {
			if                         (i < sector_range[0])	left_obstacle 		 += (1.0 - dist / max_dist);
			if (sector_range[0] <= i && i < sector_range[1])	front_left_obstacle  += (1.0 - dist / max_dist);
			if (sector_range[1] <= i && i < sector_range[2])	front_obstacle 		 += (1.0 - dist / max_dist);
			if (sector_range[2] <= i && i < sector_range[3])	front_right_obstacle += (1.0 - dist / max_dist);
			if (sector_range[3] <= i && i < sector_range[4]+1)	right_obstacle 		 += (1.0 - dist / max_dist);
		}
	}

	// Normalize the obstacle detection
	left_obstacle        /= sector_size;
	front_left_obstacle  /= sector_size;
	front_obstacle       /= sector_size;
	front_right_obstacle /= sector_size;
	right_obstacle       /= sector_size;

	if (left_obstacle > right_obstacle && left_obstacle > NEAR) {

		const double speed_factor = (1.0 - FAST_DECREASE_FACTOR * left_obstacle) * MAX_SPEED / left_obstacle;
		left_speed = speed_factor * left_obstacle;
		right_speed = speed_factor * right_obstacle;

	} else if (front_left_obstacle > front_right_obstacle && front_left_obstacle > FAR) {

		const double speed_factor = (1.0 - SLOW_DECREASE_FACTOR * front_left_obstacle) * MAX_SPEED / front_left_obstacle;
		left_speed = speed_factor * front_left_obstacle;
		right_speed = speed_factor * front_right_obstacle;

	} else if (front_right_obstacle > front_left_obstacle && front_right_obstacle > FAR) {

		const double speed_factor = (1.0 - SLOW_DECREASE_FACTOR * front_right_obstacle) * MAX_SPEED / front_right_obstacle;
		left_speed = speed_factor * front_left_obstacle;
		right_speed = speed_factor * front_right_obstacle;

	} else if (right_obstacle > left_obstacle && right_obstacle > NEAR) {

		const double speed_factor = (1.0 - FAST_DECREASE_FACTOR * right_obstacle) * MAX_SPEED / right_obstacle;
		left_speed = speed_factor * left_obstacle;
		right_speed = speed_factor * right_obstacle;

	} else if (front_obstacle > NEAR) {

		const double speed_factor = (1.0 - FAST_DECREASE_FACTOR * front_obstacle) * MAX_SPEED / front_obstacle;
		// more obstacles on the right, so make a left u-turn to avoid being stuck
		if (front_right_obstacle > front_left_obstacle || right_obstacle > left_obstacle) {

			left_speed = 0.1*speed_factor * front_obstacle;
			right_speed = 5*speed_factor * front_obstacle;

		} else {

			left_speed = 5*speed_factor * front_obstacle;
			right_speed = 0.1*speed_factor * front_obstacle;
		}
	} else {
		left_speed = CRUISING_SPEED;
		right_speed = CRUISING_SPEED;
	}

	std::cout << " " << std::endl;
	std::cout << "[avoid] left_speed: " << left_speed  << std::endl;
	std::cout << "[avoid]right_speed: " << right_speed << std::endl;

	// Send result
	out_speed_x = WHEEL_RADIUS*(right_speed+left_speed)/2.0;
	out_speed_y = 0.0; // Because it is a two wheeled robot
	out_speed_w = WHEEL_RADIUS*(right_speed-left_speed)/WHEEL_GAP;
}
