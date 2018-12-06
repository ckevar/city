#include <stdlib.h>
#include <allegro.h>
#include <math.h>
#include "vehicle.h"
#include "types.h"
#include "utils.h"

#include <stdio.h>

void *drive(vehicle_t *myVehi) {

}

void rotatePoints(vehicle_t *myV) {

	double costheta = cos(myV->orientation);
	double sintheta = sin(myV->orientation);

	myV->point[0] = (-myV->height / 2) * costheta - 
			   (-myV->width / 2) * sintheta + myV->xr;

	myV->point[1] = (-myV->height / 2) * sintheta + 
			   (-myV->width / 2) * costheta + myV->yr;

	myV->point[2] = (myV->height / 2) * costheta - 
			   (-myV->width / 2) * sintheta + myV->xr;

	myV->point[3] = (myV->height / 2) * sintheta + 
			   (-myV->width / 2) * costheta + myV->yr;

	myV->point[4] = (myV->height / 2) * costheta - 
			   (myV->width / 2) * sintheta + myV->xr;

	myV->point[5] = (myV->height / 2) * sintheta + 
			   (myV->width / 2) * costheta + myV->yr;

	myV->point[6] = (-myV->height / 2) * costheta - 
			   (myV->width / 2) * sintheta + myV->xr;

	myV->point[7] = (-myV->height / 2) * sintheta + 
			   (myV->width / 2) * costheta + myV->yr;
}

char isAvailableThisColor(int color) {
	if (color == BLOCK_COL) return 0;
	if (color == 14) return 0; 	// YELLOW
	if (color == 12) return 0;	// RED
	if (color == 10) return 0;	// GREEN
	if (color == 1) return 0;
	return 1;
}

void auxMoveY(vehicle_t * myV, const int i, const int sgn) {
	do {
		myV->yr = myV->yr + sgn;
		rotatePoints(myV);
	} while(getpixel(screen, myV->point[2 * i], myV->point[2 * i + 1]) != STREET_COL);
	myV->yr = myV->yr + sgn * 10;
}

void auxMoveX(vehicle_t * myV, const int i, const int sgn) {
	do {
		myV->xr = myV->xr + sgn;
		rotatePoints(myV);
	} while(getpixel(screen, myV->point[2 * i], myV->point[2 * i + 1]) != STREET_COL);
	myV->xr = myV->xr + sgn * 10;
}

void placeCarInStreet(vehicle_t *myV, const int axis) {
	int i;
	for (i = 0; i < 4; ++i) {
		if (getpixel(screen, myV->point[2 * i], myV->point[2 * i + 1]) != STREET_COL) {

			printf("i %d, Axis %d\n", i, axis);

			if (i == 1) {
				if ((axis == 0) || (axis == 1)) auxMoveX(myV, i, -1);
				if (axis == 2) auxMoveY(myV, i, -1);
				if (axis == 3) auxMoveY(myV, i, 1);
			
			} else if (i == 2) {
				if (axis == 0) auxMoveX(myV, i, 1);
				if (axis == 1) auxMoveX(myV, i, -1);
				if (axis == 2) auxMoveY(myV, i, -1);
				if (axis == 3) auxMoveY(myV, i, 1);

			}
		}
	}
	printf("-------\n");

}

void initVehicle(vehicle_t *myV, const int w, const int h) {
	/* inits vehicle variables
	 * code:
	 */

	int XorY = rand() % 2;			// it shall generate the car over x or y axis
	int ULorBR = rand() % 2;		/* it shall generate the car Upper or Buttom or
									 * Left or Right
									 */
	int orientation;
	
	myV->height = VEHICLE_LENGTH;	// Length of the vehicle
	myV->width = VEHICLE_WIDTH;		// Width of vehicle

	if (XorY) {
		/* Along X axis and (by default)
		 */
		myV->yr = STREET_W;
		myV->orientation = M_PI / 2.0; 		// 90 deg
		orientation = 0;

		if (!ULorBR) {
			orientation = 1;
			myV->orientation += M_PI;		// 270 	deg
			myV->yr += h - 2 * STREET_W - STREET_W;
			/*					|________ where counting from bottom
			 */
		}

		do { myV->xr = STREET_W + rand()%(w - 2 * STREET_W);
		} while(getpixel(screen, myV->xr, myV->yr) != STREET_COL);

	} else {
		/* Along Y axis and (by default) left screen 
		 */
		myV->xr = STREET_W;
		myV->orientation = 0.0;				// 0 deg
		orientation = 2;

		if (!ULorBR) {
			/* Right Side of Screen
			 */
			orientation = 3;
			myV->orientation += M_PI; 		// 180 deg
			myV->xr += w - 2.5 * STREET_W - STREET_W;
			/*					|________ where counting from bottom
			 */
		}

		do { myV->yr = STREET_W + rand()%(h - 2 * STREET_W);
		} while(getpixel(screen, myV->xr, myV->yr) != STREET_COL);
	}

	do {
		myV->color = rand()%MAX_KNOWN_ENCODED_COLORS;
	} while(!isAvailableThisColor(myV->color));

	rotatePoints(myV);

	placeCarInStreet(myV, orientation);
	initCam(&myV->camera, myV->xr, myV->yr);
	rotatePoints(myV);
	polygon(screen, 4, myV->point, myV->color);
	// polygon(screen, 4, myV->point, color);

}
