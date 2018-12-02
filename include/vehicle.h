#include "sensors.h"

#ifndef VEHICLE_H
#define VEHICLE_H 

typedef struct
{
	const int		color;
	const double	mass;
	const double	friction;
	double 			controlU;
	double			posXr;	 // real (or granted) position
	double			posYr; 	// real (or granted) position
	double			orientation;
	double			velocity;
	dstsens_t		*dsensor;
	cam_t 			*camera;	
	gps_t 			*gps;
} vehicle_t;

void *drive(vehicle_t *myVehicle);

#endif