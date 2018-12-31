#include <stdlib.h>
#include <allegro.h>
#include <math.h>
#include <pthread.h>
#include "vehicle.h"
#include "types.h"
#include "utils.h"
 
#include <stdio.h>
#include <unistd.h>
 
pthread_mutex_t screen_lock;

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

 
void keepCertainDistanceFromBlock(vehicle_t *c, const int safeDistance) {
	if (c->ds.dsts[RIGHT_DST] > safeDistance) {
		if (c->ds.dsts[RIGHT_DST] < SMAX) {
			// c->theta = c->theta_old + M_PI / 4.0;
			c->xr += cos(c->theta_old) - sin(c->theta_old);
			c->yr += sin(c->theta_old) + cos(c->theta_old);
		}

	} else 
	if(c->ds.dsts[RIGHT_DST] < (SMIN + 5)) {
	 	c->xr -= cos(c->theta_old) - sin(c->theta_old);
	 	c->yr -= sin(c->theta_old) + cos(c->theta_old);
	} 
	// else {
	// 	c->theta = c->theta_old;
	// }
}

void steerLR(vehicle_t *c, imfeatures_t *ft) {

	double u, w;		// Vehicle frames
	u = c->planner.e.a * cos(c->planner.alpha);	//

	switch(c->turn) {
		case STEER_RIGHT:
			w = c->planner.e.b * (sin(c->planner.alpha) + 1);
			c->theta += c->planner.angleRes;			// Increases global angle of the vehicle
			c->planner.alpha += c->planner.angleRes;	// Increases angle of the trajectory
			break;

		case STEER_LEFT:
			w = c->planner.e.b * (sin(c->planner.alpha) - 1);
			c->theta -= c->planner.angleRes;			// Decreases global angle of the vehicle
			c->planner.alpha -= c->planner.angleRes;  	// Decreases angle of the trajectory
			break; 

		case TURN_180:
			// Working ... :'(
			break;

		default:		
			fprintf(stderr, "Something went wrong \n");
	}

	if (fabs(c->planner.alpha) > M_PI / 2.0) {

		if (c->turn == STEER_RIGHT) 
			c->theta = c->theta_old + M_PI / 2.0;	
		
		else if (c->turn == STEER_LEFT) 
			c->theta = c->theta_old - M_PI / 2.0;	
		
		
		if (c->theta >= 2 * M_PI)	// bringing to the first quadrant  
			c->theta -= 2 * M_PI;

		else if (c->theta < 0) 		//	bringin to positive angle
			c->theta += 2 * M_PI;
		
		c->theta_old = c->theta;
		c->Vr = V_REF;
		c->isExecuted = 0;			// Crossroad state is released

	} else {
		c->xr += round(u * cos(c->theta_old) - w * sin(c->theta_old)); 
		c->yr += round(u * sin(c->theta_old) + w * cos(c->theta_old));

		c->isExecuted = 1;			// still working on Crossroad state
		c->Vr = 0;					// Reference velocity 0
	}
}

void chooseSteering(vehicle_t *c, imfeatures_t *ft) {
			
	c->isExecuted = 1;				// Crossroad state needs to be executed

	switch(ft->stCorner.N) {		// The steering will be chosen according to the number of corners detected
		case 1: 
			// In case of the corners of the city (boundary)
			c->turn = (ft->stCorner.y[0] < (HRES / 2)) ? STEER_LEFT : STEER_RIGHT;
			break;

		case 2:
			if (!(ft->stCorner.x[0] - ft->stCorner.x[1])) {      		// T shape street, car comming from center of T
				c->turn = (rand() % 2) ? STEER_LEFT : STEER_RIGHT;

			} else if ((ft->stCorner.y[0] - ft->stCorner.y[1]) > 0) {	// L shape street
				c->turn = STEER_RIGHT;

			}  else if ((ft->stCorner.y[0] - ft->stCorner.y[1]) < 0) {	// Inverted L shape street
				c->turn = STEER_LEFT;

			} else {													// T shape vehicle coming from sides
				if (ft->stCorner.y[0] < (HRES / 2))         			// Car is comming from right side of the T
					c->turn = (rand() % 2) ? STEER_LEFT : DONT_STEER;	
				
				else													// Car is comming from left side of the T
					c->turn = (rand() % 2) ? STEER_RIGHT : DONT_STEER;	
			} 

			break;

		case 4:		
				c->turn = rand() % 3;									// If it's a full crossrode, it can steering anyway
				break;
	}

	c->planner.alpha = 0;												// Starts the angle for the elliptical shape steering
}

