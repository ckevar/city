#ifndef VEHICLE_H
#define VEHICLE_H 

typedef struct vehicle_
{
	int 		color;
	double 		mass;
	double 		friction;
	double 		controlU;
	distSens_t  *dsensor;
	cam_t 		*camera;	
	double		posXr;	 // real (or granted) position
	double		posYr; 	// real (or granted) position
	double		orientation;
	double		velocity;
	gps_t 		*gps;
} vehicle_t;

void *drive(vehicle_t *myVehicle);

#endif