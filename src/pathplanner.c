#include <math.h>
#include <stdio.h>

#include "pathplanner.h"
#include "utils.h"
#include "vehicle.h"

 
extern double Vgr;

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

// int deltaY_invX();

void steerLR(vehicle_t *c, imfeatures_t *ft) {

	double u, v;		// Vehicle frames
	double w, z;
	double dy;
	double alpha_lim;
	int 	isDone = 0;

	int translation_inverse;
	dy = COEFF_INVX * c->planner.dx / (c->planner.e.a * c->planner.e.a + c->planner.e.a * c->planner.dx);
	if (dy > 1.0) {
		c->planner.dx = c->planner.e.a * c->planner.e.a / (COEFF_INVX - c->planner.e.a);
	} 
	// c->planner.e.a += (double)(c->xr - c->planner.tmpX);
	// printf("c->xr - c->planner.tmpX %d\n", c->xr - c->planner.tmpX);
	// u = c->planner.e.a + (double)c->planner.tmpX - (double)c->xr;

	switch(c->turn) {
		case STEER_RIGHT:
			c->planner.e.a += c->planner.dx;
			u = c->planner.e.a;	//
			// w = c->planner.e.b * (sin(c->planner.alpha) + 1);
			// c->theta += c->planner.angleRes;			// Increases global angle of the vehicle
			// c->planner.alpha += c->planner.angleRes;	// Increases angle of the trajectory
			
			v = -COEFF_INVX / u;
			c->planner.alpha = atan((v - c->planner.w0) / u);
			c->theta = c->theta_old - (c->planner.alpha);	// Decreases global angle of the vehicle
			alpha_lim = (M_PI / 2 - M_PI / 55);
			translation_inverse = TRANSLATION_INVX_RIGHT;

			if (fabs(c->planner.alpha) > alpha_lim) {
				isDone = 1;
			}

			break;

		case STEER_LEFT:
			c->planner.e.a += c->planner.dx;
			u = c->planner.e.a;	//
			// w = c->planner.e.b * (sin(c->planner.alpha) - 1);
			// c->theta -= c->planner.angleRes;			// Decreases global angle of the vehicle
			// c->planner.alpha -= c->planner.angleRes;	// Decreases angle of the trajectory
			
			v = COEFF_INVX / u;
			c->planner.alpha = atan((v - c->planner.w0) / u);
			c->theta = c->theta_old - (c->planner.alpha);	// Decreases global angle of the vehicle
			alpha_lim = (M_PI / 2 - M_PI / 55);
			translation_inverse = TRANSLATION_INVX_LEFT;
			if (fabs(c->planner.alpha) > alpha_lim) {
				isDone = 1;
			}
			break; 

		case TURN_180:
			u = c->planner.e.a * cos(c->planner.alpha);
			v = c->planner.e.a * sin(c->planner.alpha) - c->planner.w0;
			c->planner.alpha -= M_PI / 128;
			alpha_lim = -M_PI / 2;
			translation_inverse = 0;
			if (c->planner.alpha < -M_PI / 2.0)
				isDone = 1;
			c->theta = c->theta_old + (c->planner.alpha - M_PI / 2.0);
			// printf(" turning around goes here %f \n", c->planner.alpha * 180 / M_PI - 90);
			break;

		default:		
			fprintf(stderr, "Something went wrong \n");
	}

	if (isDone) {
		
		if (c->turn == STEER_RIGHT) 
			c->theta = c->theta_old + M_PI / 2.0;	
		else if (c->turn == STEER_LEFT) 
			c->theta = c->theta_old - M_PI / 2.0;	
		else if (c->turn == TURN_180)
			c->theta = c->theta_old + M_PI;
		
		if (c->theta >= 2 * M_PI)	// bringing to the first quadrant  
			c->theta -= 2 * M_PI;

		else if (c->theta < 0) 		//	bringin to positive angle
			c->theta += 2 * M_PI;
		
		c->theta_old = c->theta;
		c->Vr = 0.0;
		c->isTime2Steer = 0;			// Crossroad state is released

	} else {
		c->xr = c->planner.tmpX + round((u + translation_inverse) * cos(c->theta_old) - v * sin(c->theta_old)); 
		c->yr = c->planner.tmpY + round((u + translation_inverse) * sin(c->theta_old) + v * cos(c->theta_old));
		// c->planner.tmpX = round((u + translation_inverse) * cos(c->theta_old) - v * sin(c->theta_old)); 
		// c->planner.tmpY = round((u + translation_inverse) * sin(c->theta_old) + v * cos(c->theta_old));
		// putpixel(screen, c->planner.tmpX + c->xr, c->planner.tmpY + c->yr, TL_GREEN);
		c->Vr = 0;						// Reference velocity 0
		c->isTime2Steer = 1;			// still working on Crossroad state
		// w = ((u + translation_inverse) * cos(c->theta_old) - v * sin(c->theta_old)); 
		// z = ((u + translation_inverse) * sin(c->theta_old) + v * cos(c->theta_old));
		// c->Vr = sqrt((w - c->planner.tmpX)*(w - c->planner.tmpX) + (z - c->planner.tmpY)*(z - c->planner.tmpY)) / c->T;
		// printf("vel %f w %f, z %f, w_1 %f\n", c->Vr, w, z, c->planner.tmpX); 
		// c->planner.e.a += (w - c->planner.tmpX);
		// c->planner.tmpX = w; 
		// c->planner.tmpY = z; 

	}
}

