#include <allegro.h>
#include <math.h>
 
#include "sensors.h"
#include "types.h"
#include "utils.h"
 
#include <stdio.h>
 
void initCam(cam_t *myCam, const int x, const int y) {
    /* inits camera members
     */
    myCam->x = x + 5;
    myCam->y = y;
    myCam->resH = HRES;
    myCam->resV = VRES;
}
 
 
void *getFrame(vehicle_t *c) {
/* provides periodically frames acquired by camera: 33 ms
 * code:
 */
     
    // Camera res a local variable
    int hRes = c->cam.resH; 
    int vRes = c->cam.resV;
    int offset = 10;
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
        x0 = dst->x[i];
        y0 = dst->y[i];
        alpha = dst->phi[i];
        d = SMIN;
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
    int i, j;
    int im2[VRES * HRES];
 
    for (i = 0; i < VRES; ++i) {
        for (j = 0; j < HRES - 1; ++j) {
            im2[i * HRES + j] = im[i * HRES + j] - im[(i + 1) * HRES + j + 1],
            im2[i * HRES + j] |= -im[(i + 1) * HRES + j] + im[i * HRES + j + 1],
            putpixel(screen, 400 + i, 620 + j, im2[i * HRES + j]);
        }
    }
}
 
void SobelEdgeDetector(int *im) {
    int i, j;
    int im2[VRES * HRES];
 
    for (i = 1; i < VRES; ++i) {
        for (j = 1; j < HRES - 1; ++j) {
            im2[i * HRES + j] = im[(i - 1) * HRES + (j - 1)] + 2 * im[(i - 1) * HRES + j] + im[(i - 1) * HRES + j + 1] - 
                                im[(i + 1) * HRES + (j - 1)] - 2 * im[(i + 1) * HRES + j] - im[(i + 1) * HRES + j + 1];
            // im2[i * HRES + j] |= -im[(i + 1) * HRES + j] + im[i * HRES + j + 1],
            putpixel(screen, 400 + i, 620 + j, im2[i * HRES + j]);
        }
    }
}
 
#define BCKGND      0
#define FREGRD      15
#define MAX_LABELS  30
 
void oneComponentAtTime(int *im) {
    int i, j;
    int label = 0;
    int labels[MAX_LABELS];
    int labeled[VRES * HRES];
 
    if (im[0] == BCKGND) {
        label++;
        im[0] = label;
        labels[label] = label;
    }
 
    for (i = 0; i < VRES - 1; ++i) {
        for (j = 0; j < HRES - 1; ++j) {
 
            if (0 != (i * HRES + j)) {
 
                if (0 == i) {
                     
                    if (im[j] == FREGRD) {
 
                        if (im[j - 1] != BCKGND)
                            im[j] = im[j - 1];
 
                        else {
                            label++;
                            im[j] = label;
                            labels[label] = label;
                        }
                    }
 
                } else if (0 == j) {
 
                    if (im[i * HRES] == FREGRD) {
 
                        if (im[(i - 1) * HRES] != BCKGND) 
                            im[i * HRES] == im[(i - 1) * HRES];
 
                        else {
                            label++;
                            im[i * HRES] = label;
                            labels[label] = label;
                        }
                    }
 
                } else {
 
                    if (im[i * HRES + j] == FREGRD) { 
 
                        if ((im[(i - 1) * HRES + j] == im[i * HRES + j - 1]) && (im[(i - 1) * HRES + j] != BCKGND)) 
                            /*  |---|---|---|
                                | x | a | x |
                                -------------
                                | a |i,j| x |
                                -------------
                                | x | x | x |
                                ------------- 
                            */
                            im[i * HRES + j] = im[(i - 1) * HRES + j];
 
                        else if ((im[(i - 1) * HRES + j] != BCKGND) && (im[i * HRES + j - 1] != BCKGND)) {
                            /*  |---|---|---|
                                | B | a | x |
                                -------------
                                | c |i,j| x |
                                -------------
                                | x | x | x |
                                ------------- 
                                B is background
                                a is a A-label
                                c is a C-label
                            */
                            im[i * HRES + j] = im[(i - 1) * HRES + j];
                            labels[im[i * HRES + j - 1]] = label;
 
                        } else {
 
                            if (im[(i - 1) * HRES + j] != BCKGND) 
                                im[i * HRES + j] = im[(i - 1) * HRES + j];
                                /*  |---|---|---|
                                    | x | B | x |
                                    -------------
                                    | a |i,j| x |
                                    -------------
                                    | x | x | x |
                                    ------------- 
                                    B is background
                                    a is a A-label
                                */
 
                            else if (im[i * HRES + j - 1] != BCKGND) 
                                im[i * HRES + j] = im[i * HRES + j - 1];
                                /*  |---|---|---|
                                    | x | a | x |
                                    -------------
                                    | B |i,j| x |
                                    -------------
                                    | x | x | x |
                                    ------------- 
                                    B is background
                                    a is a A-label
                                */
 
                            else {
                                /*  |---|---|---|
                                    | x | B | x |
                                    -------------
                                    | B |i,j| x |
                                    -------------
                                    | x | x | x |
                                    ------------- 
                                    B is background
                                */
                                label++;
                                im[i * HRES + j] = label;
                                labels[label] = label;
                            }
 
                        }
 
                    }
                }
            } 
        }
    }
 
    // for (i = 0; i < label + 1; ++i) {
    //     printf("%d, ", labels[i]);
    // }
    // printf("\n");
 
    int k;
 
    // for (i = label + 1; i > 0; --i) {
         
    // }
 
    for (i = 0; i < VRES; ++i) {
        for (j = 0; j < HRES; ++j) {
            if (im[i * HRES + j] != BCKGND) {
                for (k = 0; k < label + 1; ++k) {
                    if (im[i * HRES + j] == label)
                        im[i * HRES + j] == labels[label];
                }
            }
            putpixel(screen, 600 + i, 620 + j, im[i * HRES + j]);
        }
    }
}
 
void  analyzeCameraFrame(vehicle_t *c) {
/* Analyzes the camera frame looking for a traffic light
 * code:
 */
    int i, j;
    int BW[VRES][HRES];
    int tl[VRES * HRES];
    int blockFrom = -1;
 
    for (i = 0; i < VRES; ++i) {
        for (j = 0; j < HRES; ++j) {
 
            BW[i][j] = (c->cam.image[i][j] != STREET_COL) ?
                        BCKGND : FREGRD;
 
 
            // if (blockFrom < 0) {
            //  if (c->cam.image[i][j] == BLOCK_COL)
            //      blockFrom = j;
            // } else {
            //  if (c->cam.image[i][j] == TL_RED) {
            //      /* compute centroid */
            //      c->Vr = 0;
            //  }
            // }
 
            tl[i * HRES + j] = (c->cam.image[i][j] == 14) ? 
                        BCKGND : FREGRD;
 
            // putpixel(screen, 400 + i, 620 + j, c->cam.image[i][j]);
            // putpixel(screen, 600 + i, 620 + j, tl[i][j]);
        }
    }
    // thickening(tl);
    oneComponentAtTime(tl);
    // SobelEdgeDetector(tl);
}
