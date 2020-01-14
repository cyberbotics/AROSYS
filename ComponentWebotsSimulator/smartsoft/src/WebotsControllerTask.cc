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
#include "WebotsControllerTask.hh"
#include "ComponentWebotsSimulator.hh"

#include <iostream>

#include <webots/Device.hpp>
#include <webots/Node.hpp>


WebotsControllerTask::WebotsControllerTask(SmartACE::SmartComponent *comp) 
:	WebotsControllerTaskCore(comp)
{
	std::cout << "constructor WebotsControllerTask\n";
}
WebotsControllerTask::~WebotsControllerTask() 
{
	std::cout << "destructor WebotsControllerTask\n";
}


int WebotsControllerTask::on_entry()
{

	// create Robot Instance
	wb_robot = new webots::Robot();

	// Get timestep from the world
	wb_time_step = wb_robot->getBasicTimeStep();

	// set Motors
	wb_left_motor  = wb_robot->getMotor("left wheel");
	wb_right_motor = wb_robot->getMotor("right wheel");

	wb_left_motor ->setPosition(INFINITY);
	wb_right_motor->setPosition(INFINITY);

	wb_left_motor ->setVelocity(0);
	wb_right_motor->setVelocity(0);

	return 0;
}

void check_velocity(double& left_speed, double& right_speed){
    if(-left_speed > MAX_SPEED) left_speed = -MAX_SPEED;
    if( left_speed > MAX_SPEED) left_speed =  MAX_SPEED;

    if(-right_speed > MAX_SPEED) right_speed = -MAX_SPEED;
    if( right_speed > MAX_SPEED) right_speed =  MAX_SPEED;
}


int WebotsControllerTask::on_execute()
{
	// this method is called from an outside loop,
	// hence, NEVER use an infinite loop (like "while(1)") here inside!!!
	// also do not use blocking calls which do not result from smartsoft kernel

	//std::cout << "Hello from WebotsControllerTask " << std::endl;

	double speed = 0.0;
	double omega = 0.0;
	double left_speed  = 0.0;
	double right_speed = 0.0;

	// Acquisition
	COMP->WebotsMutex.acquire();

	// Get values from port
	omega = COMP->turnrate;
    speed = COMP->left_velocity;

    // Set velocities in rad/s for motors and check limits
    right_speed = (2.0*speed + omega*WHEEL_GAP)/(2.0*WHEEL_RADIUS);
    left_speed = (2.0*speed - omega*WHEEL_GAP)/(2.0*WHEEL_RADIUS);
    check_velocity(left_speed, right_speed);

	//std::cout  << "left_speed  : " << left_speed  << std::endl;
	//std::cout  << "right_speed : " << right_speed << std::endl;
	//std::cout  << "omega       : " << omega       << std::endl;

    // Controller Code that is in "while loop" if run from Simulator should be inside "if statement" below,
    //otherwise the values will not be updated
    if (wb_robot->step(wb_time_step) != -1) {
    	// Pass values to motors in Webots side
    	wb_left_motor->setVelocity(left_speed);
    	wb_right_motor->setVelocity(right_speed);
    }

    // Release
    COMP->WebotsMutex.release();

	// it is possible to return != 0 (e.g. when the task detects errors), then the outer loop breaks and the task stops
	return 0;
}


int WebotsControllerTask::on_exit()
{
	delete wb_robot;

	// use this method to clean-up resources which are initialized in on_entry() and needs to be freed before the on_execute() can be called again
	return 0;
}