void chooseSteering(vehicle_t *c, imfeatures_t *ft) {
			
	c->isTime2Steer = 1;				// Crossroad state needs to be executed

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
				if (c->ds.dsts[MID_DST] < DST_FROM_FRONTOBSTACLE) {
				}

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
				// c->turn = STEER_RIGHT;								// If it's a full crossrode, it can steering anyway
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
/* Generates parameters for K/x steering trajectory */
	double HPDivisor = 1.0 / 10.0;	// Scaling

	if (c->turn == STEER_LEFT) {
		c->planner.e.b = (c->ds.dsts[LEFT_DST] + c->w / 2) * HPDivisor;	// Axis along x
		c->planner.e.a = -TRANSLATION_INVX_LEFT;

	} else if (c->turn == STEER_RIGHT) {
		c->planner.e.b = (c->ds.dsts[RIGHT_DST]) * HPDivisor;
		c->planner.e.a = -TRANSLATION_INVX_RIGHT;				// parameter to generate the trajectory
	}

	c->planner.w0 = c->planner.e.b;
	c->planner.tmpX = c->xr;
	c->planner.tmpY = c->yr;
	// c->planner.tmpX = 0.0;
	// c->planner.tmpY = 0.0;
	c->planner.dx = 1.0;			// step size of x (or delta de x)
}

void steer(vehicle_t *c, imfeatures_t *ft) {
	if (!c->turn) {					// If not steering
		if (!ft->stCorner.N)		// Keeps the crossrode state until there is no corners
			c->isTime2Steer = 0;	// End of crossroad state
	}
	// else if (c->turn == )
	else steerLR(c, ft); // If steering, left or right
}

void check2turnAround(vehicle_t *c, imfeatures_t *imf){
	if (c->cam.image[(imf->stCorner.x[0] + 5) * HRES + 				// check if the color between the corners is the street color
		(imf->stCorner.y[0]/2 + imf->stCorner.y[1]/2)] == BLOCK_COL)
	{	
		c->planner.w0 = fabs(imf->stCorner.y[0] - imf->stCorner.y[1]);	// compute x0 to generate a half circle trajectory
		// printf("y[0] %d, y[1] %d\n", imf->stCorner.y[0], imf->stCorner.y[1]);
		if (c->planner.w0 > (STREET_W - 5.0)) {								// 5 is a tolerance
			c->planner.w0 /= 2.0;												// compute x0 to generate a half circle trajectory
			c->planner.w0 -= (imf->stCorner.y[1] - HRES / 2);					// compute x0 to generate a half circle trajectory
			c->planner.e.a = c->planner.w0; 								// radio
			c->planner.alpha = M_PI / 2;									// the circle starts at 90 degress
			c->planner.tmpX = c->xr;										// catching the current position
			c->planner.tmpY = c->yr;
			c->theta_old = c->theta;										// the current theta
			c->turn = TURN_180;												// flaggin the type of steering
			c->isTime2Steer = 1;											// enabling to execute the steering
		}
	}
}

char pathPlanner(vehicle_t *c) {
	/* To plan the future path for the car, it returns the reason why the car has been stopped
	 */
	imfeatures_t imf;				// filled with features extracted from the images 
	char stopppedByTL = 0;			// the car brakes do tu


	/***** FRONT DISTANCE RESPONSE *****/
	if (c->ds.dsts[MID_DST] < DST_FROM_FRONTOBSTACLE) {
		c->Vr = 0;	// Brakes before hit front wall or front block
		if ((c->yr < STREET_W / 2 + 5) || (c->xr < STREET_W / 2 + 5)) {
			stopppedByTL = 2;
		}
		/***********************************/
	} else { 
		c->Vr = Vgr;							// it needs to return going again
	
		analyzeCameraFrame(c, &imf);			// analyzes the current frame captured

		/****** TRAFFIC LIGHT REPONSE ******/
		if (imf.TLcenter.N) {        			// If traffic lights detected
			if (imf.TLminDistance < DST_FROM_TL2STOP) {
				/* the car should stop in red or yello light */
				if ((imf.TLstatus == TL_RED) || (imf.TLstatus == TL_YELLOW)) { 
					c->Vr = 0.0;
					stopppedByTL = 1;
				}

				else
					c->Vr = Vgr;
			}
		}
		/************************************/

		/******** CROSSROAD RESPONSE ********/
		if (!c->isTime2Steer) {
			keepCertainDistanceFromBlock(c, DST_FROM_LEFT_SIDEBLOCK);	// keep safe distance from blocks
			if (imf.stCorner.N){
				if(c->Vr > 20)	c->Vr = c->Vr-10;
				//printf("%f\n", c->vel);
				if(imf.stCorner.N== 2) {
					check2turnAround(c, &imf);
				}
			}
			if ((imf.stCorner.x[0] < DST_TO_STEER) && (imf.stCorner.N)) {
				chooseSteering(c, &imf);	// the vehicle will go left, right or straigh
				genKInvX(c, &imf);			// generates basic parameters to compute the steering trajectory
			}
		}

		if (c->isTime2Steer) 
			steer(c, &imf);					// steers
		/***********************************/
	}


	return stopppedByTL;
}

