#include <time.h>
#include <pthread.h>
#include <stdio.h>
#include <errno.h>

#include "task.h"
#include "timemanagement.h"

/*Checks if there's a deadline miss*/
int deadline_miss(rt_task_par_t * parameters){
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	if (time_cmp(now, parameters->dl) > 0) {
		parameters->dmiss++;
		return 1;
	}
	return 0;
}

/*Sets next activation time of a task*/
void set_activation(rt_task_par_t * parameters){
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	time_copy(&(parameters->at), t);
	time_copy(&(parameters->dl), t);
	time_add_ms(&(parameters->at), parameters->period);		//next activation time = now + period
	time_add_ms(&(parameters->dl), parameters->deadline);	//next deadline = now + relative deadline
}

/*Suspends the calling thread until next activation*/
void wait_for_activation(rt_task_par_t * parameters){
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME,
	 					&(parameters->at), NULL);		//sleeps until next activation/
	time_add_ms(&(parameters->at), parameters->period);	//next activation time = old act time + period
	time_add_ms(&(parameters->dl), parameters->period);	//next deadline = old deadline + period
}

/*Periodic task management*/
void periodic_task(void* arg) {
	rt_task_par_t * parameters = (rt_task_par_t *) arg;
	struct timespec t, now;
	set_activation(parameters);

	while (1) {
		// (*(parameters->function))();
		parameters->function(4);
		if(deadline_miss(parameters)){
			//do something
		}
		wait_for_activation(parameters);
	}
}

/*Creates a task executing the function fun*/
int task_create(void* fun, rt_task_par_t* par, int period, int deadline, int priority){

	pthread_attr_t myattr;
	struct sched_param mypar;
	int tret;

	if(priority>99){
		priority = 99;
		fprintf(stderr, "Cannot assign a priority higher than 99, assigning 99 to the task priority \n");
	}

	/*setting up the parameters of the task from input*/
	par->dmiss 		=	0;
	par->function 	=	fun;
	par->period 	=	period;
	par->deadline 	=	deadline;
	par->priority 	=	priority;

	pthread_attr_init(&myattr);

	/*notifying different scheduling from parent: */
	pthread_attr_setinheritsched(&myattr,PTHREAD_EXPLICIT_SCHED);

	/*setting the scheduler (for now fixed-pr. + RR): */
	pthread_attr_setschedpolicy(&myattr,SCHED_RR);

	mypar.sched_priority = par->priority;
	/*setting thread priority*/
	pthread_attr_setschedparam(&myattr,&mypar);

	tret = pthread_create(&(par->tid), &myattr, (void *) &periodic_task, par);
	if(tret)
		fprintf(stderr, "Error while creating a new task! %d\n", tret);

	return tret;
}
