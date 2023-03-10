#include <ncurses.h>
#include "curses_fps.h"

int	main(void)
{
	int		x, y, tx, ty, index;
	float	fPlayerX = 2.0f;			// Player Start Position
	float	fPlayerY = 2.0f;
	float	fPlayerA = 0.0f;			// Player Start Rotation
	float	fRayAngle = 0.0f;
	float	fStepSize = 0.1f;			// Increment size for ray casting, decrease to increase										
	float	fDistanceToWall = 0.0f; 	// resolution
	bool	bHitWall = false;			// Set when ray hits wall block
	bool	bBoundary = false;			// Set when ray hits boundary between two wall blocks
	float	fEyeX;						// Unit vector for ray in player space
	float	fEyeY;
	int		nTestX;
	int		nTestY;
	int		nCeiling;
	int		nFloor;
	t_corner	corners[4];
	t_corner	temp_corner;

	char	map[16][16] = {
		{"################"},
		{"#..............#"},
		{"#......#....#..#"},
		{"#...#.......#..#"},
		{"#...........#..#"},
		{"#.########..#..#"},
		{"#..#........#..#"},
		{"##.#.#####..#..#"},
		{"#..#........#..#"},
		{"#.####.###..#..#"},
		{"#..#...#.#.....#"},
		{"##.#.###.#####.#"},
		{"#..#...#.......#"},
		{"#.####.#########"},
		{"#..............#"},
		{"################"}
	};

	int row, col;
	initscr();				/* Start curses mode */
	keypad(stdscr, TRUE);	
	cbreak();				/* Line buffering disabled */
	noecho();
	curs_set(0);			/* Set cursor invisible */

	start_color();			/* Start the color functionality */
	init_pair(1, COLOR_BLACK, COLOR_BLACK);
	init_pair(2, COLOR_BLACK, COLOR_BLUE);
	init_pair(3, COLOR_CYAN, COLOR_BLUE);
	init_pair(4, COLOR_BLACK, COLOR_CYAN);
	init_pair(5, COLOR_CYAN, COLOR_WHITE);
	init_pair(6, COLOR_RED, COLOR_BLACK);
	init_pair(7, COLOR_WHITE, COLOR_BLACK);
	init_pair(8, COLOR_BLACK, COLOR_RED);

	init_pair(9, COLOR_BLACK, COLOR_WHITE);

	clear();

	getmaxyx(stdscr,row,col);		/* get the number of rows and columns */
	if (row < SCREEN_HEIGHT || col < SCREEN_WIDTH)
	{
		mvprintw(row / 2 - 1, col / 3,"This screen has %d rows and %d columns\n",row,col);
		mvprintw(row / 2, col / 3,"Requires screen has %d rows and %d columns\n",SCREEN_HEIGHT,SCREEN_WIDTH);
 		refresh();
 		getch();
 		endwin();

 		return 0;
	}


	int	ch;

	while (1)
	{
		for (x = 0; x < SCREEN_WIDTH; x++)
		{
			// For each column, calculate the projected ray angle into world space
			fRayAngle = (fPlayerA - FOV2) + ((float)x / (float)SCREEN_WIDTH) * FOV;

			// Find distance to wall
			fDistanceToWall = 0.0f; 	// resolution

			bHitWall = false;			// Set when ray hits wall block
			bBoundary = false;			// Set when ray hits boundary between two wall blocks

			fEyeX = sinf(fRayAngle); // Unit vector for ray in player space
			fEyeY = cosf(fRayAngle);

			// Incrementally cast ray from player, along ray angle, testing for 
			// intersection with a block
			while (!bHitWall && fDistanceToWall < DEPTH)
			{
				fDistanceToWall += fStepSize;

				nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
				nTestY = (int)(fPlayerY + fEyeY * fDistanceToWall);
				

				// Test if ray is out of bounds
				if (nTestX < 0 || nTestX >= MAP_WIDTH || nTestY < 0 || nTestY >= MAP_HEIGHT)
				{
					bHitWall = true;			// Just set distance to maximum depth
					fDistanceToWall = DEPTH;
				}
				else
				{
					// Ray is inbounds so test to see if the ray cell is a wall block
					if (map[nTestX][nTestY] == '#')
					{
						// Ray has hit wall
						bHitWall = true;
						index = 0;
						for (tx = 0; tx < 2; tx++)
						{
							for (ty = 0; ty < 2; ty++)
							{
								// Angle of corner to eye
								float vy = (float)nTestY + ty - fPlayerY;
								float vx = (float)nTestX + tx - fPlayerX;
								float d = sqrt(vx*vx + vy*vy); 
								float dot = (fEyeX * vx / d) + (fEyeY * vy / d);
								corners[index].magnitude = d;
								corners[index].dot_product = dot;
								index++;
							}
						}
						for (tx = 0; tx < 4; tx++)
						{
							for (ty = tx; ty < 4; ty++)
							{
								if (corners[tx].magnitude > corners[ty].magnitude)
								{
									temp_corner = corners[tx];
									corners[tx] = corners[ty];
									corners[ty] = temp_corner;
								}
							}
						}
						if ((acos(corners[0].dot_product) < BOUNDS) || \
						(acos(corners[1].dot_product) < BOUNDS) || (acos(corners[2].dot_product) < BOUNDS))
							bBoundary = TRUE;
					}
				}
			}
			// Calculate distance to ceiling and floor
			nCeiling = (int)((float)((float)SCREEN_HEIGHT/2.0f) - (float)((float)SCREEN_HEIGHT / fDistanceToWall));
			nFloor = (int)(SCREEN_HEIGHT - nCeiling);
		
			unsigned char nShade = ' ';
			unsigned char nfShade = ' ';
			int		col = 7;
			attron(COLOR_PAIR(1));
			if (fDistanceToWall <= DEPTH / 4.0f)	// Very close
			{
				col = 5;
				if (fDistanceToWall <= DEPTH / 7.0f)
					nShade = ' ';
				else
					nShade = 'a';
			}
			else if (fDistanceToWall < DEPTH / 3.0f)
			{
				col = 9;
				nShade = 'a';
			}
			else if (fDistanceToWall < DEPTH / 2.0f)
			{
				col = 4;
				nShade = 'a';
			}
			else if (fDistanceToWall < DEPTH)
			{
				col = 2;
				nShade = ' ';
			}
			else
			{
				col = 1;
				nShade = ' ';						// Too far away
			}

			//nShade = ' ';
			if (bBoundary)
			{
				nShade = 'x';
			}
			for (y = 0; y < SCREEN_HEIGHT; y++)
			{
				if (y < nCeiling)
				{
					attron(COLOR_PAIR(7));
					mvaddch(y, x, ' ');	// y * SCREEN_WIDTH + x = ' ';
				}
				else if (y > nCeiling && y <= nFloor)
				{
					if (bBoundary)
						attron(A_REVERSE);
					attron(COLOR_PAIR(col));
					mvaddch(y, x, NCURSES_ACS(nShade));	// y * SCREEN_WIDTH + x = '#';
					attroff(A_REVERSE);
				}
				else
				{
					
					float b = 1.0f - (((float)y - (float)SCREEN_HEIGHT / 2.0f) / ((float)SCREEN_HEIGHT / 2.0f));
					if (b < 0.25)	// Very close
					{
						attron(COLOR_PAIR(8));
						nfShade = ' ';
					}
					else if (b < 0.5)
					{
						attron(COLOR_PAIR(8));
						nfShade = 'a';
					}
					else if (b < 0.75)
					{
						attron(COLOR_PAIR(6));
						nfShade = 'a';
					}
					else if (b < 0.9)
					{
						attron(COLOR_PAIR(6));
						nfShade = 'a';
					}
					else
						nfShade = ' ';						// Too far away
					mvaddch(y, x, NCURSES_ACS(nfShade));	// y * SCREEN_WIDTH + x = ' ';
				}
			}
			attroff(A_REVERSE);
			attron(COLOR_PAIR(7));
		}
		for (tx = 0; tx < MAP_WIDTH; tx++)
		{
			for (ty = 0; ty < MAP_HEIGHT; ty++)
			{
				mvaddch(ty, tx, map[ty][tx]);
			}
		}
		mvaddch((int)fPlayerX, (int)fPlayerY, 'P');
		move((int)fPlayerX, (int)fPlayerY);
		refresh(); /* Refresh ncurses */
		usleep(50000);
		ch = getch();
		if (ch == KEY_END)
			break ;
		else if (ch == KEY_LEFT || ch == 'a')
			fPlayerA -= 0.1f;
		else if (ch == KEY_RIGHT || ch == 'd')
			fPlayerA += 0.1f;
		else if (ch == KEY_UP || ch == 'w')
		{
			fPlayerX += sinf(fPlayerA) * 0.5f;
			fPlayerY += cosf(fPlayerA) * 0.5f;
			if (map[(int)fPlayerX][(int)fPlayerY] == '#')
			{				
				fPlayerX -= sinf(fPlayerA) * 0.5f;
				fPlayerY -= cosf(fPlayerA) * 0.5f;
			}
		}
		else if (ch == KEY_DOWN || ch == 's')
		{
			fPlayerX -= sinf(fPlayerA) * 0.5f;
			fPlayerY -= cosf(fPlayerA) * 0.5f;
			if (map[(int)fPlayerX][(int)fPlayerY] == '#')
			{				
				fPlayerX += sinf(fPlayerA) * 0.5f;
				fPlayerY += cosf(fPlayerA) * 0.5f;
			}
		}
		else if (ch == KEY_UP || ch == 'e')
		{
			fPlayerX += cosf(fPlayerA) * 0.5f;
			fPlayerY += sinf(fPlayerA) * 0.5f;
			if (map[(int)fPlayerX][(int)fPlayerY] == '#')
			{				
				fPlayerX -= cosf(fPlayerA) * 0.5f;
				fPlayerY -= sinf(fPlayerA) * 0.5f;
			}
		}
		else if (ch == KEY_DOWN || ch == 'q')
		{
			fPlayerX -= cosf(fPlayerA) * 0.5f;
			fPlayerY -= sinf(fPlayerA) * 0.5f;
			if (map[(int)fPlayerX][(int)fPlayerY] == '#')
			{				
				fPlayerX += cosf(fPlayerA) * 0.5f;
				fPlayerY += sinf(fPlayerA) * 0.5f;
			}
		}
	}
	endwin();		/* End curses mode		  */
	return (0);
}
