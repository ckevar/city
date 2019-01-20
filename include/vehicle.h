#include "sensors.h"
 
#ifndef VEHICLE_H
#define VEHICLE_H 
 
// VEHICLE DIMENSIONS
#define VEHICLE_LENGTH  15
#define VEHICLE_WIDTH   10
 
// VEHICLE BOUND FEATURES
#define MIN_MASS_CAR        990
#define MAX_MASS_CAR        2000
#define MIN_FRICTION_CAR    40
#define MAX_FRICTION_CAR    60

// CONTROLLER PARAMETERS
#define	PID_KP	30000.0
#define	PID_KI	2000.0
#define	V_REF	20.0

// VELOCITY BOUNDARIES
#define MAX_VEL_CAR	40
#define MIN_VEL_CAR	0

/***** Real-Time Task *****/
void *vehicle(void *myVehi);
/* drives the car, do the planning
 * based on distance sensor information and raw camera info.  
 * Period: 20 ms
 */
/**************************/
 
void *initVehicle(void *c);
/* Inits vehicle position, and places randomly
 */
 
void *moveVehicle(void *myV);
/*  moves the car, for testing purposes
 */
 
void brakeVehicle(vehicle_t *myVehicle);
/* 
 */
void accelerateVehicle(vehicle_t *myVehicle);
/* 
 */

void *termVehicle(void* c);
/*	termination function to be called before killing
	the periodic task
*/
 
#endif
