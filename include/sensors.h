#ifndef SENSORS_H
#define SENSORS_H

#include "vehicle.h"
#include "types.h"


void *getFrame(vehicle_t *myVehicle);
void *getRangefinder(vehicle_t *myVehicle);

#endif