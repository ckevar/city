#ifndef PATH_PLANNER_H
#define PATH_PLANNER_H 

#include "types.h"


// SAFE DISTANCES FROM VEHICLE
#define DST_FROM_TL2STOP		9	// Distance to brake from the car to the red light of traffic light 
#define DST_FROM_LEFT_SIDEBLOCK	10	// Distance between the car and the left side block  
#define	DST_FROM_FRONTOBSTACLE	10	// Distance between front obstacle to the car
#define	DST_TO_STEER			2	// Distance from the closest corner (measured in pixels in the image)

// Coefficient for generating K / X steering trajectory
#define COEFF_INVX				40.0	// Coefficient K of the K / X function to generate the trajectory 
#define TRANSLATION_INVX_LEFT 	40		// tranlation y = K / (x - TRANSLATION_INVX_LEFT)
#define TRANSLATION_INVX_RIGHT 	25		// tranlation y = K / (x - TRANSLATION_INVX_LEFT)

// Controll Coefficients
#define	MAX_KFORCE	1500.0				// Max force by controller

void steerLR(vehicle_t *c, imfeatures_t *ft);
void chooseSteering(vehicle_t *c, imfeatures_t *ft);
void genEllipticalTrajectory(vehicle_t *c, imfeatures_t *ft);
void steer(vehicle_t *c, imfeatures_t *ft);
char pathPlanner(vehicle_t *c);

#endif