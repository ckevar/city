#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "vehicle.h"

void allegro_init() {
}

int main(int argc, char const *argv[])
{
	vehicle_t *myV = (vehicle_t*)malloc(sizeof(vehicle_t));
	// vehicle_t *myV1;
	myV->color = 10;

	printf("Hello World\n");
	return 0;
}
