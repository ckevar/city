#include <math.h>
#include <stdio.h>

#include "pathplanner.h"
#include "utils.h"
#include "vehicle.h"

 
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
}

void steerLR(vehicle_t *c, imfeatures_t *ft) {

	double u, w;		// Vehicle frames
	// u = c->planner.e.a * cos(c->planner.alpha);	//
	int translation_inverse;
	c->planner.e.a += 1;
	u = c->planner.e.a;	//


	switch(c->turn) {
		case STEER_RIGHT:
			// w = c->planner.e.b * (sin(c->planner.alpha) + 1);
			// c->theta += c->planner.angleRes;			// Increases global angle of the vehicle
			// c->planner.alpha += c->planner.angleRes;	// Increases angle of the trajectory
			
			w = -50.0 / u;
			c->planner.alpha = atan((w - c->planner.w0) / u);
			c->theta = c->theta_old - (c->planner.alpha);	// Decreases global angle of the vehicle
			translation_inverse = TRANSLATION_INVX_RIGHT;
			break;

		case STEER_LEFT:
			// w = c->planner.e.b * (sin(c->planner.alpha) - 1);
			// c->theta -= c->planner.angleRes;			// Decreases global angle of the vehicle
			// c->planner.alpha -= c->planner.angleRes;	// Decreases angle of the trajectory
			
			w = 50.0 / u;
			c->planner.alpha = atan((w - c->planner.w0) / u);
			c->theta = c->theta_old - (c->planner.alpha);	// Decreases global angle of the vehicle
			translation_inverse = TRANSLATION_INVX_LEFT;
			break; 

		case TURN_180:
			// Working ... :'(
			break;

		default:		
			fprintf(stderr, "Something went wrong \n");
	}

	// if (fabs(c->theta - c->theta_old) > M_PI / 2.0) {
	if (fabs(c->planner.alpha) > (M_PI / 2 - M_PI / 1024)) {
		
		if (c->turn == STEER_RIGHT) 
			c->theta = c->theta_old + M_PI / 2.0;	
		
		else if (c->turn == STEER_LEFT) 
			c->theta = c->theta_old - M_PI / 2.0;	
		
		if (c->theta >= 2 * M_PI)	// bringing to the first quadrant  
			c->theta -= 2 * M_PI;

		else if (c->theta < 0) 		//	bringin to positive angle
			c->theta += 2 * M_PI;
		
		c->theta_old = c->theta;
		c->Vr = 0.0;
		c->isExecuted = 0;			// Crossroad state is released

	} else {
		c->xr = c->planner.tmpX + round((u + translation_inverse) * cos(c->theta_old) - w * sin(c->theta_old)); 
		c->yr = c->planner.tmpY + round((u + translation_inverse) * sin(c->theta_old) + w * cos(c->theta_old));
		// putpixel(screen, c->planner.tmpX + c->xr, c->planner.tmpY + c->yr, TL_GREEN);
		c->isExecuted = 1;			// still working on Crossroad state
		c->Vr = 0;					// Reference velocity 0
		// printf("%f %f %f %d %d\n", u, w, c->planner.alpha * 180 / M_PI, c->planner.tmpX + c->xr, c->planner.tmpY + c->yr);
	}
}

void chooseSteering(vehicle_t *c, imfeatures_t *ft) {
			
	c->isExecuted = 1;				// Crossroad state needs to be executed

	switch(ft->stCorner.N) {		// The steering will be chosen according to the number of corners detected
		case 1: 
			// In case of the corners of the city (boundary)
			c->turn = (ft->stCorner.y[0] < (HRES / 2)) ? STEER_LEFT : STEER_RIGHT;
			// c->turn = STEER_RIGHT;
			break;

		case 2:
			if (!(ft->stCorner.x[0] - ft->stCorner.x[1])) {      		// T shape street, car comming from center of T
				// c->turn = STEER_RIGHT;
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
					// c->turn = STEER_RIGHT;	
			} 

			break;

		case 4:		
				// c->turn = STEER_RIGHT;									// If it's a full crossrode, it can steering anyway
				c->turn = rand() % 3;									// If it's a full crossrode, it can steering anyway
				break;
	}

	c->planner.alpha = 0;												// Starts the angle for the elliptical shape steering
}

