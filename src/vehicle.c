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
 
    double cost = cos(myV->theta);
    double sint = sin(myV->theta);
 
    myV->point[0] = (myV->l / 2) * cost - 
                    (myV->w / 2) * sint + myV->xr;
    myV->point[1] = (myV->l / 2) * sint + 
                    (myV->w / 2) * cost + myV->yr;
 
    myV->point[2] = (-myV->l / 2) * cost - 
                    (myV->w / 2) * sint + myV->xr;
    myV->point[3] = (-myV->l / 2) * sint + 
                    (myV->w / 2) * cost + myV->yr;
 
    myV->point[4] = (-myV->l / 2) * cost - 
                    (-myV->w / 2) * sint + myV->xr;
    myV->point[5] = (-myV->l / 2) * sint + 
                    (-myV->w / 2) * cost + myV->yr;
 
    myV->point[6] = (myV->l / 2) * cost - 
                    (-myV->w / 2) * sint + myV->xr;
    myV->point[7] = (myV->l / 2) * sint + 
                    (-myV->w / 2) * cost + myV->yr;
}
 
void translateVehiclePoints(vehicle_t *c) {
    /***************************/
    /**** WORKING ON THIS ******/
    int i;
    for (i = 0; i < 4; ++i) {
        c->point[2 * i] += c->xr;
        c->point[2 * i + 1] += c->yr;
    }
}
 
void pathPlanner(vehicle_t *c) {
    int i, beams;
     
    /*** brakes ***/
    if (c->ds.dsts[0] < 13) {
        if (c->ds.dsts[3] < 13) {
            c->Vr = 0;
        }
    }
    /**************/
 
    // printf("d0 %d d3 %d xr %d\n",c->ds.dsts[0], c->ds.dsts[3], c->xr);
 
    // if (c->ds.dsts[3] > 16) {
    //  if (c->ds.dsts[0] > c->ds.dsts[3]) {
    //      c->theta += M_PI / 4.0;
    //  }
    // }
    analyzeCameraFrame(c);
}
 
void *moveVehicle(void *myV) {
    vehicle_t *c = (vehicle_t *) myV;
    double xd = c->xr;
    double yd = c->yr;
 
    pthread_mutex_lock(&screen_lock);
    polygon(screen, 4, c->point, STREET_COL);    // Delete previous position
    pthread_mutex_unlock(&screen_lock);	
 
    /** Reading Sensors **/
    getRangefinder(&c->ds, RANGEFINDER_4_BEAMS);
    getFrame(c);
    pathPlanner(c);
 
    /** PI controller **/
    c->e = c->Vr - c->v_1;                     // Computes the error
    c->Ie += c->e;                                // Integrates the error
    c->u = c->K.Kp * c->e + c->K.Ki * c->Ie; // computes the control output
 
    /* Dynamic model
     * Vk = (T / (m + bT)) * F + (m / (m + bT))*Vk-1
     * Xk = Xk-1 + Vk*T
     */
    c->vel = (c->T / (c->m + c->b * c->T) ) * c->u + (c->m / (c->m + c->b * c->T)) * c->v_1;
    c->dx =  c->vel * c->T;        // distance covered by c->vel;
 
    xd += c->dx * cos(c->theta);  // New position over x
    yd += c->dx * sin(c->theta);  // New position over y
 
    c->xr = xd;  // New position over x
    c->yr = yd;
 
    if (c->theta < 1.58) {
        c->xr = ceil(xd);    // New position over x
        c->yr = ceil(yd);    
    } 
 
    rotatePoints(c);                        // compute new points to plot
    place4BeamsRangefinderOnVehicle(c);
 
    pthread_mutex_lock(&screen_lock);
    polygon(screen, 4, c->point, c->color);   // Draw new position
    pthread_mutex_unlock(&screen_lock);
 
    // c->v_1 = sqrt((c->xr - x_old) * (c->xr - x_old) + (c->yr - y_old) * (c->yr - y_old)) / c->T;
    c->v_1 = c->vel;
}
 
char isAvailableThisColor(int color) {
    if (color == BLOCK_COL) return 0;
    if (color == 14) return 0;  // YELLOW
    if (color == 12) return 0;  // RED
    if (color == 10) return 0;  // GREEN
    if (color == 1) return 0;
    return 1;
}
 
void auxMoveY(vehicle_t * myV, const int i, const int sgn) {
    // pthread_mutex_lock(&screen_lock);
    do {
        myV->yr = myV->yr + sgn;
        // translateVehiclePoints(myV);
        rotatePoints(myV);  // only translation is needed
    } while(getpixel(screen, myV->point[2 * i], myV->point[2 * i + 1]) != STREET_COL);
    // pthread_mutex_unlock(&screen_lock);
 
    myV->yr = myV->yr + sgn * 10;
}
 
void auxMoveX(vehicle_t * myV, const int i, const int sgn) {
    // pthread_mutex_lock(&screen_lock);
    do {
        myV->xr = myV->xr + sgn;
        // translateVehiclePoints(myV);
        rotatePoints(myV);  // only translation is needed
    } while(getpixel(screen, myV->point[2 * i], myV->point[2 * i + 1]) != STREET_COL);
 
    myV->xr = myV->xr + sgn * 10;
}
 
