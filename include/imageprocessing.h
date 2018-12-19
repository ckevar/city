#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H 

#include "types.h"

void delation(int *in, int *out);

/* implementation of Roberts' Algorithm to detect edges
 */
void RobertsEdgeDetector(int *im);

/* implementation of Roberts' Algorithm to detect edges
 */
void RobertsEdgeDetectorH(int *im);
 
/* implementation of Sobel Algorithm to detect edges
 * No FULLY WORKING
 */	
void SobelEdgeDetector(int *im);

void circleHoughT(int *im);


void addLabel(disjoint_t *labels, int *label);

/* Implementation of two passes algorithm for connected labeling
 * Considerations:
 */
void RosenfeldPfaltz(int *im, imxy_t *cxy, const int init);

void *fastHarrisRobertCornerDetection(void *void_im);

#endif