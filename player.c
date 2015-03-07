#include <ncurses.h>

#include "config.h"
#include "player.h"
#include "screen.h"
#include "map.h"
#include "rungame.h"

int init_player(dungeon_t *dungeon) {

	int x = rand() % DUNG_X;
	int y = rand() % DUNG_Y;

	for(;;) {
		if(dungeon->map[y][x] == ter_room) {
			break;
		}
		if(dungeon->map[y][x] == ter_corridor) {
			break;
		}

		x = rand() % DUNG_X;
		y = rand() % DUNG_Y;
	}

	character_t play = { x, y, 10, PLAYER_CHAR };
	player_t tmp = { x, y, 10 };
	play.p = &tmp;
	dungeon->player = play;
	
	return 1;
	
}

int move_player(dungeon_t *dungeon) {

	int player_move = getch();
	switch(player_move) {

		/* move up one, left one */
		case '7':
		case 'y':
			if(!dungeon->look_mode) {
				if(dungeon->map[dungeon->player.y - 1][dungeon->player.x - 1] != ter_wall && dungeon->map[dungeon->player.y - 1][dungeon->player.x - 1] == ter_corridor) {
					dungeon->player.x = dungeon->player.x - 1;
					dungeon->player.y = dungeon->player.y - 1;
				}
			}

			break;

		/* move up one */
		case '8':
		case 'k':
		case 'w':
			if(dungeon->look_mode) {
				if(dungeon->look_mode_x == -1) {
					dungeon->look_mode_x = dungeon->player.x;
				}
				if(dungeon->look_mode_y == -1) {
					dungeon->look_mode_y = dungeon->player.y;
				}

				dungeon->look_mode_y -= DRAW_Y;

				update_term(dungeon, dungeon->look_mode_x, dungeon->look_mode_y);

			} else {
				if(dungeon->map[dungeon->player.y - 1][dungeon->player.x] != ter_wall && dungeon->map[dungeon->player.y - 1][dungeon->player.x] != ter_immutable) {
					dungeon->player.y = dungeon->player.y - 1;
				}
			}

			break;

		/* move up one, right one */
		case '9':
		case 'u':
			if(!dungeon->look_mode) {
				if(dungeon->map[dungeon->player.y - 1][dungeon->player.x + 1] != ter_wall && dungeon->map[dungeon->player.y - 1][dungeon->player.x + 1] != ter_immutable) {
					dungeon->player.y = dungeon->player.y - 1;
					dungeon->player.x = dungeon->player.x + 1;
				}
			}
			break;

		/* move right */
		case '6':
		case 'l':
		case 'd':
			if(dungeon->look_mode) {
				if(dungeon->look_mode_x == -1) {
					dungeon->look_mode_x = dungeon->player.x;
				}
				if(dungeon->look_mode_y == -1) {
					dungeon->look_mode_y = dungeon->player.y;
				}

				dungeon->look_mode_x += DRAW_X;

				update_term(dungeon, dungeon->look_mode_x, dungeon->look_mode_y);

			} else {
				if(dungeon->map[dungeon->player.y][dungeon->player.x + 1] != ter_wall && dungeon->map[dungeon->player.y][dungeon->player.x + 1] != ter_immutable) {
					dungeon->player.x = dungeon->player.x + 1;
				}
			}
			break;

		/* move down one, right one */
		case '3':
		case 'n':
			if(!dungeon->look_mode) {
				if(dungeon->map[dungeon->player.y + 1][dungeon->player.x + 1] != ter_wall && dungeon->map[dungeon->player.y + 1][dungeon->player.x + 1] != ter_immutable) {
					dungeon->player.x = dungeon->player.x + 1;
					dungeon->player.y = dungeon->player.y + 1;
				}
			}
			break;

		/* move down one */
		case '2':
		case 'j':
		case 's':
			if(dungeon->look_mode) {
				if(dungeon->look_mode_x == -1) {
					dungeon->look_mode_x = dungeon->player.x;
				}
				if(dungeon->look_mode_y == -1) {
					dungeon->look_mode_y = dungeon->player.y;
				}

				dungeon->look_mode_y += DRAW_Y;

				update_term(dungeon, dungeon->look_mode_x, dungeon->look_mode_y);

			} else {
				if(dungeon->map[dungeon->player.y + 1][dungeon->player.x] != ter_wall && dungeon->map[dungeon->player.y + 1][dungeon->player.x] != ter_immutable) {
					dungeon->player.y = dungeon->player.y + 1;
				}
			}
			break;

		/* move down one, left one */
		case '1':
		case 'b':
			if(!dungeon->look_mode) {
				if(dungeon->map[dungeon->player.y + 1][dungeon->player.x - 1] != ter_wall && dungeon->map[dungeon->player.y + 1][dungeon->player.x - 1] != ter_immutable) {
					dungeon->player.x = dungeon->player.x - 1;
					dungeon->player.y = dungeon->player.y + 1;
				}
			}
			break;

		/* move left one */
		case '4':
		case 'h':
		case 'a':
			if(dungeon->look_mode) {
				if(dungeon->look_mode_x == -1) {
					dungeon->look_mode_x = dungeon->player.x;
				}
				if(dungeon->look_mode_y == -1) {
					dungeon->look_mode_y = dungeon->player.y;
				}

				dungeon->look_mode_x -= DRAW_X;

				update_term(dungeon, dungeon->look_mode_x, dungeon->look_mode_y);

			} else {
				if(dungeon->map[dungeon->player.y][dungeon->player.x - 1] != ter_wall && dungeon->map[dungeon->player.y][dungeon->player.x - 1] != ter_immutable) {
					dungeon->player.x = dungeon->player.x - 1;
				}
			}
			break;

		/* go up a set of stairs */
		case '<':
			if(!dungeon->look_mode && dungeon->map[dungeon->player.y][dungeon->player.x] == ter_stair_up) {
				dungeon_t dung;
				generate_map(&dung);
				run_game(&dung);
			}
			break;

		/* go down a set of stairs */
		case '>':
			if(!dungeon->look_mode && dungeon->map[dungeon->player.y][dungeon->player.x] == ter_stair_down) {
				dungeon_t dung;
				generate_map(&dung);
				run_game(&dung);
			}

		/* skip a turn */
		case ' ':
			break;

		/* enter look mode */
		case 'L':
			dungeon->look_mode = 1;
			break;

		/* save and quit */	
		case 'S':
			// save
			endwin();
			exit(1);
			break;

		/* switch back to control mode */
		case 27:
			if(dungeon->look_mode) {
				dungeon->look_mode = 0;
			}
			break;

	}

	dijkstra(dungeon);

	return 1;

}
