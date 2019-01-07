#include <allegro.h>
#include <math.h>
#include <pthread.h>
 
#include "sensors.h"
#include "types.h"
#include "utils.h"
#include "imageprocessing.h"
 
#include <stdio.h>

pthread_mutex_t screen_lock;

void initCam(cam_t *myCam, const int x, const int y) {
	/* inits camera members
	 */
	myCam->x = x + 5;
	myCam->y = y;
	myCam->resH = HRES;
	myCam->resV = VRES;
}
 
void display(int *im, const int x0, const int y0) {
	int *p, i ,j;

	i = 0;
	j = 0;

	for (p = im; p < (im + VRES * HRES); ++p) {
		if (*p != getpixel(screen, x0 + i, y0 + j)) {	// if the curren pixel is equal than the previous
			pthread_mutex_lock(&screen_lock);			// it's not neccesary to redraw
			putpixel(screen, x0 + i, y0 + j, *p);
			pthread_mutex_unlock(&screen_lock);
		}

		j++;
		if (j == HRES) {
			j = 0;
			i++;
		}
	}
}
 

void *getFrame(vehicle_t *c) {
/* provides periodically frames acquired by camera: 33 ms
 * code:
 */
	 
	// Camera res a local variable
	double cost = cos(c->theta);
	double sint = sin(c->theta);
	
	// Camera global position
	int x0 = (c->l / 2 + VRES / 2 + CAM_OFFSET_POS) * cost + c->xr;
	int y0 = (c->l / 2 + VRES / 2 + CAM_OFFSET_POS) * sint + c->yr;
 
	// Taking picture
	int i, j;
	int x, y;
 
	for (i = 0; i < VRES; i++) {
		for(j = 0; j < HRES; j++) {
			x = (-VRES / 2 + i) * cost - (-HRES / 2 + j) * sint + x0; 
			y = (-VRES / 2 + i) * sint + (-HRES / 2 + j) * cost + y0; 
			if ((x < 0) || (y < 0)) 
				c->cam.image[i * HRES + j] = BLOCK_COL;
			else 
				c->cam.image[i * HRES + j] = getpixel(screen, x, y);
		}
	}
}
 
void place3BeamsRangefinderOnVehicle(vehicle_t *c) {
	/* Place 3 rangefinders in front left corner, front center,
	 * front right corner 90 degrees 
	 */
	c->ds.x[0] = c->point[0];				// front Right sensor 
	c->ds.y[0] = c->point[1];
	c->ds.phi[0] = c->theta + M_PI / 2.0;

	c->ds.x[2] = c->point[6];				// front left sensor
	c->ds.y[2] = c->point[7];
	c->ds.phi[2] = c->ds.phi[0] + M_PI;

	c->ds.x[1] = (c->ds.x[2] + c->ds.x[0]) / 2.0;	// front middle sensor
	c->ds.y[1] = (c->ds.y[2] + c->ds.y[0]) / 2.0;
	c->ds.phi[1] = c->theta;

}

void place4BeamsRangefinderOnVehicle(vehicle_t *c) {
	/* Place 4 rangefinders in each corner of the vehicle
	 * 45 degrees 
	 */
	int i;
	double phiRel = M_PI / 4.0 + c->theta;
 
	for (i = 0; i < 4; ++i) {
		c->ds.x[i] = c->point[2 * i];
		c->ds.y[i] = c->point[2 * i + 1];
		c->ds.phi[i] = phiRel;
		phiRel += M_PI / 2.0;
	}
}
 
void getRangefinder(dstsens_t *dst, const int beam) {
/* function to provide periodically informations acquired 
 * by the distance sensor. Period: 20 ms
 * code: 
 */
	int c, d, i, x, y, x0, y0;
	double cost, sint;

	for (i = 0; i < beam; ++i) {
		x0 = dst->x[i];			// Position of the Sensor on x
		y0 = dst->y[i];			// Position of the sensor on y
		d = SMIN;				// Minimum measured distance 
		cost = cos(dst->phi[i]);
		sint = sin(dst->phi[i]);
		do {
			x = x0 + d * cost;
			y = y0 + d * sint;
			c = getpixel(screen, x, y);
			d = d + STEP;
		} while ((d <= SMAX) && (c == STREET_COL));
		dst->dsts[i] = d;
	}
}

char isStreetColor(const int tmp) {
	return 	(tmp == TL_COL) || (tmp == TL_GREEN) ||
			(tmp == TL_YELLOW) 	|| (tmp == TL_RED) 	||
			(tmp == BLOCK_COL) || (tmp == BLACK);  
}

void  analyzeCameraFrame(vehicle_t *c, imfeatures_t *imft) {
/* Analyzes the camera frame looking for a traffic light
 * code:
 */
	int i, j;					// Iterators
	int minim = HRES * VRES;	// to get the closest traffic light
	img_t bwStreet;
	int bwTL[minim];			// bw image isolating the traffic light cabine
	int bwStreetTmp[minim];			// bw image isolating the traffic light cabine
	int tmp;					// temporal color of the image
	int tmpPos;
 
	for (i = 0; i < VRES; ++i) {
		for (j = 0; j < HRES; ++j) {

 			tmpPos = i * HRES + j;
			tmp = c->cam.image[tmpPos];

			// Isolate color of streets
			bwStreet.im[tmpPos] = isStreetColor(tmp) ? BLACK : WHITE;
			// bwStreetTmp[tmpPos] = isStreetColor(tmp) ? BLACK : WHITE;
		
			// Isolate color of Traffic Lights
			bwTL[tmpPos] = (tmp == TL_COL) ? WHITE : BLACK;

		}
	}

	// display(bwTL, c->id * 100, 620);
	RosenfeldPfaltz(bwTL, &imft->TLcenter, 0);
	fastHarrisRobertCornerDetection(&bwStreet);
	imft->stCorner = bwStreet.ft;

	/** TO BE WRAPPED UP **/
	/** Read traffic light status **/
	
	imft->TLstatus = TL_GREEN;
	for (i = 0; i < imft->TLcenter.N; i++) {
		if (imft->TLcenter.y[i + 1] > (HRES / 2)) {		// always look in the right
			if (imft->TLcenter.x[i + 1] > 5) {
				if (imft->TLcenter.x[i + 1] < minim) {		// check for the closest traffic light
					minim = imft->TLcenter.x[i + 1];
					imft->TLminDistance = minim; 
					imft->TLstatus = c->cam.image[(imft->TLcenter.x[i + 1] - 5) * HRES + imft->TLcenter.y[i + 1]];
				}
			}
		}
	}

}

/* Camera Reference frame
 * ^ x
 * |	-------------
 * |	|			|
 * |	|	FRAME	|
 * |	-------------  		y
 * ------------------------->
 * 			-----
 * 			|CAR|
 * 			|	|
 * 			-----
 */