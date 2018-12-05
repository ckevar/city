
#ifndef TIME_MANAGEMENT_H
#define TIME_MANAGEMENT_H 
#endif

/*Copies time struct ts into td*/
void time_copy(struct timespec *td, struct timespec ts);

/*Adds ms milliseconds to the time variable pointed by t*/
void time_add_ms(struct timespec *t, int ms);

/*	Returns 0 if t1 == t2
*	Returns 1 if t1 > t2
*	Returns -1 if t1 < 2
*/
int time_cmp(struct timespec t1,struct timespec t2);