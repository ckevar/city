#include <allegro.h>
#include <math.h>
#include "sensors.h"
#include "types.h"


void initCam(cam_t *myCam, const int x, const int y) {
	/* inits camera members
	 */
	myCam->x = x + 5;
	myCam->y = y;
	myCam->resH = HRES;
	myCam->resV = VRES;
}

void *getFrame(vehicle_t *myVehicle) {
/* provides periodically frames acquired by camera: 33 ms
 * code:
 */	
	// Camera global position
	unsigned x0 = myVehicle->camera.x;
	unsigned y0 = myVehicle->camera.y;
	
	// Camera res a local variable
	unsigned hRes = myVehicle->camera.resH; 
	unsigned vRes = myVehicle->camera.resV;

	// Taking picture
	unsigned i, j;
	unsigned x, y;
	for (i = 0; i < hRes; ++i) {
		for(j = 0; j < vRes; j++) {
			x = x0 - hRes/2 + i;
			y = y0 - vRes/2 + j;
			myVehicle->camera.image[i][j] = getpixel(screen, x, y);
		}
	}

}


void *getRangefinder(vehicle_t *myVehicle) {
/* function to provide periodically informations acquired 
 * by the distance sensor. Period: 20 ms
 * code: 
 */
	int c;
	int x, y;
	int d = SMIN;
	int x0 = myVehicle->xr;
	int y0 = myVehicle->yr;

	double alpha = myVehicle->theta;
	
	do {
		x = x0 + d * cos(alpha);
		y = y0 + d * cos(alpha);
		c = getpixel(screen, x, y);
		d = d + STEP;
	} while ((d <= SMAX) && (c == 8));
}

void  analyzeCameraFrame(vehicle_t *myVehicle) {
/* Analyzes the camera frame looking for a traffic light
 * code:
 */
}
