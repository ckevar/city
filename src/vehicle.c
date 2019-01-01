#include <stdlib.h>
#include <allegro.h>
#include <math.h>
#include <pthread.h>

#include "vehicle.h"
#include "types.h"
#include "utils.h"
#include "pathplanner.h"
 
#include <stdio.h>
#include <unistd.h>
 
pthread_mutex_t screen_lock;

int idVehicle = 1;				// Vehicle ID
pthread_mutex_t id_lock;		// mutex for vehicle id

void rotatePoints(vehicle_t *myV) {

	/* 	Car - Reference Frame
	 *  
	 *	
	 * 	--------------------------------> X axis
	 *	|	x3, y3-----------x0, y0
	 * 	|	|					|
	 * 	|	|		Cx, Cy	HEAD|
	 * 	|	|					|
	 *	|	x2, y2-----------x1, y1
	 *	|
	 *  v y Axis
	 */

	double cost = cos(myV->theta);
	double sint = sin(myV->theta);

	myV->point[0] = (myV->l / 2) * cost - 
					(myV->w / 2) * sint + myV->xr;	// x0 
	myV->point[1] = (myV->l / 2) * sint + 
					(myV->w / 2) * cost + myV->yr;	// y0

	myV->point[2] = (-myV->l / 2) * cost - 
					(myV->w / 2) * sint + myV->xr;	// x1
	myV->point[3] = (-myV->l / 2) * sint + 
					(myV->w / 2) * cost + myV->yr; 	// y1

	myV->point[4] = (-myV->l / 2) * cost - 
					(-myV->w / 2) * sint + myV->xr; // x2
	myV->point[5] = (-myV->l / 2) * sint + 
					(-myV->w / 2) * cost + myV->yr; // y2

	myV->point[6] = (myV->l / 2) * cost - 
					(-myV->w / 2) * sint + myV->xr;	// x3
	myV->point[7] = (myV->l / 2) * sint + 
					(-myV->w / 2) * cost + myV->yr;	// y3
}

void pidVelController(vehicle_t *c) {
	c->e = c->Vr - c->v_1;						// Computes the error
	c->Ie += c->e;								// Integrates the error
	c->u = c->K.Kp * c->e + c->K.Ki * c->Ie;	// computes the control output
}

void dynamicCruiseModel(vehicle_t *c, double *xd, double *yd) {
	/* Dynamic model
	 * Vk = (T / (m + bT)) * F + (m / (m + bT))*Vk-1
	 * Xk = Xk-1 + Vk*T
	 */
	c->vel = (c->T / (c->m + c->b * c->T) ) * c->u + (c->m / (c->m + c->b * c->T)) * c->v_1;
	c->dx = c->vel * c->T;			// distance covered by c->vel;

	*xd += c->dx * cos(c->theta);	// New position over x
	*yd += c->dx * sin(c->theta);	// New position over y

	c->xr = *xd; // New position over x
	c->yr = *yd;

	if (c->theta <= M_PI / 2) {		// Ceiling due to double-to-int casting  
		c->xr = ceil(*xd);       	// New position over x
		c->yr = ceil(*yd);   		
	} 	
}

void *moveVehicle(void *myV) {
	vehicle_t *c = (vehicle_t *) myV;			// Casting vehicle type
	double xd;									// Taking as double the current position
	double yd;									// this one too
	int tmpPosition[8];

	cpyPnts(c->point, tmpPosition, 8);			// copying temporal position 

	/** Reading Sensors **/
	getRangefinder(&c->ds, RANGEFINDER_3_BEAMS);
	getFrame(c);
	pathPlanner(c);

	xd = c->xr;
	yd = c->yr;

	/** PI controller **/
	pidVelController(c);

	/* Dynamic model */
	dynamicCruiseModel(c, &xd, &yd);

	rotatePoints(c);                        // compute new points to plot
	place3BeamsRangefinderOnVehicle(c);     // 
	
	if (cmpPnts(tmpPosition, c->point, 8)) { 	// If the points are the same as the previous one, 
												// the car wont be reploted.
		/** DELETING PREVIOUS POSITION **/
		pthread_mutex_lock(&screen_lock);           	// Locking shared resource screen
		polygon(screen, 4, tmpPosition, STREET_COL); 	// Delete previous position
	
		/** DRAWING NEW POSITION **/
		polygon(screen, 4, c->point, c->color); 		// Draw new position
		circlefill(screen, c->xr - c->l/4 * cos(c->theta), c->yr - c->l/4 * sin(c->theta), c->w / 2 - 3, BLACK);
		pthread_mutex_unlock(&screen_lock);
	}

	c->v_1 = c->vel;
}
 
