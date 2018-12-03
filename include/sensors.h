#ifndef SENSORS_H
#define SENSORS_H

#include "vehicle.h"
#include "types.h"

// struct dstsens_;
// struct cam_;
// struct gps_;

void *getFrame(vehicle_t *myVehicle);
void *getRangefinder(vehicle_t *myVehicle);

#endif