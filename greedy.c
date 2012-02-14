/*
 *      _     ____                   _          _  
 *     | |   / ___|_ __ ___  ___  __| |_   _   | | 
 *    / __) | |  _| '__/ _ \/ _ \/ _` | | | | / __)
 *    \__ \ | |_| | | |  __/  __/ (_| | |_| | \__ \
 *    (   /  \____|_|  \___|\___|\__,_|\__, | (   /
 *     |_|                             |___/   |_|
 *
 *     Author: Juhamatti Niemelä <juhamatti.niemela@edu.haapavesi.fi>
 *     29.03.2002
 *     
 */

#include <ncurses.h>
#include <stdlib.h>

#define VERSION "Version: 0.1.5"

#define S_MENU 0
#define S_NEWGAME 1
#define S_GAME 2
#define S_OVER 3
#define S_SCORES 4

#define LEVEL_WIDTH 80
#define LEVEL_HEIGHT 23
#define HS_STRINGLEN 33

// colors
#define P_COLOR 3
#define N_COLOR 1
#define H_COLOR 2

#define HISCORE_FILE "/var/lib/games/greedy.scores"

struct level_pos {
	int color;
	int number;
};

struct score_entry {
	int s;
	char n[20];
};

void makeLevel();
void litRoute();
void movePlayer(int x, int y);
int isGameOver();
int getRandomInt(int min, int max);
void game();
void quit(int ret_code);
void usage();

FILE *openScoreFile(char *mode);
void loadScores();
void saveScores();
void addScoreEntry(char *nname, int nscore);


struct level_pos level[LEVEL_WIDTH][LEVEL_HEIGHT];

struct score_entry hiscores[10];

int state = 0;
int changed = 1;

int px, py, score;
char score_str[12];



