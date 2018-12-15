#include "types.h"
 
#ifndef SENSORS_H
#define SENSORS_H
 
#include "vehicle.h"
 
/** RANGEFINDER FEATURES **/
#define SMIN 4
#define SMAX 30
#define STEP 1
#define RANGEFINDER_4_BEAMS 4
 
/**** REAL-TIME TASKS ****/
void *getFrame(vehicle_t *myVehicle);
/************************/
void getRangefinder(dstsens_t *dst, const int beam);
 
void initCam(cam_t *myCam, const int x, const int y);
/* Inits camera features
 */
 
void analyzeCameraFrame(vehicle_t *myVehicle);
/* analyze the camera frame
 */
 
void place4BeamsRangefinderOnVehicle(vehicle_t *c);
/* Inits sensors positions and angle of measurement
 */
#endif
