#include <allegro.h>
#include <math.h>
#include <pthread.h>
 
#include "sensors.h"
#include "types.h"
#include "utils.h"
 
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
		pthread_mutex_lock(&screen_lock);
		putpixel(screen, x0 + i, y0 + j, *p);
		pthread_mutex_unlock(&screen_lock);

		j++;
		if (j == HRES) {
			j = 0;
			i++;
		}
	}
}
 
void checkStruct(int *in, int *out,
				const int x,
				const int y,
				const int i0,
				const int i1,
				const int j0,
				const int j1)
{
	int i, j;

	for (i = i0; i < i1; ++i) {
		for (j = j0; j < j1; ++j) {
			out[x * HRES + y] = (in[i * HRES + j] == WHITE) ? WHITE : BLACK;
		}
	}
}

void delation(int *in, int *out) {
	int i, j;
	int i0, i1, j0, j1;

	for (i = 0; i < VRES; ++i) {
		for (j = 0; j < HRES; ++j) {
			
			i0 = (i == 0) ? i : i - 1;
			j0 = (j == 0) ? j : j - 1;
			i1 = (i == (VRES - 1)) ? i1 : i + 1;                
			j1 = (j == (HRES - 1)) ? j1 : j + 1;                

			checkStruct(in, out, i, j, i0, i1, j0, j1);

		}
	}
}

void *getFrame(vehicle_t *c) {
/* provides periodically frames acquired by camera: 33 ms
 * code:
 */
	 
	// Camera res a local variable
	int hRes = c->cam.resH;     // Horizontal Resolution
	int vRes = c->cam.resV;     // Vertical Resolution
	int offset = 1;             // distance from the car 
	double cost = cos(c->theta);
	double sint = sin(c->theta);
	
	// Camera global position
	int x0 = (c->l / 2 + vRes / 2 + offset) * cost + c->xr;
	int y0 = (c->l / 2 + vRes / 2 + offset) * sint + c->yr;
 
	// Taking picture
	int i, j;
	int x, y;
 
	for (i = 0; i < vRes; i++) {
		for(j = 0; j < hRes; j++) {
			x = (-vRes/2 + i) * cost - (-hRes/2 + j) * sint + x0; 
			y = (-vRes/2 + i) * sint + (-hRes/2 + j) * cost + y0; 
			c->cam.image[i][j] = getpixel(screen, x, y);
			putpixel(screen, 400 + i, 620 + j, c->cam.image[i][j]);
		}
	}
}
 
void place4BeamsRangefinderOnVehicle(vehicle_t *c) {
	/* Place 4 rangefinders in each corner of the vehicle
	 * 45 degrees 
	 */
	double phiRel = M_PI / 4.0 + c->theta;
	int i;
 
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
	double alpha;
	 
	for (i = 0; i < beam; ++i) {
		x0 = dst->x[i];			// Position of the Sensor on x
		y0 = dst->y[i];			// Position of the sensor on y
		alpha = dst->phi[i];	// Angle of the current beam
		d = SMIN;				// Minimum measured distance 
		do {
			x = x0 + d * cos(alpha);
			y = y0 + d * sin(alpha);
			c = getpixel(screen, x, y);
			d = d + STEP;
		} while ((d <= SMAX) && (c == STREET_COL));
		dst->dsts[i] = d;
	}
}
 
void RobertsEdgeDetector(int *im) {
/* implementation of Roberts' Algorithm to detect edges
 */
	int i, j;
	int im2[VRES * HRES];
 
	for (i = 0; i < VRES; ++i) 
		for (j = 0; j < HRES - 1; ++j) 
			im2[i * HRES + j] = abs(im[i * HRES + j] - im[(i + 1) * HRES + j + 1]);
}

void RobertsEdgeDetectorH(int *im) {
/* implementation of Roberts' Algorithm to detect edges
 */
	int i, j;
	// int im2[VRES * HRES];
 
	for (i = 0; i < VRES; ++i) 
		for (j = 0; j < HRES - 1; ++j) 
			im[i * HRES + j] = abs(im[i * HRES + j] - im[i * HRES + j + 1]);
}
 
void SobelEdgeDetector(int *im) {
/* implementation of Sobel Algorithm to detect edges
 * No FULLY WORKING
 */	
	int i, j;
	int im2[VRES * HRES];
 
	for (i = 1; i < VRES; ++i) {
		for (j = 1; j < HRES - 1; ++j) {
			im2[i * HRES + j] = im[(i - 1) * HRES + (j - 1)] + 2 * im[(i - 1) * HRES + j] + im[(i - 1) * HRES + j + 1] - 
								im[(i + 1) * HRES + (j - 1)] - 2 * im[(i + 1) * HRES + j] - im[(i + 1) * HRES + j + 1];
			// im2[i * HRES + j] |= -im[(i + 1) * HRES + j] + im[i * HRES + j + 1],
		}
	}
}