int main(int argc, char *argv[]) {
	int transparent_bg = 0;
	int optchr;
	/* getopt code from cmatrix
	 * http://www.asty.org/cmatrix
	 *
	 * There aren't too many clargs yet, but this code will be easier
	 * to develop later, when there are more options...
	 */
	while ( (optchr = getopt(argc, argv, "t")) != EOF) {
		switch (optchr) {
			case 't':
				transparent_bg = 1;
				break;
			case 'h':
			case '?':
				usage();
				exit(0);
		}
	}
	srand(time((time_t *) 0));
	
	initscr();
	if (has_colors()) {
		int bg = COLOR_BLACK;
		if (transparent_bg) {
			if (use_default_colors() != ERR) {
				bg = -1;
			}
		}
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
	// temp-string for hiscore-screenformatting
	char tmpstr[20];
	
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
			mvaddstr((LEVEL_HEIGHT-4) / 2,(LEVEL_WIDTH-41) / 2,
					"  ___                   ___");
			mvaddstr((LEVEL_HEIGHT-4) / 2 + 1,(LEVEL_WIDTH-41) / 2,
					" / __|__ _ _ __  ___   / _ \\__ _____ _ _");
			mvaddstr((LEVEL_HEIGHT-4) / 2 + 2,(LEVEL_WIDTH-41) / 2,
					"| (_ / _` | '  \\/ -_) | (_) \\ V / -_) '_|");
			mvaddstr((LEVEL_HEIGHT-4) / 2 + 3,(LEVEL_WIDTH-41) / 2,
					" \\___\\__,_|_|_|_\\___|  \\___/ \\_/\\___|_|");
			sprintf(score_str, "%s %d", "Score: ", score);
			mvaddstr((LEVEL_HEIGHT -4) / 2 + 5, (LEVEL_WIDTH - 12) / 2,
					score_str);
			mvaddstr(LEVEL_HEIGHT - 5,(LEVEL_WIDTH - 13) / 2,
					"Press any key");
			
			if (getch() != 0) {
				if (score > hiscores[9].s) {
					clear();
					printw("You made hiscore! What's your name?\n\nName: ");
					refresh();
					echo();
					getstr(tmpstr);
					noecho();
					addScoreEntry(tmpstr, score);
					// We don't need scores anymore...
					score = 0;
					state = S_SCORES;
					changed = 1;
				}
				else
					state = S_MENU;
			}
		}
		else if (state == S_SCORES) {
			if ( hiscores[0].s == 0 )
				loadScores();
			if (changed == 1) {
			  clear();
			  attron(COLOR_PAIR(N_COLOR));
			  mvaddstr(1, (LEVEL_WIDTH-30) / 2, " _  _ _");
			  mvaddstr(2, (LEVEL_WIDTH-30) / 2, "| || (_)___ __ ___ _ _ ___ ___");
			  mvaddstr(3, (LEVEL_WIDTH-30) / 2, "| __ | (_-</ _/ _ \\ '_/ -_|_-<");
			  mvaddstr(4, (LEVEL_WIDTH-30) / 2, "|_||_|_/__/\\__\\___/_| \\___/__/");

			  // Highlight the first three
			  attron(COLOR_PAIR(H_COLOR));
			  for (i=0;i<3;i++) {
				  sprintf(tmpstr, " %d.", i+1);
				  mvaddstr(6+i, (LEVEL_WIDTH - HS_STRINGLEN) / 2, tmpstr);
				  mvaddstr(6+i, (LEVEL_WIDTH - HS_STRINGLEN) / 2 + 5, hiscores[i].n);
				  sprintf(tmpstr, "%d", hiscores[i].s);
				  mvaddstr(6+i, (LEVEL_WIDTH - HS_STRINGLEN) / 2 + 27, tmpstr);
			  }
			  attron(COLOR_PAIR(N_COLOR));
			  for (i=3;i<9;i++) {
                                  sprintf(tmpstr, " %d.", i+1);
				  mvaddstr(6+i, (LEVEL_WIDTH - HS_STRINGLEN) / 2, tmpstr);
				  mvaddstr(6+i, (LEVEL_WIDTH - HS_STRINGLEN) / 2 + 5, hiscores[i].n);
				  sprintf(tmpstr, "%d", hiscores[i].s);
				  mvaddstr(6+i, (LEVEL_WIDTH - HS_STRINGLEN) / 2 + 27, tmpstr);
			  }
			  sprintf(tmpstr, "%d.", 10);
			  mvaddstr(15, (LEVEL_WIDTH - HS_STRINGLEN) / 2, tmpstr);
			  mvaddstr(15, (LEVEL_WIDTH - HS_STRINGLEN) / 2 + 5, hiscores[9].n);
			  sprintf(tmpstr, "%d", hiscores[9].s);
			  mvaddstr(15, (LEVEL_WIDTH - HS_STRINGLEN) / 2 + 27, tmpstr);
			  
			  mvaddstr(LEVEL_HEIGHT - 4,(LEVEL_WIDTH - 13) / 2,
					"Press any key");
			  changed = 0;
			}
			if (getch() != 0) {
				state = S_MENU;
				changed = 1;
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
				mvaddstr(11,(LEVEL_WIDTH-17) / 2,
						"h - Show Hiscores");
				mvaddstr(12,(LEVEL_WIDTH-8) / 2, "q - Quit");
				refresh();
				changed = 0;
			}

			switch (getch()) {
				case 's':
					state++;
					break;
				case 'h':
					state = S_SCORES;
					changed = 1;
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

void usage() {
	printf("Greedy %s%s", VERSION, "\n");
	printf("Usage: greedy -[ht]\n");
	printf("  -h: Print help and exit\n");
	printf("  -t: Transparent background color\n");
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


/* 
 * Begin Hiscores handling code
 *
 * Looked little help from Penguin-Command sources
 * You can get Penguin-Command from  http://www.linux-games.com
 */

FILE *openScoreFile(char *mode) {
	FILE *file;

	if ( (file = fopen(HISCORE_FILE, mode)) == NULL) {
		printf("\nUnable to open hiscore file \"%s\" ", HISCORE_FILE);
		if ( strcmp(mode, "r") == 0)
			printf("for read.\n");
		else if ( strcmp(mode, "w") == 0)
			printf("for write.\n");
	}

	return file;
}

void loadScores() {
	int i;
	FILE *file;

	if ( (file = openScoreFile("r")) == NULL) {
		if ( (file = openScoreFile("w")) == NULL ) {
			quit(EXIT_FAILURE);
		}
		for (i=0;i<10;i++) {
			// TODO: Add random names from credits to this later
			fprintf(file, "Player\n");
			fprintf(file, "%d\n", 1000-i*100);
		}
		fclose(file);
		file = openScoreFile("r");
	}

	for (i=0;i<10;i++) {
		fscanf(file, "%s\n", hiscores[i].n);
		fscanf(file, "%d\n", &hiscores[i].s);
	}

	fclose(file);
}

void saveScores() {
	int i;
	FILE* file;

	if ( (file = openScoreFile("w")) == NULL) {
		quit(EXIT_FAILURE);
	}

	for (i=0;i<10;i++) {
		fprintf(file, "%s\n", hiscores[i].n);
		fprintf(file, "%d\n", hiscores[i].s);
	}
	
	fclose(file);
}

void addScoreEntry(char *nname, int nscore) {
	int i,j;
	
	if (hiscores[0].s == 0) {
		loadScores();
	}

	for (i=0;i<10;i++) {
		if (score > hiscores[i].s) {
			/*
			 * Move score entries lower to make room for new one
			 */
			for (j=8;j>=i;j--) {
				hiscores[j+1].s = hiscores[j].s;
				sprintf(hiscores[j+1].n, "%s", hiscores[j].n);
			}
			// Add new score entry
			hiscores[i].s = nscore;
			sprintf(hiscores[i].n, "%s", nname);
			break;
		}
	}

	saveScores();
}