char isAvailableThisColor(int color) {
	if (color == BLOCK_COL) return 0;
	if (color == TL_YELLOW) return 0;
	if (color == TL_RED) 	return 0;
	if (color == TL_GREEN)	return 0;
	if (color == STREET_COL) return 0;
	if (color == BLACK) 	return 0;
	if (color == 14) return 0;	// yellow, too clear
	return 1;
}
 
void auxMoveY(vehicle_t * myV, const int i, const int sgn) {
	/* Moves the vehicle according i and sng along Y axis:
	 * - SGN is the direction of displacement
	 * - I is the point under test
	 */
	do {
		myV->yr = myV->yr + sgn;
		rotatePoints(myV);	// only translation is needed
	} while(getpixel(screen, myV->point[2 * i], myV->point[2 * i + 1]) != STREET_COL);

	myV->yr = myV->yr + sgn * 10;
	rotatePoints(myV);	// only translation is needed
}
 
void auxMoveX(vehicle_t * myV, const int i, const int sgn) {
	/* Moves the vehicle according i and sng along X axis:
	 * - SGN is the direction of displacement
	 * - I is the point under test
	 */
	do {
		myV->xr = myV->xr + sgn;
		rotatePoints(myV);	// only translation is needed
	} while(getpixel(screen, myV->point[2 * i], myV->point[2 * i + 1]) != STREET_COL);

	myV->xr = myV->xr + sgn * 10;
	rotatePoints(myV);	// only translation is needed
}
 
void placeCarInStreet(vehicle_t *myV, const char axis) {
	/* Axis: Reference initial orientation 
	 * 0: 90 deg, 1: 270deg, 2: 0 deg, 3: 180deg
	 */
	int i;
	for (i = 0; i < 4; ++i) {
		if (getpixel(screen, myV->point[2 * i], myV->point[2 * i + 1]) != STREET_COL) {
							//		   x point,			y point 				
			
			if (i == 0) {								// if the first corner
				if (axis == 0) auxMoveX(myV, i, 1);		// 90 deg, positive displacement
				if (axis == 1) auxMoveX(myV, i, -1);	// 270 deg, negative displacement
				if (axis == 2) auxMoveY(myV, i, -1);	// 0 deg, negative displacement
				if (axis == 3) auxMoveY(myV, i, 1);		// 180 deg, positive displacement
			
			} else if (i == 3) {						// if the 4th corner
				if (axis == 0) auxMoveX(myV, i, -1);	// 90 deg, negative displacement
				if (axis == 1) auxMoveX(myV, i, 1);		// 270 deg, positive displacement
				if (axis == 2) auxMoveY(myV, i, 1);		// 0 deg, positive displacement
				if (axis == 3) auxMoveY(myV, i, -1);	// 180 deg, negative displacement

			}
		}
	}
}
 
