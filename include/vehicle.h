#include "sensors.h"

#ifndef VEHICLE_H
#define VEHICLE_H 

#define VEHICLE_LENGTH	30
#define VEHICLE_WIDTH 	20

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
/*  
 */
void brakeVehicle(vehicle_t *myVehicle);
/* 
 */
void accelerateVehicle(vehicle_t *myVehicle);
/* 
 */

#endif
