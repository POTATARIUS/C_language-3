#include <curses.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

int py, px; // @ coords
int ty, tx; // t coord
bool t_placed = 0; // flag for goblin
bool p_placed = 0; // flag for player
int r_placed = 0; // flag for room
int p_gold = 0; // player gold

int game_loop(int c, int rows, int cols, char (* map)[cols]) // char (* map)[cols]) - two-dimensional array ...
{	
	srand(time(NULL));
	
	if (!r_placed) // r_placed = 0
	{
		int ry, rx; // room coords
		int r_size_y, r_size_x; // room size
		int r_center_y, r_center_x;
		int r_old_center_y, r_old_center_x;
		int room_num = rand() % 5 + 5;
		bool collision; // flag for collision of rooms
		
		
		// fill dungeon with walls and borders
		for (int y = 0; y <= rows; y++)
		{
			for (int x = 0; x <= cols; x++)
			{
				//borders
				if (y == 0 || y == rows - 1 || x == 0 || x == cols || y == rows)
					map[y][x] = '%';
				//walls
				else
					map[y][x] = '#';
			}
		}
		
		while(r_placed < room_num)
		{
			int try_counter = 0; // number of tries of prototyping
			
			// prototyping the room until no collision
			do
			{
				collision = 0;
				
				// room coords
				ry = rand() % (rows - 4) + 1;
				rx = rand() % (cols - 4) + 1;
				
				// room size
				r_size_y = rand() % 5 + 4;
				r_size_x = rand() % 10 + 8;
				
				try_counter++;
				if (try_counter > 1000)
				{
					ry = rx = 3;
					r_size_y = r_size_x = 3;
					break;
				}
				
				// fill DB map with rooms
				for (int y = ry; y <= ry + r_size_y; y++)
				{
					for (int x = rx; x <= rx + r_size_x; x++)
					{
						if (map[y][x] == '%' || map[y][x] == ' ' ||
							map[y + 2][x] == ' ' || map[y - 2][x] == ' ' ||
							map[y][x + 2] == ' ' || map[y][x - 2] == ' ')
						{
							collision = 1;
							y = ry + r_size_y; // exit upper loop...
							break; // ...exit from current loop...
						}
					}
				}	
			}
			while (collision == 1);
			
				// fill DB map with rooms
				for (int y = ry; y <= ry + r_size_y; y++)
				{
					for (int x = rx; x <= rx + r_size_x; x++)
					{
						if (map[y][x] == '%')
						{
							y = ry + r_size_y; // exit upper loop...
							break; // ...exit from current loop...
						}
						else
							map[y][x] = ' ';
					}
				}
				r_placed++;

				// corridors
				if (r_placed > 1)
				{
					r_old_center_y = r_center_y;
					r_old_center_x = r_center_x;
				}
				
				r_center_y = ry + (r_size_y / 2);
				r_center_x = rx + (r_size_x / 2);
				
				if (r_placed > 1)
				{
					int path_y;
					
					for (path_y = r_old_center_y; path_y != r_center_y; )
					{
						map[path_y][r_old_center_x] = ' ';
						
						if (r_old_center_y < r_center_y)
							path_y++;
						else if (r_old_center_y > r_center_y)
							path_y--;
					}
					
					for (int x = r_old_center_x; x != r_center_x; )
					{
						map[path_y][x] = ' ';
						
						if (r_old_center_x < r_center_x)
							x++;
						else if (r_old_center_x > r_center_x)
							x--;
					}
				}
		}
    }
	
	// draw the dungeon
	for (int y = 0; y <= rows; y++)
    {
        for (int x = 0; x <= cols; x++)
        {
			if (y == rows)
				mvaddch(y,x,' ');
			else if (map[y][x] == '%')
				mvaddch(y,x,'%');
			else if (map[y][x] == ' ')
				mvaddch(y,x,' ');
			else
				mvaddch(y,x,'#');
        }
    }

    if (c == KEY_UP && map[py-1][px] == ' ') py--;
    else if (c == KEY_DOWN && map[py+1][px] == ' ') py++;
    else if (c == KEY_LEFT && map[py][px-1] == ' ') px--;
    else if (c == KEY_RIGHT && map[py][px+1] == ' ') px++;
	
	if (!p_placed) // aka t_placed == 0
	{
		do
		{
			py = rand() % rows;
			px = rand() % cols;
		}
		while (map[py][px] == '#' || map [py][px] == '%') ;
		p_placed = 1;
	}
	
	if (!t_placed) // aka t_placed == 0
	{
		do
		{
			ty = rand() % rows;
			tx = rand() % cols;
		}
		while (map[ty][tx] == '#' || map [ty][tx] == '%') ;
		t_placed = 1;
	}
	
	if (py == ty && px == tx)
	{
		t_placed = 0;
		p_gold += rand() % 10 + 1;
	}
	
    mvaddch(py,px,'@'); // print gamer
	mvaddch(ty,tx,'t'); // print goblin
	mvprintw(rows, 0, "Gold: %d", p_gold);
	
	return 0;
}


int main(void)
{
    int c = 0; // input
    int cols, rows; 

    initscr();
    keypad(stdscr, 1); // allow arrows
    noecho(); // don't display input
    curs_set(0); // hide cursor
    getmaxyx(stdscr, rows, cols);

    char map[rows][cols]; // 0 -- 119  0 -- 23
	
    do
    {
        game_loop(c, rows - 1, cols - 1, map);

    }
    while ((c = getch()) != 27); // 27 = Esc
	
    getch();

    endwin();
	
    return 0;
}