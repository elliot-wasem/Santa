/**
 * @Author: Elliot Wasem
 * @Github: http://github.com/elliot-wasem
 * This project: http://github.com/elliot-wasem/Santa

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>
#include <errno.h>

// quick color definitions, for ease of use.
// colors declared inside main
#define GREEN  1
#define RED    2
#define BLUE   3
#define YELLOW 4
#define WHITE  5

struct point {
    int x;
    int y;
};

typedef struct point flake;
typedef struct point sleigh_pos;

int max_y, max_x;
flake *flakes;
sleigh_pos sleigh = {0, 0};
int num_reindeer = 3;

int init_flakes();
void draw_snowflakes();
void draw_reindeer(int pos_y, int pos_x, int version);
void draw_sleigh(int pos_y, int pos_x, int version);
void draw_string(int pos_y, int pos_x, char *str);

int main(int argc, char *argv[]) {
    int time;
    int c;

    /* timeout */
    time = 150;

    setlocale(LC_ALL, "");
    /* initialize screen */
    initscr();

    /* enables colors */
    start_color();

    /* initially refreshes screen, emptying it */
    refresh();

    /* keypresses will not be displayed on screen */
    noecho();

    /* set wait timeout to time */
    timeout(time);

    /* sets color pairs to numbers defined above */
    init_pair(GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(RED, COLOR_RED, COLOR_BLACK);
    init_pair(BLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(WHITE, COLOR_WHITE, COLOR_BLACK);

    getmaxyx(stdscr, max_y, max_x);

    if (init_flakes() == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }
    int version = 0;
    sleigh.x = max_x;
    sleigh.y = (max_y / 2) - 5;
    do {
        if (max_y > 22 && max_x > 43) {
            sleigh.x -= 1;
            erase();
            draw_snowflakes();
            for (int i = 0; i < num_reindeer; i++) {
                draw_reindeer(sleigh.y, sleigh.x + (i * 11), (version+(i%3))%3);
            }
            draw_sleigh(sleigh.y, sleigh.x+(num_reindeer*11), version);

            mvprintw(max_y-9, (max_x/2)-22, "+------------------------------------------+");
            mvprintw(max_y-8, (max_x/2)-22, "| Add/remove reindeer (max 10, min 0): a/s |");
            mvprintw(max_y-7, (max_x/2)-22, "|            Inc/dec speed: +/-            |");
            mvprintw(max_y-6, (max_x/2)-22, "|               quit: q                    |");
            mvprintw(max_y-5, (max_x/2)-22, "|                                          |");
            mvprintw(max_y-4, (max_x/2)-22, "|           Author: Elliot Wasem           |");
            mvprintw(max_y-3, (max_x/2)-22, "|     https://github.com/elliot-wasem/     |");
            mvprintw(max_y-2, (max_x/2)-22, "+------------------------------------------+");
            if (version != 0) {
                mvprintw(2, (max_x/2)-18, "Merry christmas and happy new year!");
            }
            move(max_y-1, max_x-1);
            c = getch();
            if (c == '+') {
                time = (time - 5 <= 0 ? 0 : time - 5);
                timeout(time);
            } else if (c == '-') {
                time = (time + 5 > 500 ? 500 : time + 5);
                timeout(time);
            } else if (c == 'a') {
                num_reindeer = (num_reindeer >= 10 ? 10 : num_reindeer+1);
            } else if (c == 's') {
                num_reindeer = (num_reindeer <= 0 ? 0 : num_reindeer-1);
            } else if (c == KEY_RESIZE) {
                getmaxyx(stdscr, max_y, max_x);
                sleigh.y = (max_y / 2) - 5;
                if (sleigh.x > max_x) {
                    sleigh.x = max_x;
                }
            }
            version = (version+1) % 3;
            if (sleigh.x < -(num_reindeer*11+22)) {
                sleigh.x = max_x;
            }
        } else {
            c = getch();
            if (c == KEY_RESIZE) {
                getmaxyx(stdscr, max_y, max_x);
                sleigh.y = max_y / 2;
                if (sleigh.x > max_x) {
                    sleigh.x = max_x;
                }
            }
            erase();
            mvprintw(1, 1, "Screen size must be");
            mvprintw(2, 1, "at least 23 high");
            mvprintw(3, 1, "and 44 wide.");
        }
        refresh();
    } while (c != 'q');
    endwin();
}

int init_flakes() {
    // big enough that even on super wide screens, the children can still see the snow.
    // this isn't meant as an industrial application after all, just something to make you smile :)
    flakes = malloc(sizeof(flake) * 1000);

    if (flakes == NULL) {
        endwin();
        fprintf(stderr, "ERROR: Malloc failed. %s.\n", strerror(errno));
        return EXIT_FAILURE;
    }
    for(int i = 0; i < 1000; i++) {
	flakes[i].x = i;
	flakes[i].y = rand() % max_y;
    }
    return EXIT_SUCCESS;
}

void draw_snowflakes() {
    for(int i = 0; i < max_x; i++) {
	int random = rand() % 3;
        mvprintw(flakes[i].y, flakes[i].x, "%c", (random == 2 ? '*' : (random == 1 ? '+' : 'x')));
	if(flakes[i].y < max_y-1)
	    flakes[i].y++;
        else {
            flakes[i].y = max_y-1;
        }
    }
    flakes[rand() % max_x].y = 0;
}

void draw_reindeer(int pos_y, int pos_x, int version) {
    attron(COLOR_PAIR(YELLOW));
    draw_string(pos_y+1, pos_x+3,    "#");
    attroff(COLOR_PAIR(YELLOW));
    attron(COLOR_PAIR(RED));
    draw_string(pos_y+2, pos_x+0, "o");
    attroff(COLOR_PAIR(RED));
    attron(COLOR_PAIR(YELLOW));
    draw_string(pos_y+2, pos_x+1, "<##"); draw_string(pos_y+2, pos_x+10, "/");
    draw_string(pos_y+3, pos_x+3,    "#######");
    attroff(COLOR_PAIR(YELLOW));
    draw_string(pos_y+4, pos_x+3,    "===========");
    attron(COLOR_PAIR(YELLOW));
    switch (version) {
    case 0:
        draw_string(pos_y+5, pos_x+2,   "/ |    | \\");
        break;
    case 1:
        draw_string(pos_y+5, pos_x+2,   "  | \\  \\)");
        break;
    case 2:
        draw_string(pos_y+5, pos_x+2,   "(/      |\\");
        break;
    }
    attroff(COLOR_PAIR(YELLOW));
}

void draw_sleigh(int pos_y, int pos_x, int version) {
    attron(COLOR_PAIR(RED));
    draw_string(pos_y+0, pos_x+10, ">");
    draw_string(pos_y+1, pos_x+9, "@");
    draw_string(pos_y+0, pos_x+9, "@");
    draw_string(pos_y+3, pos_x+2, "___________________");
    draw_string(pos_y+2, pos_x+7, "@");
    draw_string(pos_y+3, pos_x+7, "@");
    draw_string(pos_y+2, pos_x+8, "@");
    draw_string(pos_y+3, pos_x+8, "@");
    attron(A_UNDERLINE);
    draw_string(pos_y+3, pos_x+9, "############");
    attroff(A_UNDERLINE);
    draw_string(pos_y+0, pos_x+8, "@");
    attroff(COLOR_PAIR(RED));
    draw_string(pos_y+0, pos_x+11, "o");
    attron(COLOR_PAIR(YELLOW));
    draw_string(pos_y+1, pos_x+7, "#");
    draw_string(pos_y+1, pos_x+8, "#");
    draw_string(pos_y+0, pos_x+7, "#");
    attroff(COLOR_PAIR(YELLOW));
    draw_string(pos_y+2, pos_x+6, "/");
    draw_string(pos_y+3, pos_x+6, "|");
    draw_string(pos_y+4, pos_x+2, "==================");
    attron(COLOR_PAIR(RED));
    draw_string(pos_y+5, pos_x+4, "################");
    attroff(COLOR_PAIR(RED));
    draw_string(pos_y+6, pos_x+2, "\\__|__________|_____");
    attron(COLOR_PAIR(BLUE));
    draw_string(pos_y+2, pos_x+10, "@@@@@@@@@@@");
    draw_string(pos_y+1, pos_x+11, "@@@@@@@@@");
    draw_string(pos_y, pos_x+15, "\\/");
    attroff(COLOR_PAIR(BLUE));
}

void draw_string(int pos_y, int pos_x, char *str) {
    if (pos_y < 0 || pos_y >= max_y) {
        return;
    }
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        if (pos_x + i < 0) {
            continue;
        }
        mvprintw(pos_y, pos_x+i, "%c", str[i]);
    }
}
