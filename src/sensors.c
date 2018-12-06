#include "types.h"

#ifndef SENSORS_H
#define SENSORS_H

#include "vehicle.h"

/** RANGEFINDER FEATURES **/
#define SMIN 10
#define SMAX 100
#define STEP 1

/** CAMERA FEATURES **/
#define HRES 16
#define VRES 9


void initCam(cam_t *myCam, const int x, const int y);

/**** REAL-TIME TASKS ****/
void *getFrame(vehicle_t *myVehicle);
void *getRangefinder(vehicle_t *myVehicle);
/************************/

void analyzeCameraFrame(vehicle_t *myVehicle);
/* analyze the camera frame
 */
#endif
