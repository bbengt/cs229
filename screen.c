#include <ncurses.h>
#include <unistd.h>

#include "screen.h"

void init_term() {
	initscr();
	raw();
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);
}

int update_term(dungeon_t *dungeon, int x, int y) {

	/* calculate what to render around x and y */
	int left_cutoff = x - (DRAW_X / 2);
	int right_cutoff = x + (DRAW_X / 2);
	int top_cutoff = y - (DRAW_Y / 2);
	int bottom_cutoff = y + (DRAW_Y / 2);

	/* make sure we're not rendering off the map */
	if(left_cutoff < 0) {
		left_cutoff = 0;
	}
	if(right_cutoff > DUNG_X) {
		right_cutoff = DUNG_X;
	}
	if(top_cutoff < 0) {
		top_cutoff = 0;
	}
	if(bottom_cutoff > DUNG_Y) {
		bottom_cutoff = DUNG_Y;
	}

	int i, j, k, l;
	i = j = 0;

	clear();

	/* fill terrain appropriately */
	for(l = top_cutoff; l < bottom_cutoff; l++) {
		for(k = left_cutoff; k < right_cutoff; k++) {

			switch(dungeon->map[l][k]) {
				case ter_wall:
				case ter_immutable:
					mvaddch(i, j, '#');
					break;
				case ter_corridor:
				case ter_room:
					mvaddch(i, j, '.');
					break;
				case ter_stair_up:
					mvaddch(i, j, '<');
					break;
				case ter_stair_down:
					mvaddch(i, j, '>');
					break;
				case ter_debug:
					mvaddch(i, j, '*');
					break;
			}
			j++;

		}
		j = 0;
		i++;
	}

	/* place monsters */
	for(i = 0; i < dungeon->num_mons; i++) {
		if(dungeon->mons[i].y < bottom_cutoff && dungeon->mons[i].y > top_cutoff && dungeon->mons[i].x < right_cutoff && dungeon->mons[i].x > left_cutoff) {

			mvaddch(dungeon->mons[i].y - top_cutoff, dungeon->mons[i].x - left_cutoff, dungeon->mons[i].type);

		}

	}

	/* place player */
	if(dungeon->player.y < bottom_cutoff && dungeon->player.y > top_cutoff && dungeon->player.x < right_cutoff && dungeon->player.x > left_cutoff) {

		mvaddch(dungeon->player.y - top_cutoff, dungeon->player.x - left_cutoff, dungeon->player.type);

	}

	refresh();

	return 0;

}