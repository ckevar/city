#include <pthread.h>

#ifndef TASK_H
#define TASK_H


/*Structure that defines the parameters of a real time task*/
typedef struct {
	pthread_t tid;		/*task id*/
	int arg;			/* task argument */
	long wcet;			/* in microseconds */
	int period;			/* in milliseconds */
	int deadline;		/* relative (ms) */
	int priority;		/* in [0,99] */
	int dmiss;			/* no. of misses */
	struct timespec at;	/* next activ. time */
	struct timespec dl;	/* abs. deadline */
	void (*function)();	/*task cose to be executed*/
} rt_task_par_t;

/*	Creates a task executing the function fun
*	Returns 0 on success, error code otherwise
*/
int task_create(void* fun, rt_task_par_t* par, int period, int deadline, int priority);

#endif