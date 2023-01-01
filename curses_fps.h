#ifndef CURSES_FPS_H
# define CURSES_FPS_H

# define SCREEN_WIDTH 120
# define SCREEN_HEIGHT 30
# define MAPWIDTH = 16				// World Dimensions
# define MAPHEIGHT = 16
# define DEPTH 16				// Maximum rendering distance
# define FOV 3.14159f / 4.0f	// Field of view
# define FOV2 FOV / 2.0f		// Half of field of view
# define SPEED 5.09f			// walking speed

# ifndef TRUE
# 	define TRUE 1
# endif
# ifndef FALSE
#  define FALSE 0
# endif

#endif