void genTrajectory(vehicle_t *c, imfeatures_t *ft) {
/* generate basic parameters to steer in an elliptical shape
 */	
	int i;

	if (c->turn == STEER_LEFT) {
		c->planner.e.b = (c->ds.dsts[LEFT_DST] + c->w / 2) / 10;	// Axis along x
		c->planner.e.a = c->planner.e.b;							// By default if there is no corner bigger than this corner

		for (i = 1; i < ft->stCorner.N; ++i) {						// Iterates to choose the longer corner distance from vehicle
			if (ft->stCorner.x[i] > ft->stCorner.x[i - 1]) {		// as
				c->planner.e.a = (ft->stCorner.x[i] - c->w / 2 - 1) / 10;
			}
		}

	} else if (c->turn == STEER_RIGHT) {
		c->planner.e.b = (c->ds.dsts[RIGHT_DST]) / 10;
		c->planner.e.a = (c->ds.dsts[RIGHT_DST] + c->w * 2.2) / 10;		
	}			
}

void steer(vehicle_t *c, imfeatures_t *ft) {
	if (!c->turn) {	// If not steering
		if (!ft->stCorner.N)	// Keeps the crossrode state until there is no corners
			c->isExecuted = 0;	// End of crossroad state
	}
	else steerLR(c, ft); // If steering, left or right
}

