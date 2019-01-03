#ifndef PATH_PLANNER_H
#define PATH_PLANNER_H 

#include "types.h"

#define TRANSLATION_INVX_LEFT 40
#define TRANSLATION_INVX_RIGHT 25

void steerLR(vehicle_t *c, imfeatures_t *ft);
void chooseSteering(vehicle_t *c, imfeatures_t *ft);
void genEllipticalTrajectory(vehicle_t *c, imfeatures_t *ft);
void steer(vehicle_t *c, imfeatures_t *ft);
void pathPlanner(vehicle_t *c);

#endif