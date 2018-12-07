#include "sensors.h"

#ifndef VEHICLE_H
#define VEHICLE_H 

// VEHICLE DIMENSIONS
#define VEHICLE_LENGTH	30
#define VEHICLE_WIDTH 	20

// VEHICLE BOUND FEATURES
#define MIN_MASS_CAR		990
#define MAX_MASS_CAR		2000
#define MIN_FRICTION_CAR	40
#define MAX_FRICTION_CAR	60

// CONTROLLER PARAMETERS
#define  PID_KP 800.0;
#define  PID_KI 100.0;

/***** Real-Time Task *****/
void *drive(vehicle_t *myVehicle);
/* drives the car, do the planning
 * based on distance sensor information and raw camera info.  
 * Period: 20 ms
 */
/**************************/

void initVehicle(vehicle_t *myV, const int w, const int h);
/* Inits vehicle position, and places randomly
 */

void moveVehicle(vehicle_t *myVehicle);
/*  moves the car, for testing purposes
 */

void brakeVehicle(vehicle_t *myVehicle);
/* 
 */
void accelerateVehicle(vehicle_t *myVehicle);
/* 
 */

#endif
