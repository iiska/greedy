/*
 *      _     ____                   _          _  
 *     | |   / ___|_ __ ___  ___  __| |_   _   | | 
 *    / __) | |  _| '__/ _ \/ _ \/ _` | | | | / __)
 *    \__ \ | |_| | | |  __/  __/ (_| | |_| | \__ \
 *    (   /  \____|_|  \___|\___|\__,_|\__, | (   /
 *     |_|                             |___/   |_|
 *
 *     Verion: 0.0.1
 *     Author: Juhamatti Niemelä <juhamatti.niemela@edu.haapavesi.fi>
 *     29.03.2002
 *     
 */

#include <ncurses.h>
#include <stdlib.h>

#define VERSION "Version: 0.1.1"

#define S_MENU 0
#define S_NEWGAME 1
#define S_GAME 2
#define S_OVER 3

#define LEVEL_WIDTH 80
#define LEVEL_HEIGHT 23

// colors
#define P_COLOR 3
#define N_COLOR 1
#define H_COLOR 2

struct level_pos {
	int color;
	int number;
};

void makeLevel();
void litRoute();
void movePlayer(int x, int y);
int isGameOver();
int getRandomInt(int min, int max);
void game();
void quit(int ret_code);

struct level_pos level[LEVEL_WIDTH][LEVEL_HEIGHT];

int state = 0;
int changed = 1;

int px, py, score;
char score_str[12];

int main(int argc, char *argv[]) {
	srand(time((time_t *) 0));
	
	initscr();
	if (has_colors()) {
		int bg = COLOR_BLACK;
		start_color();
		init_pair(N_COLOR, COLOR_GREEN, bg);
		init_pair(H_COLOR, COLOR_CYAN, bg);
		init_pair(P_COLOR, COLOR_WHITE, bg);
	}
	else {
		printf("This game is very unplayable without colors.");
		quit(EXIT_FAILURE);
	}
	nl();
	noecho();
	curs_set(0);
	
	game();
	
	quit(0);
	return 0;
}

void game() {
	int i, j;
	
	while(1) {
		if (state == S_NEWGAME) {
			makeLevel();
			px = getRandomInt(0, LEVEL_WIDTH-1);
			py = getRandomInt(0, LEVEL_HEIGHT-1);
			state++;
			changed = 1;
			
			clear();
			for (i=0; i<LEVEL_WIDTH; i++) {
				mvaddch(LEVEL_HEIGHT,i,'-');
			}
		}

		if (state == S_GAME) {
			if (changed == 1) {
				litRoute();
				for (j=0; j<LEVEL_HEIGHT; j++) {
					for (i=0; i<LEVEL_WIDTH; i++) {
						attron(COLOR_PAIR(level[i][j].color));
						if ( level[i][j].number != 0 )
							mvaddch(j,i, '0'+level[i][j].number);
						else
							mvaddch(j,i,' ');
						level[i][j].color = N_COLOR;
					}
				}
			attron(COLOR_PAIR(P_COLOR));
			mvaddch(py, px, '@');
			sprintf(score_str, "%s %d", "Score:", score);
			mvaddstr(LEVEL_HEIGHT+1,LEVEL_WIDTH-12,score_str);
			refresh();
			changed = 0;
			}
		
			switch (getch()) {
				case '7':
					movePlayer(px-1,py-1);
					break;
				case '8':
					movePlayer(px,py-1);
					break;
				case '9':
					movePlayer(px+1,py-1);
					break;
				case '6':
					movePlayer(px+1,py);
					break;
				case '3':
					movePlayer(px+1,py+1);
					break;
				case '2':
					movePlayer(px,py+1);
					break;
				case '1':
					movePlayer(px-1,py+1);
					break;
				case '4':
					movePlayer(px-1,py);
					break;
			}
			if (isGameOver() == 1) {
				state++;
			}
		}
		else if (state == S_OVER) {
			clear();
			attron(COLOR_PAIR(N_COLOR));
			mvaddstr(LEVEL_HEIGHT / 2, (LEVEL_WIDTH - 9) / 2,
					"Game Over");
			sprintf(score_str, "%s %d", "Score: ", score);
			mvaddstr(LEVEL_HEIGHT / 2 + 1, (LEVEL_WIDTH - 12) / 2,
					score_str);
			mvaddstr(LEVEL_HEIGHT - 5,(LEVEL_WIDTH - 13) / 2,
					"Press any key");
			if (getch() != 0) {
				state = 0;
			}
		}
		else if (state == S_MENU) {
			if (changed == 1) {
				clear();
				attron(COLOR_PAIR(N_COLOR));
				// Writes large Greedy text
				mvaddstr(2,(LEVEL_WIDTH-33) / 2,
						"  ____                   _");
				mvaddstr(3,(LEVEL_WIDTH-33) / 2,
						" / ___|_ __ ___  ___  __| |_   _");
				mvaddstr(4,(LEVEL_WIDTH-33) / 2,
						"| |  _| '__/ _ \\/ _ \\/ _` | | | |");
				mvaddstr(5,(LEVEL_WIDTH-33) / 2,
						"| |_| | | |  __/  __/ (_| | |_| |");
				mvaddstr(6,(LEVEL_WIDTH-33) / 2,
						" \\____|_|  \\___|\\___|\\__,_|\\__, |");
				mvaddstr(7,(LEVEL_WIDTH-33) / 2,
						"                           |___/");
				mvaddstr(8,(LEVEL_WIDTH-33) / 2, VERSION);
				mvaddstr(10,(LEVEL_WIDTH-18) / 2,
						"s - Start new game");
				mvaddstr(11,(LEVEL_WIDTH-8) / 2, "q - Quit");
				refresh();
				changed = 0;
			}

			switch (getch()) {
				case 's':
					state++;
					break;
				case 'q':
					quit(0);
					break;
			}
		}
		
		usleep(80000L);
	}
}