void initCarFeatures(vehicle_t *c) {

	pthread_mutex_lock(&id_lock);
	c->id = idVehicle;		// Assigns a unique vehicle id;
	idVehicle++;				// increases the id for the next vehicle
	pthread_mutex_unlock(&id_lock);

	c->l = VEHICLE_LENGTH;	// Length of the vehicle
	c->w = VEHICLE_WIDTH;		// Width of vehicle
	c->m = MIN_MASS_CAR + rand() % (MAX_MASS_CAR - MIN_MASS_CAR);
	c->b = MIN_FRICTION_CAR + rand() % (MAX_FRICTION_CAR - MIN_FRICTION_CAR);
	c->K.Kp = PID_KP;			// Proportional Gain
	c->K.Ki = PID_KI;			// Integral gain
	c->dx = 0.0;				// covered distance 
	c->Vr = V_REF;				// Velocity set Point (could be random per each car)
	c->T  = 0.033;				// Period
	c->Ie = 0;					// Integrating the error
	c->v_1 = 0;					// Initializing previous speed
	c->isExecuted = 0;			// steering execution event
	c->turn = DONT_STEER;		// starts as no init
	c->planner.angleRes = M_PI / 32;	// angle resolution for steering
}

char generateRandomPositionAroundCity(vehicle_t *c) {
	char theta;						/* Value to return. Reference initial, orientation 
									 * 0: 90 deg, 1: 270deg, 2: 0 deg, 3: 180deg
									 */

	int nb;							// Number of blocks per axis
	int blkLen = BLOCK_W + STREET_W;// length of a block (+ street width)
	int XorY = rand() % 2;			// it shall generate the car over x or y axis
	int ULorBR = rand() % 2;		/* it shall generate the car Upper or Buttom or
									 * Left or Right, according the result of XorY
									 */
	if (XorY) {
		/* Along X axis and (by default) Top Screen */
		c->yr = STREET_W;						// Initial position along y axis
		c->theta = M_PI / 2.0; 					// 90 deg
		theta = 0;								// Flag representeng 90 deg

		if (!ULorBR) {
			/* Bottom Side of screen */
			theta = 1;
			nb = (H - STREET_W) / blkLen;		// Number of blocks along y axis
			c->theta += M_PI;					// 270 	deg
			c->yr += nb * blkLen - STREET_W;
			/*					|________ where counting from bottom
			 */
		}
		nb = (W - STREET_W) / blkLen;			// Number of blocks along x axis
		do { c->xr = STREET_W + rand()%(nb * blkLen);	/* Random position between first corner
														 * up to rightest corner of the rightest block
														 */
		} while(getpixel(screen, c->xr, c->yr) != STREET_COL);

	} else {
		/* Along Y axis and (by default) left screen */
		c->xr = STREET_W;						// Initial position along x axis
		c->theta = 0.0;							// 0 deg
		theta = 2;								// Flag representing 0 deg

		if (!ULorBR) {
			/* Right Side of Screen */
			theta = 3;
			nb = (W - STREET_W) / blkLen;		// Number of blocks along x axis
			c->theta += M_PI; 					// 180 deg
			c->xr += nb * blkLen - STREET_W;	// Rightest corner of the rightest block
			/*					|________ where counting from bottom
			 */
		}

		nb = (H - STREET_W) / blkLen;			// Number of blocks along y axis

		do { c->yr = STREET_W + rand()%(nb * blkLen); 	/* Random position between
															 * first corner til last corner
															 */
		} while(getpixel(screen, c->xr, c->yr) != STREET_COL);
	}

	c->theta_old = c->theta;							// this is steering purposes
	return theta;
}

void *initVehicle(void *c) {
	/* inits vehicle variables
	 * code:
	 */
	vehicle_t * myV = (vehicle_t *) c; // casting to vehicle type
	char quadrant;

	/** Car features **/
	initCarFeatures(myV);

	/** Generate random position **/
	quadrant = generateRandomPositionAroundCity(myV);

	// Random color but choose an available one
	do {
		myV->color = rand() % MAX_KNOWN_ENCODED_COLORS;
	} while(!isAvailableThisColor(myV->color));

	// Rotate the points to plot
	rotatePoints(myV);

	placeCarInStreet(myV, quadrant);
	place3BeamsRangefinderOnVehicle(myV);
	initCam(&myV->cam, myV->xr, myV->yr);

	pthread_mutex_lock(&screen_lock);
	polygon(screen, 4, myV->point, myV->color);
	pthread_mutex_unlock(&screen_lock);
}
 