void pathPlanner(vehicle_t *c) {
	/* To plan the future path for the car
	 */

	imfeatures_t imf;

	/***** FRONT DISTANCE RESPONSE *****/
	if (c->ds.dsts[MID_DST] < 10) c->Vr = 0;	// Minimun Distance before hit wall from front
	/***********************************/

	/****** TRAFFIC LIGHT REPONSE ******/
	if (imf.TLcenter.N) {        		// No traffic lights detected
		if (imf.TLminDistance < 9)
			if ((imf.TLstatus == TL_RED) || (imf.TLstatus == TL_YELLOW))
				c->Vr = 0.0;

		else if (imf.TLstatus == TL_GREEN)
			c->Vr = 10.0;
	}
	/************************************/
	
	analyzeCameraFrame(c, &imf);	

	/******** CROSSROAD RESPONSE ********/
	if (!c->isExecuted) {

		keepCertainDistanceFromBlock(c, 10);	// keep safe distance from blocks

		if ((imf.stCorner.x[0] < 2) && (imf.stCorner.N)) {
			chooseSteering(c, &imf);	// the vehicle will go left, right or straigh
			genTrajectory(c, &imf);		// generates basic parameters to compute the steering trajectory
		}
	}

	if (c->isExecuted) 
		steer(c, &imf);					// steers
	
	/***********************************/
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

	if (c->theta <= M_PI / 2) { //  
		c->xr = ceil(*xd);       // New position over x
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
	
	if (cmpPnts(tmpPosition, c->point, 8)) { 	/* if the points are the same as the previous one, 
												 * the car wont be reploted
												 */
		/** DELETING PREVIOUS POSITION **/
		pthread_mutex_lock(&screen_lock);           	// locking shared resource screen
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
 
void placeCarInStreet(vehicle_t *myV, const int axis) {
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
 
void *initVehicle(void *c) {
	/* inits vehicle variables
	 * code:
	 */
	vehicle_t * myV = (vehicle_t *) c; // casting to vehicle type
	int theta;						/* Reference initial, orientation 
									 * 0: 90 deg, 1: 270deg, 2: 0 deg, 3: 180deg
									 */

	int nb;							// Number of blocks per axis
	int blkLen = BLOCK_W + STREET_W;// length of a block (+ street width)
	int XorY = rand() % 2;			// it shall generate the car over x or y axis
	int ULorBR = rand() % 2;		/* it shall generate the car Upper or Buttom or
									 * Left or Right, according the result of XorY
									 */

	/*************** Car features ***************/
	myV->l = VEHICLE_LENGTH;	// Length of the vehicle
	myV->w = VEHICLE_WIDTH;		// Width of vehicle
	myV->m = MIN_MASS_CAR + rand() % (MAX_MASS_CAR - MIN_MASS_CAR);
	myV->b = MIN_FRICTION_CAR + rand() % (MAX_FRICTION_CAR - MIN_FRICTION_CAR);
	myV->K.Kp = PID_KP;			// Proportional Gain
	myV->K.Ki = PID_KI;			// Integral gain
	myV->dx = 0.0;				// covered distance 
	myV->Vr = V_REF;			// Velocity set Point (could be random per each car)
	myV->T 	= 0.033;			// Period
	myV->Ie = 0;				// Integrating the error
	myV->v_1 = 0;				// Initializing previous speed
	myV->isExecuted = 0;		// steering execution event
	myV->turn = DONT_STEER;		// starts as no init
	myV->planner.angleRes = M_PI / 32;	// angle resolution for steering
	/*********************************************/


	// GENERATE RANDOM POSITION
	if (XorY) {
		/* Along X axis and (by default) Top Screen
		 */
		myV->yr = STREET_W;						// Initial position along y axis
		myV->theta = M_PI / 2.0; 				// 90 deg
		theta = 0;								// Flag representeng 90 deg

		if (!ULorBR) {
			/* Bottom Side of screen
			 */
			theta = 1;
			nb = (H - STREET_W) / blkLen;		// Number of blocks along y axis
			myV->theta += M_PI;					// 270 	deg
			myV->yr += nb * blkLen - STREET_W;
			/*					|________ where counting from bottom
			 */
		}
		nb = (W - STREET_W) / blkLen;			// Number of blocks along x axis
		do { myV->xr = STREET_W + rand()%(nb * blkLen);	/* Random position between first corner
														 * up to rightest corner of the rightest block
														 */
		} while(getpixel(screen, myV->xr, myV->yr) != STREET_COL);

	} else {
		/* Along Y axis and (by default) left screen 
		 */
		myV->xr = STREET_W;						// Initial position along x axis
		myV->theta = 0.0;						// 0 deg
		theta = 2;								// Flag representing 0 deg

		if (!ULorBR) {
			/* Right Side of Screen
			 */
			theta = 3;
			nb = (W - STREET_W) / blkLen;		// Number of blocks along x axis
			myV->theta += M_PI; 				// 180 deg
			myV->xr += nb * blkLen - STREET_W;	// Rightest corner of the rightest block
			/*					|________ where counting from bottom
			 */
		}

		nb = (H - STREET_W) / blkLen;			// Number of blocks along y axis

		do { myV->yr = STREET_W + rand()%(nb * blkLen); 	/* Random position between
															 * first corner til last corner
															 */
		} while(getpixel(screen, myV->xr, myV->yr) != STREET_COL);
	}

	myV->theta_old = myV->theta;							// this is steering purposes

	// Random color but choose an available one
	do {
		myV->color = rand() % MAX_KNOWN_ENCODED_COLORS;
	} while(!isAvailableThisColor(myV->color));

	// Rotate the points to plot
	rotatePoints(myV);

	placeCarInStreet(myV, theta);
	place3BeamsRangefinderOnVehicle(myV);
	initCam(&myV->cam, myV->xr, myV->yr);

	pthread_mutex_lock(&screen_lock);
	polygon(screen, 4, myV->point, myV->color);
	pthread_mutex_unlock(&screen_lock);
}
 
