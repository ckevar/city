#include <allegro.h>
#include <stdlib.h>
#include <stdio.h>

#include "utils.h"
#include "tlgraphics.h"
#include "types.h"
#include "vehicle.h"

/* copy int elements from source to destination */
void cpyPnts(int *source, int *destination, const int N) {
	int *p;
	for (p = source; p < source + N; p++)
		destination[p - source] = *p;
}

/* compares int elements between A and B */
char cmpPnts(int *A, int *B, const int N) {
	int *p;
	for (p = A; p < A + N; p++)
		if (*p != B[p - A]) return 1;
	return 0;
}


void chooseCarDelete(int *r, vehicle_t *c, rt_task_par_t *p) {
	*r = -1;
	int i;
	for (i = 0; i < MAX_CARS; i++) {
		if(p[i].isActive) {

			if (c[i].isStopped == 1) *r = i;

			if (*r == -1) *r = i;
		}
	}

}

void checkWhosOut(int *r, vehicle_t *c, rt_task_par_t *p) {
	*r = -1;
	int i;
	for (i = 0; i < MAX_CARS; i++) {
		if(p[i].isActive && (c[i].isStopped == 2)) {
			*r = i;
		}
	}
}


void task_createTrafficLight(tl_manager_arg *man_arg, rt_task_par_t *tl_manager_par, int period, int deadline, int prioriti) {
	task_create(&tl_init, &tl_manager, NULL, man_arg, tl_manager_par, period, deadline, prioriti);
}

void task_createVehicle(vehicle_t *cars, rt_task_par_t *carsPrms, int period, int deadline, int prioriti){
	task_create(initVehicle, moveVehicle, termVehicle, cars, carsPrms, period, deadline, prioriti);
}
