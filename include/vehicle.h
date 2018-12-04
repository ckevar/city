#include "sensors.h"
#include "types.h"

#ifndef VEHICLE_H
#define VEHICLE_H 

/***** Real-Time Task *****/
void *drive(vehicle_t *myVehicle);
/* drives the car, do the planning
 * based on distance sensor information and raw camera info.  
 * Period: 20 ms
 */
/**************************/

void initVehicle(vehicle_t *myV);
/* Inits vehicle position
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