#include <stdio.h>
#include "types.h"
#include "vehicle.h"

void allegro_init() {
}

int main(int argc, char const *argv[]) 
{
	vehicle_t myV;
	myV.color = 10;

	printf("Hello World\n");
	return 0;
}
