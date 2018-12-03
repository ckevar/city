#ifndef TYPE_H
#define TYPE_H 

typedef enum tlState_ {RED_LIGHT, ORANGE_LIGHT, GREEN_LIGHT} tlState_t;

typedef struct  
{
	char	visible;
	int		posX;
	int 	posY;
	double 	*distances;
} dstsens_t;

typedef struct 
{
	int		resX;
	int		ResolutionY;
	double	posX;
	double	posY;
} cam_t;

typedef struct 
{
	
} gps_t;

typedef struct {
	int			color;
	double		mass;
	double		friction;
	double 		controlU;
	double		posXr;	 // real (or granted) position
	double		posYr; 	// real (or granted) position
	double		orientation;
	double		velocity;
	dstsens_t	dsensor;
	cam_t 		camera;	
	gps_t 		gps;
} vehicle_t;

#endif