void circleHoughT(int *im) {
	int im2[HRES * VRES];
	int xcast[VRES];
	int ycast[HRES];
	int maxx[4];	// 4 because maximun number of traffic lights in a corner
	int maxy[4];	// 4 because maximun number of traffic lights in a corner
	int i, j, top, x, y, max;
	double theta;

	for (i = 0; i < VRES; ++i) {
		xcast[i] = 0;
		ycast[i] = 0;
	}

	RobertsEdgeDetector(im);

	for (i = 0; i < VRES; ++i) {
		for (j = 0; j < HRES; ++j) {
			if (im[i * HRES + j] == WHITE) {
				for (theta = 0; theta < 360; ++theta) {
					x = i + LIGHTS_RAY * cos(theta * M_PI / 180.0);			
					y = j + LIGHTS_RAY * sin(theta * M_PI / 180.0);
					xcast[x] += 1; 			
					ycast[y] += 1; 	
				}
			}
		}
	}

	max = 0;
	top = -1;
	int c;

	for (i = 0; i < VRES; ++i) {
		for (j = 0; j < HRES; ++j) {

			c = xcast[i] + ycast[j];

			if (c > max) {
				top = (top + 1) % 4;
				maxx[top] = i;
				maxy[top] = j;
				max = c;
			}
		}
	}
}

void addLabel(disjoint_t *labels, int *label) {
	/* add a new label in the labels array 
	 */
	labels[*label].parent = *label;
	labels[*label].child = 0;
	*label = *label + 1;

	if(*label == WHITE) 
		*label = *label + 1;
}

void RosenfeldPfaltz(int *im, imxy_t *cxy) {
	/* Implementation of two passes algorithm for connected labeling
	 * Considerations:
	 * Background color -> BLACK
	 * Foreground color -> WHITE
	 */
	int i, j;									// for iteration
	int a[MAX_ELEMNTS];							// Store area of certain label
	int label = 1;								// init label at 1, cause 0 is the black color (background)
	disjoint_t labels[MAX_ELEMNTS];				// union-find data struct, to store the found labels

	if (*im == WHITE) {							// Check if the first element of the image is a foreground, if so:
		*im = label;							// assign the current label.
		a[label] = 0;							// Inits at zero the area of the current label.
		cxy->x[label] = 0;						// Inits position over x at 0.
		cxy->y[label] = 0;						// Inits position over y at 0.
		addLabel(labels, &label);				// Adds a new label to labels array.
	}

	/** FIRST PASS OF THE ALGORITHM **/
	for (i = 0; i < VRES; i++) {
		for (j = 0; j < HRES; j++) {

			if (im[i * HRES + j] == WHITE) {	// Check if the current pixel is a foreground

				if (i == 0) {					// If the first raw is being analyzed, the interesting 
					if (im[j - 1] == BLACK) {	// pixel is located in the previous column and if it's a background:
						im[j] = label;			// Assigns current label
						a[label] = 0;			
						cxy->x[label] = 0;		
						cxy->y[label] = 0;		
						addLabel(labels, &label);	// Adds a new label to labels array.
					} else 
						im[j] = im[j - 1]; 		// if the previous column has been labeled, use that label
				
				} else if (j == 0) {			// if the first column is being analyzed
					if (im[(i - 1) * HRES] == BLACK) {
						im[i * HRES] = label;	
						a[label] = 0;
						cxy->x[label] = 0;
						cxy->y[label] = 0;
						addLabel(labels, &label);

					} else
						im[i * HRES] = im[(i - 1) * HRES];

				} else {
					if ((im[(i - 1) * HRES + j] == BLACK) &&
						(im[i * HRES + j - 1] == BLACK)) {	// if the left and upper pixels are background
						im[i * HRES + j] = label;			// then create a new label
						a[label] = 0;
						cxy->x[label] = 0;
						cxy->y[label] = 0;
						addLabel(labels, &label);
					}
					
					else if (im[(i - 1) * HRES + j] == BLACK) 		// if only the upper pixel is background
						im[i * HRES + j] = im[i * HRES + j - 1];	// use the label of the left pixel
					
					else if (im[i * HRES + j - 1] == BLACK) 		// if only the left pixel is background
						im[i * HRES + j] = im[(i - 1) * HRES + j];	// use the label of the upper pixel

					else {											

						if((im[(i - 1) * HRES + j] == im[i * HRES + j - 1]))	// if the upper and left pixel is labeled with same label
							im[i * HRES + j] = im[(i - 1) * HRES + j]; 			// the current label shall be the same label.

						else {
							if (im[(i - 1) * HRES + j] < im[i * HRES + j - 1]) {	// if label of the upper pixel is less than label of the left 
								im[i * HRES + j] = im[(i - 1) * HRES + j];			// choose the lower one
								labels[im[i * HRES + j - 1]].parent = im[i * HRES + j];	// using union-find data structure, make the lower label a parent
								labels[im[i * HRES + j - 1]].child = 1;				// the higher label is a child

							} else {
								im[i * HRES + j] = im[i * HRES + j - 1];			// if labeld of the left pixel is less than the upper one
								labels[im[(i - 1) * HRES + j]].parent = im[i * HRES + j];
								labels[im[(i - 1) * HRES + j]].child = 1;
							}
						}
					}
				}
			}
		}
	}	

	/** END FIRST PASS OF THE ALGORITHM **/


	if (label == 1) { 		// If there is no label found, return
		cxy->N = 0;
		return;
	}
	/** SECOND PASS OF THE ALGORITHM **/
	for (i = 0; i < VRES; i++) {
		for (j = 0; j < HRES; j++) {
			if (im[i * HRES + j] != BLACK) {			// if the pixel is a background, thank u, next
				while (labels[im[i * HRES + j]].child) 	// if the current label is child
					im[i * HRES + j] = labels[im[i * HRES + j]].parent;	// keep digging in the sons until finding the parent
				a[im[i * HRES + j]]++;					// increase the area of the current label
				cxy->x[im[i * HRES + j]] += i;			// useful to compute de center of certain label
				cxy->y[im[i * HRES + j]] += j;			// according to: integral (X*da) / integral(da) 
			}
		}
	}
	/** END SECOND PASS OF THE ALGORITHM **/

	a[label + 1] = 0;		// A mark to stop the later while, this label doesn exist
	label = 1;										
	
	/** COMPUTING CENTERS OF THE CONNECTED LABELS **/
	while (a[label] != 0) {
		cxy->x[label] /= a[label]; 
		cxy->y[label] /= a[label];
		label++;
	}

	cxy->N = label - 1;	
}

