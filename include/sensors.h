#include "types.h"

#ifndef SENSORS_H
#define SENSORS_H

#include "vehicle.h"

/**** REAL-TIME TASKS ****/
void *getFrame(vehicle_t *myVehicle);
void *getRangefinder(vehicle_t *myVehicle);
/************************/

void analyzeCameraFrame(vehicle_t *myVehicle);
/* analyze the camera frame
 */
#endif