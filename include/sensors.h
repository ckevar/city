#include "types.h"
 
#ifndef SENSORS_H
#define SENSORS_H
 
#include "vehicle.h"
 
/** RANGEFINDER FEATURES **/
#define SMIN 3					// min distance of measurement
#define SMAX 30					// max distance of measurement
#define STEP 1 					// distance sensor resolution
#define RANGEFINDER_4_BEAMS 4	// beams to measure distance
#define RANGEFINDER_3_BEAMS 3	// beams to measure distance
#define RIGHT_DST	0		// index of front right distance sensor 
#define MID_DST		1		// index of front middle distance sensor
#define LEFT_DST	2 		// index of front left distance sensor

/**** REAL-TIME TASKS ****/
void *getFrame(vehicle_t *myVehicle);
/************************/
void getRangefinder(dstsens_t *dst, const int beam);
 
/* Inits camera features
 */
void initCam(cam_t *myCam, const int x, const int y);
 
/* analyze the camera frame
 */
void analyzeCameraFrame(vehicle_t *myVehicle, imfeatures_t *imft);
 
/* Inits distance sensors positions and angle of measurement
 */
void place4BeamsRangefinderOnVehicle(vehicle_t *c);

/* Inits distance sensors positions and angle of measurement
 */
void place3BeamsRangefinderOnVehicle(vehicle_t *c);

#endif
