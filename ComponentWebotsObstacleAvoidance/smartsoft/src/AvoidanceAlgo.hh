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

// These parameters have to be consistent with the robot used

//* --- Pioneer 3-DX ---
#define MAX_SPEED 12.3      // in rad/s
#define WHEEL_GAP 0.269     // in meter
#define WHEEL_RADIUS 0.0975 // in meter
#define CRUISING_SPEED 5.0  // in rad/s
//*/

/* --- Robotino 3 ---
#define MAX_SPEED 44.4      // in rad/s
#define WHEEL_GAP 0.184     // in meter
#define WHEEL_RADIUS 0.0625 // in meter
#define CRUISING_SPEED 20.0 // in rad/s
*/

// Parameters that can be modified
#define N_SECTOR 5
#define UNUSED_POINT 15
#define DIST_THRESHOLD 5.0  // in meter
#define TOL 0.000001

// Tuning parameters
#define FAR 0.4
#define NEAR 0.75
#define M_TO_CM 100.0 // convert meter to cm
#define FAST_DECREASE_FACTOR 0.9
#define SLOW_DECREASE_FACTOR 0.5


#include <CommBasicObjects/CommMobileLaserScan.hh>
#include <CommBasicObjects/CommNavigationVelocity.hh>


class AvoidanceAlgo
{
private:

public:
  // Calculates the linear velocity and turnrate according to sensor's data
  static void runCycle(CommBasicObjects::CommMobileLaserScan scan,
                       double &outSpeedX,
                       double &outSpeedY,
                       double &outSpeedW);
};

#endif
