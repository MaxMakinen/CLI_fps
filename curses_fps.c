#include <ncurses.h>

int	main(void)
{
	int		x;
	float	fPlayerX = 14.7f;			// Player Start Position
	float	fPlayerY = 5.09f;
	float	fPlayerA = 0.0f;			// Player Start Rotation
	float	fRayAngle;
	float	fStepSize = 0.1f;			// Increment size for ray casting, decrease to increase										
	float	fDistanceToWall = 0.0f; 	// resolution
	bool	bHitWall = false;			// Set when ray hits wall block
	bool	bBoundary = false;			// Set when ray hits boundary between two wall blocks
	float	fEyeX;						// Unit vector for ray in player space
	float	fEyeY;
	int		nTestX;
	int		nTestY;

	char	map[16][16] = {
		{"#########......."},
		{"#..............."},
		{"#.......########"},
		{"#..............#"},
		{"#......##......#"},
		{"#......##......#"},
		{"#..............#"},
		{"###............#"},
		{"##.............#"},
		{"#......####..###"},
		{"#......#.......#"},
		{"#......#.......#"},
		{"#..............#"},
		{"#......#########"},
		{"#..............#"},
		{"################"}
	}

	initscr();				/* Start curses mode 		*/
	start_color();			/* Start the color functionality */
	cbreak();				/* Line buffering disabled, Pass on every thing to me 		*/
	keypad(stdscr, TRUE);	/* I need that nifty F1 	*/
	noecho();

	while (1)
	{
		for (x = 0; x < SCREEN_WIDTH; x++)
		{
			// For each column, calculate the projected ray angle into world space
			fRayAngle = (fPlayerA - FOV2) + ((float)x / (float)SCREEN_WIDTH) * FOV;

			// Find distance to wall

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
				if (nTestX < 0 || nTestX >= MAPWIDTH || nTestY < 0 || nTestY >= MAPHEIGHT)
				{
					bHitWall = true;			// Just set distance to maximum depth
					fDistanceToWall = DEPTH;
				}
				else
				{
					// Ray is inbounds so test to see if the ray cell is a wall block
					if (map[nTestX * MAPWIDTH + nTestY] == '#')
					{
						// Ray has hit wall
						bHitWall = true;
/* ****************************************************************************** */

						// To highlight tile boundaries, cast a ray from each corner
						// of the tile, to the player. The more coincident this ray
						// is to the rendering ray, the closer we are to a tile 
						// boundary, which we'll shade to add detail to the walls
						vector<pair<float, float>> p;

						// Test each corner of hit tile, storing the distance from
						// the player, and the calculated dot product of the two rays
						for (int tx = 0; tx < 2; tx++)
							for (int ty = 0; ty < 2; ty++)
							{
								// Angle of corner to eye
								float vy = (float)nTestY + ty - fPlayerY;
								float vx = (float)nTestX + tx - fPlayerX;
								float d = sqrt(vx*vx + vy*vy); 
								float dot = (fEyeX * vx / d) + (fEyeY * vy / d);
								p.push_back(make_pair(d, dot));
							}

						// Sort Pairs from closest to farthest
						sort(p.begin(), p.end(), [](const pair<float, float> &left, const pair<float, float> &right) {return left.first < right.first; });
						
						// First two/three are closest (we will never see all four)
						float fBound = 0.01;
						if (acos(p.at(0).second) < fBound) bBoundary = true;
						if (acos(p.at(1).second) < fBound) bBoundary = true;
						if (acos(p.at(2).second) < fBound) bBoundary = true;
					}
				}
			}
		
			// Calculate distance to ceiling and floor
			int nCeiling = (float)(SCREEN_HEIGHT/2.0) - SCREEN_HEIGHT / ((float)fDistanceToWall);
			int nFloor = SCREEN_HEIGHT - nCeiling;

			// Shader walls based on distance
			short nShade = ' ';
			if (fDistanceToWall <= DEPTH / 4.0f)			nShade = 0x2588;	// Very close	
			else if (fDistanceToWall < DEPTH / 3.0f)		nShade = 0x2593;
			else if (fDistanceToWall < DEPTH / 2.0f)		nShade = 0x2592;
			else if (fDistanceToWall < DEPTH)				nShade = 0x2591;
			else											nShade = ' ';		// Too far away

			if (bBoundary)		nShade = ' '; // Black it out
			
			for (int y = 0; y < SCREEN_HEIGHT; y++)
			{
				// Each Row
				if(y <= nCeiling)
					screen[y*SCREEN_WIDTH + x] = ' ';
				else if(y > nCeiling && y <= nFloor)
					screen[y*SCREEN_WIDTH + x] = nShade;
				else // Floor
				{				
					// Shade floor based on distance
					float b = 1.0f - (((float)y -SCREEN_HEIGHT/2.0f) / ((float)SCREEN_HEIGHT / 2.0f));
					if (b < 0.25)		nShade = '#';
					else if (b < 0.5)	nShade = 'x';
					else if (b < 0.75)	nShade = '.';
					else if (b < 0.9)	nShade = '-';
					else				nShade = ' ';
					screen[y*SCREEN_WIDTH + x] = nShade;
				}
			}
		}

		// Display Stats
		swprintf_s(screen, 40, L"X=%3.2f, Y=%3.2f, A=%3.2f FPS=%3.2f ", fPlayerX, fPlayerY, fPlayerA, 1.0f/fElapsedTime);

		// Display Map
		for (int nx = 0; nx < MAPWIDTH; nx++)
			for (int ny = 0; ny < MAPWIDTH; ny++)
			{
				screen[(ny+1)*SCREEN_WIDTH + nx] = map[ny * MAPWIDTH + nx];
			}
		screen[((int)fPlayerX+1) * SCREEN_WIDTH + (int)fPlayerY] = 'P';

		// Display Frame
		screen[SCREEN_WIDTH * SCREEN_HEIGHT - 1] = '\0';
		WriteConsoleOutputCharacter(hConsole, screen, SCREEN_WIDTH * SCREEN_HEIGHT, { 0,0 }, &dwBytesWritten);

		refresh(); /* Refresh ncurses */

	}
	endwin();		/* End curses mode		  */
	return (0);
}
