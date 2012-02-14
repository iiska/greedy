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
void game();
int getRandomInt(int min, int max);


struct level_pos level[LEVEL_WIDTH][LEVEL_HEIGHT];
int state = 1;
int changed = 0;

int px, py, score;

FILE *random_dev;

int main(int argc, char *argv[]) {

	if ( (random_dev = fopen("/dev/urandom","rb")) == NULL) {
		printf("Unable to open /dev/urandom");
		return EXIT_FAILURE;
	}
	
	initscr();
	if (has_colors()) {
		int bg = COLOR_BLACK;
		start_color();
		init_pair(N_COLOR, COLOR_GREEN, bg);
		init_pair(H_COLOR, COLOR_CYAN, bg);
		init_pair(P_COLOR, COLOR_WHITE, bg);
	}
	nl();
	noecho();
	curs_set(0);

	game();
	
	endwin();
	fclose(random_dev);
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
			//mvaddstr(LEVEL_HEIGHT+1,0,"Score: "+score);
			refresh();
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
		}
		else if (state == S_OVER) {
			mvaddstr(LEVEL_HEIGHT / 2, (LEVEL_WIDTH - 9) / 2, "Game Over");
			if (getch() != 0) {
				state = 0;
			}
		}
		
		if ( (state == S_GAME) && (isGameOver() == 1) ) {
			state++;
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
			if ( (px+i >= 0) && (py+j >= 0) && (px+i < LEVEL_WIDTH) && (py+j < LEVEL_HEIGHT) ) {
				for ( k = 1; k <= level[px+i][py+j].number; k++) {
					if ( level[px+(i*k)][py+(j*k)].number == 0 ) {
						break;
					}
					else if ( (px+(i*k) >= 0) && (py+(j*k) >= 0) && (px+(i*k) < LEVEL_WIDTH) && (py+(j*k) < LEVEL_HEIGHT) ) {
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
		if ( (px+(i*xc) >= 0) && (py+(i*yc) >= 0) && (px+(i*xc) < LEVEL_WIDTH) && (py+(i*yc) < LEVEL_HEIGHT) ) {
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

int getRandomInt(int min, int max) {
	
	unsigned int rand_int;
	
	// Let's see if we have insane input
	if (min > max) {
		return 0;
	}

	if ( fread(&rand_int, 1, sizeof(rand_int),random_dev) != sizeof(rand_int) ) {
		printf("Unable to read /dev/urandom");
		return -1;
	}
	
	rand_int = min + (rand_int % (max - min + 1));
	return rand_int;
}
