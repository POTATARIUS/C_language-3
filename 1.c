#include <curses.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

int py, px; // @ coords
bool t_placed = 0; // flag for goblin
bool p_placed = 0; // flag for player
int r_placed = 0; // flag for room
int p_gold = 0; // player gold
int dlvl = 1;

struct monsters
{
	int y;
	int x;
	int hp;
};

struct monsters monster[10]; // 0 ... 9


int battle(int cols, char (* map)[cols], int dir_y, int dir_x)
{
	for (int m = 0; m < 10; m++)
	{
			if (dir_y == monster[m] .y && dir_x == monster[m] .x)
			{
				if (monster[m] .hp <= 0)
				{
					map[dir_y][dir_x] = ' ';
					p_gold += rand() % 10 + 1;
				}
				else
					monster[m] .hp -= 1;
				break;
			}
	}
	return 0;
}

int p_action(int c, int cols, char (* map)[cols])
{
	int dir_y = py, dir_x = px;
		
	if (c == KEY_UP)
		dir_y--;
	else if (c == KEY_DOWN)
		dir_y++;
	else if (c == KEY_LEFT)
		dir_x--;
	else if (c == KEY_RIGHT)
		dir_x++;
	else if (c == '>' && map[py][px] == '>')
	{
		t_placed = 0;
		p_placed = 0;
		r_placed = 0;
		return 1;
	}
	
	if (map[dir_y][dir_x] == ' ' || map[dir_y][dir_x] == '>')
	{
		py = dir_y;
		px = dir_x;
	}
	else if (map[dir_y][dir_x] == 't')
		battle(cols, map, dir_y, dir_x);
	return 0;
}

int dungeon_draw (int rows, int cols, char (* map)[cols])
{
	// draw the dungeon
	for (int y = 0; y <= rows; y++)
    {
        for (int x = 0; x <= cols; x++)
        {
			if (y == 0 || y == rows)
				mvaddch(y,x,' ');
			else if (map[y][x] == 't')
				mvaddch(y,x,'t');
			else if (map[y][x] == '>')
				mvaddch(y,x,'>');
			else if (map[y][x] == '%')
				mvaddch(y,x,'%');
			else if (map[y][x] == ' ')
				mvaddch(y,x,' ');
			else
				mvaddch(y,x,'#');
        }
    }
	
	mvprintw(rows, 0, "Gold: %d \t Dlvl: %d", p_gold, dlvl);
	
	return 0;
}

int respawn_creatures(int rows, int cols, char (* map)[cols])
{
	if (!t_placed) // aka t_placed == 0
	{
		int my, mx;
		
		for (int m = 0; m < 10; m++)
		{
			do
			{
				my = rand() % rows;
				mx = rand() % cols;
			}
			while (map[my][mx] == '#' || map [my][mx] == '%') ;
			
			monster[m] .y = my;
			monster[m] .x = mx;
			monster[m] .hp = 2;
			map[monster[m] .y][monster[m] .x] = 't';
		}	
		
		t_placed = 1;
		
	}
	
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
	return 0;
}

int dungeon_gen (int rows, int cols, char (* map)[cols])
{
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
				if (y == 0 || y == 1|| y == rows - 1 || x == 0 ||
					x == cols || y == rows)
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
			{
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
		
		// staircase gen
		int sy, sx;
		do
		{
			sy = rand() % rows;
			sx = rand() % cols;
		}
		while (map[sy][sx] != ' ');
		map[sy][sx] = '>';
		
    }
	
	return 0;
}

int game_loop(int rows, int cols, char (* map)[cols]) // char (* map)[cols]) - two-dimensional array ...
{	
	int c;
	int new_lvl;
	srand(time(NULL));
	
	dungeon_gen(rows, cols, map);

	respawn_creatures (rows, cols, map);	
	
	if (c != 0)
	{	
		new_lvl = p_action (c, cols, map); // battle ()
	}
	
	dungeon_draw (rows, cols, map);
	
    mvaddch(py,px,'@'); // draw p
	
	if (new_lvl)
		mvprintw(0, 0, "Welcome to level %d (Peress any key to continue)", ++dlvl);	
	
	c = getch();
	
	return c;
}


int main(void)
{
    int c = 0; // input
    int rows, cols;	

    initscr(); // init curses
    keypad(stdscr, 1); // allow arrows, F1 - F12
	
    noecho(); // don't display user input
    curs_set(0); // hide cursor
	
    getmaxyx(stdscr, rows, cols);
	//printw("rows: %d, cols: %d", rows, cols);

    char map[rows][cols]; // 0 -- 119  0 -- 23
	
    do
    {
        c = game_loop(rows - 1, cols - 1, map); // rows-1 cause map[][] 0 ... 119
    }
    while (c != 27); // 27 = Esc
	
    refresh();
	getch();
    endwin();
	
    return 0;
}