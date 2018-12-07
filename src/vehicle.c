#include <stdlib.h>
#include <allegro.h>
#include <math.h>
#include "vehicle.h"
#include "types.h"
#include "utils.h"

#include <stdio.h>
#include <unistd.h>

void *drive(vehicle_t *myVehi) {
}


void rotatePoints(vehicle_t *myV) {

	double cost = cos(myV->theta);
	double sint = sin(myV->theta);

	myV->point[0] = (-myV->l / 2) * cost - 
			   (-myV->w / 2) * sint + myV->xr;

	myV->point[1] = (-myV->l / 2) * sint + 
			   (-myV->w / 2) * cost + myV->yr;

	myV->point[2] = (myV->l / 2) * cost - 
			   (-myV->w / 2) * sint + myV->xr;

	myV->point[3] = (myV->l / 2) * sint + 
			   (-myV->w / 2) * cost + myV->yr;

	myV->point[4] = (myV->l / 2) * cost - 
			   (myV->w / 2) * sint + myV->xr;

	myV->point[5] = (myV->l / 2) * sint + 
			   (myV->w / 2) * cost + myV->yr;

	myV->point[6] = (-myV->l / 2) * cost - 
			   (myV->w / 2) * sint + myV->xr;

	myV->point[7] = (-myV->l / 2) * sint + 
			   (myV->w / 2) * cost + myV->yr;
}

void moveVehicle(vehicle_t *c) {
	double deltax;		// how much the car was moved
	double T = 1;		// Period
	double Vref = 0.4;	// Velocity set Point (could be random per each car)
	double e;			// Error betwee Set Point and and Output 
	double Ie = 0;		// Integrating the error

	while(1) {
		polygon(screen, 4, c->point, STREET_COL);	// Delete previous position
		/** PI controller **/
		e = Vref - c->v_1;					// Computes the error
		Ie += e; 							// Integrates the error
		c->u = c->K.Kp * e + c->K.Ki * Ie;	// computes the control output

		/* Dynamic model
		 * Vk = (T / (m + bT)) * F + (m / (m + bT))*Vk-1
		 * Xk = Xk-1 + Vk*T
		 */
		c->vel = (T / (c->m + c->b * T) ) * c->u + (c->m / (c->m + c->b * T)) * c->v_1;
		deltax +=  c->vel * T;				// distance covered by c->vel;
		c->xr += deltax * cos(c->theta);	// New position
		c->yr += deltax * sin(c->theta);	// New position
		printf("vel %f\n", c->vel);
		rotatePoints(c);							// compute new points to plot
		polygon(screen, 4, c->point, c->color);	// Draw new position
		c->v_1 = c->vel;
		sleep(1);
	}
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
}

void initVehicle(vehicle_t *myV, const int w, const int h) {
	/* inits vehicle variables
	 * code:
	 */

	int theta;						/* Reference initial, orientation, 0: 90deg, 
									   1: 270deg, 2: 0deg, 3: 180deg
									 */
	int XorY = rand() % 2;			// it shall generate the car over x or y axis
	int ULorBR = rand() % 2;		/* it shall generate the car Upper or Buttom or
									 * Left or Right
									 */
	/*** Car features ***/
	myV->l = VEHICLE_LENGTH;	// Length of the vehicle
	myV->w = VEHICLE_WIDTH;		// Width of vehicle
	myV->m = MIN_MASS_CAR + rand() % (MAX_MASS_CAR - MIN_MASS_CAR);
	myV->b = MIN_FRICTION_CAR + rand() % (MAX_FRICTION_CAR - MIN_FRICTION_CAR);
	myV->K.Kp = PID_KP;			//	Proportional Gain
	myV->K.Ki = PID_KI;			//	Integral gain
	/********************/

	if (XorY) {
		/* Along X axis and (by default)
		 */
		myV->yr = STREET_W;
		myV->theta = M_PI / 2.0; 		// 90 deg
		theta = 0;						

		if (!ULorBR) {
			theta = 1;
			myV->theta += M_PI;			// 270 	deg
			myV->yr += h - 3 * STREET_W - STREET_W;
			/*					|________ where counting from bottom
			 */
		}

		do { myV->xr = STREET_W + rand()%(w - 2 * STREET_W);
		} while(getpixel(screen, myV->xr, myV->yr) != STREET_COL);

	} else {
		/* Along Y axis and (by default) left screen 
		 */
		myV->xr = STREET_W;
		myV->theta = 0.0;				// 0 deg
		theta = 2;

		if (!ULorBR) {
			/* Right Side of Screen
			 */
			theta = 3;
			myV->theta += M_PI; 		// 180 deg
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

	placeCarInStreet(myV, theta);
	initCam(&myV->camera, myV->xr, myV->yr);
	rotatePoints(myV);
	polygon(screen, 4, myV->point, myV->color);
	// polygon(screen, 4, myV->point, color);

}
