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
 
void *vehicle(void *myVehi) {
	// vehicle_t v1;
	// initVehicle(&v1);
	// moveVehicle(&v1);
	// getFrame(&v1);
}


void rotatePoints(vehicle_t *myV) {

	/* 	Car - Reference Frame
	 *  
	 *	^ Y axis
	 * 	|
	 *	|	x3, y3-----------x0, y0
	 * 	|	|					|
	 * 	|	|		Cx, Cy	HEAD|
	 * 	|	|					|
	 *	|	x2, y2-----------x1, y1
	 *	|
	 *  --------------------------------> X axis
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

int steer(vehicle_t *c, steer_t *s) {

}
 
void pathPlanner(vehicle_t *c) {
	/* To plan the future path for the car
	 */

	imfeatures_t imf;
	steer_t turn;
	int isExecuted = 0;

	analyzeCameraFrame(c, &imf);

	if ((!imf.TLcenter.N)) {        // No traffic lights detected
		/*** brakes ***/
		if (c->ds.dsts[0] < 13)		// Minimun Distance before hit walls
			if (c->ds.dsts[3] < 13)   
				c->Vr = 0;
		/**************/
	} else {
		
		if ((imf.TLminDistance < 9) && (imf.TLstatus == TL_RED))
			c->Vr = 0.0;

		else if (imf.TLstatus == TL_GREEN)
			c->Vr = 10.0;
	}

	/* CORNERS **/

	// if (!isExecuted) {
	//  if (imf.streetCorner.N == 1) {  //  only one corner for the outter region of city
	//      if (imf.streetCorner.x[0] < 5) {
	//          turn = (imf.streetCorner.y[0] < (HRES / 2)) ? STEER_LEFT : STEER_RIGHT;
	//          isExecuted = 1;
	//          printf("%d \n", imf.streetCorner.x[0]);
	//      }
	//  }                               // if the corner is the left, turn left, otherwise turn right
	//  else if (imf.streetCorner.N == 2) {

	//      if (!(imf.streetCorner.x[0] - imf.streetCorner.x[1])) {     // if both corners are in the same x, then it can turn left or right 
	//          if (imf.streetCorner.x[0] < 5) {
	//              /* code */
	//          }
	//          turn = (rand() % 2) ? STEER_LEFT : STEER_RIGHT;
	//      }
	//      else 
	//          if (imf.streetCorner.y[0] < (HRES / 2))         // if both corners are in the same y, then it goes straight or left
	//              turn = (rand() % 2) ? STEER_LEFT : DONT_STEER;
	//          else
	//              turn = (rand() % 2) ? STEER_RIGHT : DONT_STEER;
			
	//  } else if (imf.streetCorner.N > 3) {
	//      printf("choose randomly steer left, right or dont %d\n", rand()%3);
	//  }
	
	// } else {
	//  isExecuted = steer(c, turn);    
	// }
}
 
void *moveVehicle(void *myV) {
	vehicle_t *c = (vehicle_t *) myV;           // casting vehicle type
	double xd = c->xr;                          // taking as double the current position
	double yd = c->yr;                          // this one too

	/** DELETING PREVIOUS POSITION **/
	pthread_mutex_lock(&screen_lock);           // locking shared resource screen
	polygon(screen, 4, c->point, STREET_COL);   // Delete previous position
	pthread_mutex_unlock(&screen_lock);         // unlocking

	/** Reading Sensors **/
	getRangefinder(&c->ds, RANGEFINDER_4_BEAMS);
	getFrame(c);
	pathPlanner(c);

	/** PI controller **/
	c->e = c->Vr - c->v_1;                      // Computes the error
	c->Ie += c->e;                              // Integrates the error
	c->u = c->K.Kp * c->e + c->K.Ki * c->Ie;    // computes the control output

	/* Dynamic model
	 * Vk = (T / (m + bT)) * F + (m / (m + bT))*Vk-1
	 * Xk = Xk-1 + Vk*T
	 */
	c->vel = (c->T / (c->m + c->b * c->T) ) * c->u + (c->m / (c->m + c->b * c->T)) * c->v_1;
	c->dx =  c->vel * c->T;     // distance covered by c->vel;

	xd += c->dx * cos(c->theta);    // New position over x
	yd += c->dx * sin(c->theta);    // New position over y

	c->xr = xd; // New position over x
	c->yr = yd;

	if (c->theta <= M_PI / 2) { //  
		c->xr = ceil(xd);       // New position over x
		c->yr = ceil(yd);   
	} 

	rotatePoints(c);                        // compute new points to plot
	place4BeamsRangefinderOnVehicle(c);     // 

	pthread_mutex_lock(&screen_lock);
	polygon(screen, 4, c->point, c->color); // Draw new position
	pthread_mutex_unlock(&screen_lock);

	// c->v_1 = sqrt((c->xr - x_old) * (c->xr - x_old) + (c->yr - y_old) * (c->yr - y_old)) / c->T;
	c->v_1 = c->vel;
}
 
char isAvailableThisColor(int color) {
	if (color == BLOCK_COL) return 0;
	if (color == TL_YELLOW) return 0;
	if (color == TL_RED) 	return 0;
	if (color == TL_GREEN)	return 0;
	if (color == STREET_COL) return 0;
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

	// Random color but choose an available one
	do {
		myV->color = rand()%MAX_KNOWN_ENCODED_COLORS;
	} while(!isAvailableThisColor(myV->color));

	// Rotate the points to plot
	rotatePoints(myV);

	placeCarInStreet(myV, theta);
	place4BeamsRangefinderOnVehicle(myV);
	initCam(&myV->cam, myV->xr, myV->yr);

	pthread_mutex_lock(&screen_lock);
	polygon(screen, 4, myV->point, myV->color);
	pthread_mutex_unlock(&screen_lock);
}
 
