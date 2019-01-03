#include <allegro.h>
#include <stdlib.h>
#include <stdio.h>
#include "utils.h"
#include "tlgraphics.h"

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

