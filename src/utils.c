#include <allegro.h>
#include <stdlib.h>
#include <stdio.h>

#include "utils.h"
#include "tlgraphics.h"
#include "types.h"

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
		if(p[i].period > 0) {

			if (c[i].isStopped) *r = i;

			if (*r == -1) *r = i;
		}
	}

}