void fastHarrisRobertCornerDetection(int *im, imxy_t *c) {
	/* Corner Detection of Binary images based on Harris Corner Detection 
	 * and Roberts Edge Detecion 
	 *	---------
	 *	| P | a |
	 *	---------
	 *	| b | c |
	 *  ---------
	 *  if P pixel at i,j is a corner then (P - c) has to be different than (a - b)
	 */
	int i, j;						// iterators 
	int dx, dy;						// derivatives
	int cornerCounter = 0;			// number of corners

	for (i = 1; i < VRES - 1; i++) {
		for (j = 1; j < HRES - 1; j++) {

			dx = abs(im[i * HRES + j] - im[(i + 1) * HRES + j + 1]); // P - c 
			dy = abs(im[i * HRES + j + 1] - im[(i + 1) * HRES + j]); // a - b

			if (dx != dy) {	// if they are different then pixel at i,j is a corner
				c->x[cornerCounter] = i; 
				c->y[cornerCounter] = j;
				cornerCounter++;
			}
		}
	}

	c->N = cornerCounter;
}

void  analyzeCameraFrame(vehicle_t *c, imfeatures_t *imft) {
/* Analyzes the camera frame looking for a traffic light
 * code:
 */
	int i, j;					// Iterators
	int minim = HRES * VRES;	// to get the closest traffic light
	int bwStreet[minim];		// bw image isolating streets
	int bwTL[minim];			// bw image isolating the traffic light cabine
	int tmp;					// temporal color of the image
 
	for (i = 0; i < VRES; ++i) {
		for (j = 0; j < HRES; ++j) {

			tmp = c->cam.image[i][j];
 
			// Isolate color of streets
			bwStreet[i * HRES + j] = (tmp != STREET_COL) ?
						BLACK : WHITE;

			// Isolate color of Traffic Lights
			bwTL[i * HRES + j] = (tmp == TL_COL) ? 
						WHITE : BLACK;

			// putpixel(screen, 400 + i, 620 + j, tmp);
		}
	}

	/* Morphology ops will go here */

	// RosenfeldPfaltz(bwTL, &imft->TLcenter);
	RobertsEdgeDetectorH(bwTL);
	fastHarrisRobertCornerDetection(bwStreet, &imft->streetCorner);
	// display(bwStreet, 400, 620);
	
	/** TO BE WRAPPED UP **/
	/** Read traffic light status **/ 
	for (i = 0; i < imft->TLcenter.N; i++) {
		if (imft->TLcenter.y[i + 1] > (HRES / 2)) {		// always look in the right
			if (imft->TLcenter.x[i + 1] < minim) {		// check for the closest traffic light
				minim = imft->TLcenter.x[i + 1];
				imft->TLminDistance = minim; 
				imft->TLstatus = c->cam.image[imft->TLcenter.x[i + 1]][imft->TLcenter.y[i + 1]];
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