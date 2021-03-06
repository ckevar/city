#include <stdio.h>
#include <allegro.h>
#include <pthread.h>

#include "graphics.h"
#include "types.h"
#include "vehicle.h"
#include "tlmanager.h"
#include "utils.h"
#include "task.h"

extern pthread_mutex_t screen_lock;
extern pthread_mutex_t dmiss_lock;
extern int deadline_misses;

double Vgr;

int main(int argc, char const *argv[])
{
	vehicle_t 		cars[MAX_CARS];
	rt_task_par_t 	carsPrms[MAX_CARS];

	Vgr = V_REF;
	
	char	scan;
	char 	carCounter = 0;
	char 	isPressed = 1;

	int		readyCar = 0;
	int 	i;
	int 	tl_matrix[N_BLOCKS_X*2 * N_BLOCKS_Y*2];

	srand (time(NULL));
	allegro_init();

	initialize_graphics(tl_matrix);

	/**** Undet Test Switch Light task *****/	
	tl_manager_arg man_arg;
	man_arg.tl_matrix = tl_matrix;
	rt_task_par_t tl_manager_par;
	// task_create(&tl_init, &tl_manager, NULL, &man_arg, &tl_manager_par, 4000, 1000, 3);
	task_createTrafficLight(&man_arg, &tl_manager_par, 4000, 1000, 3);
	/***************************************/

	/******* Under Test Vehicle-Task *******/
	do {

		if(keypressed()) {				// if there's is a pressed key,  
			scan = readkey() >> 8;		// it catches the pressed key,
			isPressed++;				// How many times the key is pressed
		} else isPressed = 0;			// otherwise no key pressed

		if (isPressed == 1) {			// only the first catch will be taken in count

			/** If N key is pressed a new car will be created only if MAX is not reached **/
			if (scan == KEY_N && carCounter < MAX_CARS) {
				readyCar = 0;
				while(carsPrms[readyCar].isActive) readyCar++;
				task_createVehicle(&cars[readyCar], &carsPrms[readyCar], 40, 40, 2);
				// task_create(initVehicle, moveVehicle, termVehicle, &cars[readyCar], &carsPrms[readyCar], 40, 40, 2);
				carCounter++;
			}

			/** if D key is pressed a still car or the last added will be removed off simulation **/
			if (scan == KEY_D && carCounter > 0) {
				chooseCarDelete(&readyCar, cars, carsPrms);
				task_terminate(&carsPrms[readyCar]);
				carCounter--;
			}

			/** if UP key is pressed all the vehicle will increase its velocity **/
			if(scan == KEY_UP) {
				Vgr = (Vgr >= MAX_VEL_CAR) ? MAX_VEL_CAR : Vgr + 1;
				drawGVel(Vgr);
			}

			/** if DOWN key is pressed all the vehicle will decrease its velocity **/
			if (scan == KEY_DOWN) {
				Vgr = (Vgr <= MIN_VEL_CAR) ? MIN_VEL_CAR : Vgr - 1;
				drawGVel(Vgr);
			}

			drawNCars(carCounter);	
		}

		checkWhosOut(&readyCar, cars, carsPrms);
		if (readyCar > -1) {
			task_terminate(&carsPrms[readyCar]);
			carCounter--;
			drawNCars(carCounter);	
		}

	} while(scan != KEY_ESC);
	/***************************************/

	/* Terminates all threads */
	for(i = 0; i < MAX_CARS; i++) {
		if (carsPrms[i].isActive)
			task_terminate(&carsPrms[i]);
	}

	allegro_exit();

	return 0;
}
