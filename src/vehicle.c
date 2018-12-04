#include "vehicle.h"
#include "types.h"

void *drive(vehicle_t *myVehi) {

}

void initVehicle(vehicle_t *myV) {
	/* inits vehicle variables
	 * code:
	 */
	myV->xr = 15;
	myV->yr = 16;
	myV->camera.x = myV->xr + 5;
	myV->camera.y = myV->yr;
	myV->camera.resH = 16;
	myV->camera.resV = 9;
}