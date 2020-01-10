//------------------------------------------------------------------------
//
//  Copyright (C) 2008, 2009 Andreas Steck, Christian Schlegel
//
//        schlegel@hs-ulm.de
//
//        Christian Schlegel (schlegel@hs-ulm.de)
//        University of Applied Sciences
//        Prittwitzstr. 10
//        89075 Ulm (Germany)
//
//  This file is part of the "SmartKatana component".
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warRanty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//--------------------------------------------------------------------------

/*
 * simple example library for obstacle avoidance based on laser scan.
 */

#ifndef SIMPLE_AVOID_HH
#define SIMPLE_AVOID_HH

// These 4 parameters have to be the same in the ComponentWebotsSimulator
#define MAX_LIDAR_DIST 80.0 // in meter
#define WHEEL_GAP 0.269     // in meter
#define WHEEL_RADIUS 0.0975 // in meter
#define MAX_SPEED 12.3      // in rad/s

#define CRUISING_SPEED 5.0     // in rad/s
#define NEAR_OBS_THRESHOLD 1.0 // in meter
#define FAR_OBS_THRESHOLD 5.0  // in meter
#define FAST_DECR_FACTOR 0.9
#define SLOW_DECR_FACTOR 0.5
#define DIVISION_FACTOR 8.0
#define UNUSED_POINTS 15
#define N_SECTOR 5


#include <CommBasicObjects/CommMobileLaserScan.hh>
#include <CommBasicObjects/CommNavigationVelocity.hh>


class AvoidanceAlgo
{
private:
	// Checks speed limits in rad/s
    static double check_speed(double speed);

public:
    // Calculates the linear velocity and turnrate according to sensor's information
    static void run_cycle(CommBasicObjects::CommMobileLaserScan scan,
    		      double &out_speed, double &out_turnrate);
};

#endif