void genEllipticalTrajectory(vehicle_t *c, imfeatures_t *ft) {
/* generate basic parameters to steer in an elliptical shape
 */	
	int i;
	double HPDivisor = 1.0 / 10.0;	//
	double HPMultiplier = 2.2;	// Degree of freedom for turning right

	if (c->turn == STEER_LEFT) {
		c->planner.e.b = (c->ds.dsts[LEFT_DST] + c->w / 2) * HPDivisor;	// Axis along x
		c->planner.e.a = c->planner.e.b;							// By default if there is no corner bigger than this corner

		for (i = 1; i < ft->stCorner.N; ++i) {						// Iterates to choose the longer corner distance from vehicle
			if (ft->stCorner.x[i] > ft->stCorner.x[i - 1]) {		// as
				c->planner.e.a = (ft->stCorner.x[i] - c->w / 2 - 1) * HPDivisor;
			}
		}

	} else if (c->turn == STEER_RIGHT) {
		c->planner.e.b = (c->ds.dsts[RIGHT_DST]) * HPDivisor;
		c->planner.e.a = (c->ds.dsts[RIGHT_DST] + c->w * HPMultiplier) * HPDivisor;		
	}
}

void genKInvX(vehicle_t *c, imfeatures_t *ft) {
	int i;
	double HPDivisor = 1.0 / 10.0;	//
	double HPMultiplier = 2.2;	// Degree of freedom for turning right


	if (c->turn == STEER_LEFT) {
		c->planner.e.b = (c->ds.dsts[LEFT_DST] + c->w / 2) * HPDivisor;	// Axis along x
		c->planner.e.a = -TRANSLATION_INVX_LEFT;

		// for (i = 1; i < ft->stCorner.N; ++i) {						// Iterates to choose the longer corner distance from vehicle
		// 	if (ft->stCorner.x[i] > ft->stCorner.x[i - 1]) {		// as
		// 		c->planner.e.a = (ft->stCorner.x[i] - c->w / 2 - 1) * HPDivisor;
		// 	}
		// }

	} else if (c->turn == STEER_RIGHT) {
		c->planner.e.a = -TRANSLATION_INVX_RIGHT;
		c->planner.e.b = (c->ds.dsts[RIGHT_DST]) * HPDivisor;
		// c->planner.e.a = (c->ds.dsts[RIGHT_DST] + c->w * HPMultiplier) * HPDivisor;		
	}

	c->planner.w0 = c->planner.e.b;
	c->planner.tmpX = c->xr;
	c->planner.tmpY = c->yr;
}

void steer(vehicle_t *c, imfeatures_t *ft) {
	if (!c->turn) {	// If not steering
		if (!ft->stCorner.N)	// Keeps the crossrode state until there is no corners
			c->isExecuted = 0;	// End of crossroad state
	}
	else steerLR(c, ft); // If steering, left or right
}

char pathPlanner(vehicle_t *c) {
	/* To plan the future path for the car
	 */
	imfeatures_t imf;
	char stopppedByTL = 0;


	/***** FRONT DISTANCE RESPONSE *****/
	if (c->ds.dsts[MID_DST] < 10) {
		c->Vr = 0;	// Minimun Distance before hit wall from front
		/***********************************/
	}
	else { 
		c->Vr = V_REF;							// it needs to return going again
		/****** TRAFFIC LIGHT REPONSE ******/
		if (imf.TLcenter.N) {        		// No traffic lights detected
			// printf("before TLminDistance %f\n", c->Vr);
			if (imf.TLminDistance < 9) {
				if ((imf.TLstatus == TL_RED) || (imf.TLstatus == TL_YELLOW)) {
					c->Vr = 0.0;
					stopppedByTL = 1;
				}
				else
					c->Vr = V_REF;
			}
			// else if (imf.TLstatus == TL_GREEN)
			// 	c->Vr = V_REF;
		}
		/************************************/
		// printf("After TLminDistance %f\n", c->Vr);

		
		analyzeCameraFrame(c, &imf);	

		/******** CROSSROAD RESPONSE ********/
		if (!c->isExecuted) {

			keepCertainDistanceFromBlock(c, 10);	// keep safe distance from blocks

			if ((imf.stCorner.x[0] < 2) && (imf.stCorner.N)) {
				chooseSteering(c, &imf);	// the vehicle will go left, right or straigh
				genKInvX(c, &imf);		// generates basic parameters to compute the steering trajectory
				// genEllipticalTrajectory(c, &imf);		// generates basic parameters to compute the steering trajectory
			}
		}

		if (c->isExecuted) 
			steer(c, &imf);					// steers
		/***********************************/
	}
	return stopppedByTL;
}

