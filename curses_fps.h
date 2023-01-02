#ifndef CURSES_FPS_H
# define CURSES_FPS_H

# include <math.h>
# include <unistd.h>

# define SCREEN_WIDTH 120
# define SCREEN_HEIGHT 30
# define MAP_WIDTH 16				// World Dimensions
# define MAP_HEIGHT 16
# define DEPTH 16				// Maximum rendering distance
# define FOV 3.14159f / 4.0f	// Field of view
# define FOV2 FOV / 2.0f		// Half of field of view
# define SPEED 5.09f			// walking speed
# define BOUNDS 0.008f			// For determining if we see a corner

# ifndef TRUE
# 	define TRUE 1
# endif
# ifndef FALSE
#  define FALSE 0
# endif

typedef struct s_corner
{
	float	magnitude;
	float	dot_product;
}	t_corner;


#endif