void makeLevel() {
	int i, j;

	for (j=0; j<LEVEL_HEIGHT; j++) {
		for (i=0; i<LEVEL_WIDTH; i++) {
			level[i][j].number = getRandomInt(1,9);
			level[i][j].color = N_COLOR;
		}
	}
}


void litRoute() {
	int i, j, k;

	for (i = -1; i <= 1; i++) {
		for (j = -1; j <= 1; j++) {
			if ( (px+i >= 0) && (py+j >= 0) &&
					(px+i < LEVEL_WIDTH) &&
					(py+j < LEVEL_HEIGHT) ) {
				
				for ( k = 1; k <= level[px+i][py+j].number; k++)
				{
					if ( level[px+(i*k)][py+(j*k)].number
							== 0 ) {
						break;
					}
					else if ( (px+(i*k) >= 0) &&
						(py+(j*k) >= 0) &&
						(px+(i*k) < LEVEL_WIDTH)
						&& (py+(j*k) < LEVEL_HEIGHT) ) {
						
						level[px+(i*k)][py+(j*k)].color = H_COLOR;
					}
					else
						break;
				}
			}
		}
	}
}

void movePlayer(int x, int y) {
	int xc, yc, i, i_max;

	xc = x - px; yc = y - py;
	i_max = level[px+xc][py+yc].number;
	
	for ( i = 1; i <= i_max; i++) {
		if ( level[px+(i*xc)][py+(i*yc)].number == 0) {
			i_max = i-1;
			break;
		}
		if ( (px+(i*xc) >= 0) && (py+(i*yc) >= 0) &&
				(px+(i*xc) < LEVEL_WIDTH) &&
				(py+(i*yc) < LEVEL_HEIGHT) ) {
			score += level[px+(i*xc)][py+(i*yc)].number;
			level[px+(i*xc)][py+(i*yc)].number = 0;
		}
		else {
			i_max = i-1;
			break;
		}
	}

	level[px][py].number = 0;
	px += xc*i_max;
	py += yc*i_max;
	changed = 1;
}

int isGameOver() {
	int i, j;

	for (i=-1; i<=1; i++) {
		for (j=-1; j<=1; j++) {
			if ( (px+i >= 0) && (py+j >= 0) &&
					(px+i < LEVEL_WIDTH) &&
					(py+j < LEVEL_HEIGHT) &&
					level[px+i][py+j].number != 0 ) {
				return 0;
			}
		}
	}

	return 1;
}

void quit(int ret_code) {
	endwin();
	exit(ret_code);
}

int getRandomInt(int min, int max) {
	
	unsigned int rand_int;
	
	// Let's see if we have insane input
	if (min > max) {
		return 0;
	}
	
	rand_int = min + (rand() % (max - min + 1));
	
	return rand_int;
}
