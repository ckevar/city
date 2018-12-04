#ifndef TYPE_H
#define TYPE_H 

#define NUM_BEAMS 4
#define HRES 16
#define VRES 9


typedef enum tlState_ {RED_LIGHT, ORANGE_LIGHT, GREEN_LIGHT} tlState_t;

typedef struct  
{
	char	visible;
	int		x;		// x pixel position
	int 	y;		// y pixel position
	double 	distances[NUM_BEAMS];
} dstsens_t;

typedef struct 
{
	unsigned	resH;
	unsigned	resV;
	unsigned	x;		// x position
	unsigned	y;		// y position
	unsigned 	image[HRES][VRES];	// image
} cam_t;

typedef struct 
{
	
} gps_t;

typedef struct {
	int			color;
	double		mass;
	double		friction;
	double 		controlU;
	double		xr;	 // real (or granted) position
	double		yr; 	// real (or granted) position
	double		orientation;
	double		velocity;
	dstsens_t	dsensor;
	cam_t 		camera;	
	gps_t 		gps;
} vehicle_t;

#endif