void placeCarInStreet(vehicle_t *myV, const int axis) {
    int i;
    for (i = 0; i < 4; ++i) {
        if (getpixel(screen, myV->point[2 * i], myV->point[2 * i + 1]) != STREET_COL) {
             
            if (i == 0) {
                if (axis == 0) auxMoveX(myV, i, 1);
                if (axis == 1) auxMoveX(myV, i, -1);
                if (axis == 2) auxMoveY(myV, i, -1);
                if (axis == 3) auxMoveY(myV, i, 1);
             
            } else if (i == 3) {
                if (axis == 0) auxMoveX(myV, i, -1);
                if (axis == 1) auxMoveX(myV, i, 1);
                if (axis == 2) auxMoveY(myV, i, 1);
                if (axis == 3) auxMoveY(myV, i, -1);
 
            }
        }
    }
}
 
void *initVehicle(void *c) {
    /* inits vehicle variables
     * code:
     */
    vehicle_t * myV = (vehicle_t *) c;
    int theta;                      /* Reference initial, orientation, 0: 90deg, 
                                       1: 270deg, 2: 0deg, 3: 180deg
                                     */
    int nb;                         // Number of blocks per axis
    int blkLen = BLOCK_W + STREET_W;// length of a block (+ street width)
    int XorY = rand() % 2;          // it shall generate the car over x or y axis
    int ULorBR = rand() % 2;        /* it shall generate the car Upper or Buttom or
                                     * Left or Right
                                     */
 
    /*** Car features ***/
    myV->l = VEHICLE_LENGTH; // Length of the vehicle
    myV->w = VEHICLE_WIDTH;      // Width of vehicle
    myV->m = MIN_MASS_CAR + rand() % (MAX_MASS_CAR - MIN_MASS_CAR);
    myV->b = MIN_FRICTION_CAR + rand() % (MAX_FRICTION_CAR - MIN_FRICTION_CAR);
    myV->K.Kp = PID_KP;          // Proportional Gain
    myV->K.Ki = PID_KI;          // Integral gain
    myV->dx = 0.0;               // covered distance 
    myV->Vr = 10;                // Velocity set Point (could be random per each car)
    myV->T = 0.033;              // Period
    myV->Ie = 0;             // Integrating the error
    myV->v_1 = 0;                // Initializing previous speed
    /********************/
 
 
    // GENERATE RANDOM POSITION
    if (XorY) {
        /* Along X axis and (by default) Top Screen
         */
        myV->yr = STREET_W;                      // Initial position along y axis
        myV->theta = M_PI / 2.0;                 // 90 deg
        theta = 0;                              // Flag representeng 90 deg
 
        if (!ULorBR) {
            /* Bottom Side of screen
             */
            theta = 1;
            nb = (H - STREET_W) / blkLen;       // Number of blocks along y axis
            myV->theta += M_PI;                  // 270  deg
            myV->yr += nb * blkLen - STREET_W;
            /*                  |________ where counting from bottom
             */
        }
        nb = (W - STREET_W) / blkLen;           // Number of blocks along y axis
        do { myV->xr = STREET_W + rand()%(nb * blkLen);  /* Random position between first corner
                                                         * up to rightest corner of the rightest block
                                                         */
        } while(getpixel(screen, myV->xr, myV->yr) != STREET_COL);
 
    } else {
        /* Along Y axis and (by default) left screen 
         */
        myV->xr = STREET_W;                      // Initial position along x axis
        myV->theta = 0.0;                        // 0 deg
        theta = 2;                              // Flag representing 0 deg
 
        if (!ULorBR) {
            /* Right Side of Screen
             */
            theta = 3;
            nb = (W - STREET_W) / blkLen;       // Number of blocks along x axis
            myV->theta += M_PI;              // 180 deg
            myV->xr += nb * blkLen - STREET_W;   // Rightest corner of the rightest block
            /*                  |________ where counting from bottom
             */
        }
 
        nb = (H - STREET_W) / blkLen;           // Number of blocks along y axis
 
        do { myV->yr = STREET_W + rand()%(nb * blkLen);  /* Random position between
                                                             * first corner til last corner
                                                             */
        } while(getpixel(screen, myV->xr, myV->yr) != STREET_COL);
    }
 
    // printf("x %d, y %d \n", myV->xr, myV->yr);
 
    // Random color but available color
    do {
        myV->color = rand()%MAX_KNOWN_ENCODED_COLORS;
    } while(!isAvailableThisColor(myV->color));
 
    // Rotate the points to plot
    rotatePoints(myV);
 
    placeCarInStreet(myV, theta);
    rotatePoints(myV); // Re-rotating the points according new position
    place4BeamsRangefinderOnVehicle(myV);
    initCam(&myV->cam, myV->xr, myV->yr);
 
    pthread_mutex_lock(&screen_lock);
    polygon(screen, 4, myV->point, myV->color);
    pthread_mutex_unlock(&screen_lock);
}
 
 
/*
|Estados    | forward   | left  | right | stop  | straight  |
|-----------|-----------------------------------------------
|green      |           |       |       |       |           |
|------------------------------------------------------------
|orange     |           |       |       |       |           |
|------------------------------------------------------------
|red        |           |       |       |       |           |
|------------------------------------------------------------
*/
