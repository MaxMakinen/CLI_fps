# CLI_fps
Command line fps style maze game using ncurses.

Implementing a first-person 3D rendering of a maze game in a terminal window using the ncurses library. 
The game takes in a 2D character array called 'map' that represents the layout of the maze, 
and the player's position and orientation are represented by the variables 'fPlayerX', 'fPlayerY', and 'fPlayerA'. 
The game continuously renders the maze to the terminal by casting rays from the player's position and 
orientation and determining the distance to the nearest wall. The game also takes input from the user to move the player's position and orientation in the maze.

The code begins by including the ncursed and cursed_fps.h header files and declaring several variables. 
It then initializes the ncursed library and sets up the terminal window for rendering. 
The 'map' array is defined and initialized with the layout of the maze. 
The code then enters a loop that continuously renders the maze to the terminal by casting rays from the player's position and 
orientation and determining the distance to the nearest wall. The player's position and orientation can be changed by the user using keyboard input. 
The loop ends when the user closes the terminal window.

compile with:
gcc curses_fps.c curses_fps.h -lncurses -lm

Move the player with the arrow or WASD keys.

End the game with pressing the end key.
