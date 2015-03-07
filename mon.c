#include <limits.h>
#include <math.h>
#include <ncurses.h>

#include "mon.h"

/*
*
* print_dij(): updates the game map in dungeon to have ter_debug along the shortest path as determined by dijkstra()
*
*/

void print_dij(dungeon_t *dungeon, path_t *node) {

	if(node != NULL) {
		dungeon->map[node->y][node->x] = ter_debug;
		print_dij(dungeon, node->prev);
	} else {
		return;
	}

}

/*
*
* dijkstra(): finds the shortest path from (start_x, start_y) to (end_x, end_y) while only traversing open spaces
*
*/
// path_t *last;
// int dijkstra(dungeon_t *dungeon, int start_x, int start_y, int end_x, int end_y) {

// 	queue_t qu;
// 	qu.num_elements = 0;
// 	queue_t *q = &qu;

// 	// create a path representation of each cell on the map - set each cost to "infinite" unless it's the source node (which has cost 0)
// 	uint32_t x, y;
// 	for(y = 0; y < DUNG_Y; y++) {
// 		for(x = 0; x < DUNG_X; x++) {
// 			int cost = INT_MAX;
// 			if(x == start_x && y == start_y) { // source node
// 				cost = 0;
// 			}
// 			add_element(q, x, y, cost);
// 		}
// 	}

// 	path_t *u;

// 	// loop through the queue, removing the node with the smallest cost each time
// 	for(;;) {
// 		u = remove_min(q);
// 		u->scanned = 1;
// 		if(u->x == end_x && u->y == end_y) {
// 			break;
// 		}

// 		uint32_t x, y;
// 		x = u->x;
// 		y = u->y;
// 		uint32_t i, j;

// 		// look at each neighbor of u.  If we haven't already scanned it, calculate its cost. 
// 		for(j = y - 1; j < y + 2; j++) {
// 			for(i = x - 1; i < x + 2; i++) {

// 				// make sure we don't run off the edge of the map
// 				if(i < 0) {
// 					i = 0;
// 				}
// 				if(i > DUNG_X - 1) {
// 					i = DUNG_X - 1;
// 				}
// 				if(j < 0) {
// 					j = 0;
// 				}
// 				if(j > DUNG_Y - 1) {
// 					j = DUNG_Y - 1;
// 				}

// 				path_t *v = fetch_at_pos(q, i, j);

// 				// if we haven't already scanned v, make sure it's an open space before updating the cost (u's cost + 1) 
// 				if(v->scanned == 0) {
// 					if(dungeon->map[j][i] == ter_corridor || dungeon->map[j][i] == ter_room) {
// 						int tmp = u->cost + 1;
// 						if(tmp < v->cost) {
// 							v->cost = tmp;
// 							v->prev = u;
// 						}
// 					}
// 				}
// 			}
// 		}
// 	}

// 	last = u;

// 	return 1;

// }

/*
*
* init_mons(): initialize monsters & place on dungeon map
*
*/

int init_mons(dungeon_t *dungeon) {

	character_t c;
	char mon_types[] = { 'p', 'P', 'D', 'g' };
	int i;
	for(i = 0; i < dungeon->num_mons; i++) {

		uint32_t x = rand() % DUNG_X;
		uint32_t y = rand() % DUNG_Y;
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

		c.speed = 5 + rand() % 16;
		c.x = x;
		c.y = y;
		c.type = mon_types[rand() % 4];
		c.alive = 1;

		mon_t m;
		m.telepathic = rand() % 2;
		m.smart = rand() % 2;

		// initially set last saw location to player location.  This will be updated accordingly in the second turn.
		m.last_saw_x = dungeon->player.x;
		m.last_saw_y = dungeon->player.y;

		c.m = &m;
		c.p = NULL;

		dungeon->mons[i] = c;

	}

	return 1;

}

int can_see(dungeon_t *dungeon, character_t *mon) {

	if(dungeon->player.x == mon->x) {
		int smaller, larger;
		if(dungeon->player.y < mon->y) {
			smaller = dungeon->player.y;
			larger = mon->y;
		} else {
			smaller = mon->y;
			larger = dungeon->player.y;
		}
		int i;
		for(i = smaller; i < larger; i++) {
			if(dungeon->map[i][mon->x] != ter_corridor || dungeon->map[i][mon->x] != ter_room) {
				return 1;
			}
		}

	}
	if(dungeon->player.y == mon->y) {
		int smaller, larger;
		if(dungeon->player.x < mon->x) {
			smaller = dungeon->player.x;
			larger = mon->x;
		} else {
			smaller = mon->x;
			larger = dungeon->player.x;
		}
		int i;
		for(i = smaller; i < larger; i++) {
			if(dungeon->map[mon->y][i] != ter_corridor || dungeon->map[mon->y][i] != ter_room) {
				return 1;
			}
		}
	}

	return 0;
}

/*
*
* move_mon(): moves a monster mon
*
*/

int move_mon(dungeon_t *dungeon, character_t *mon) {

	int play_x, play_y;
	if(mon->m == NULL) {
		return 0;
	}

	/* figure out where to move to next */
	if(!mon->m->telepathic) {
		if(can_see(dungeon, mon)) {
			mon->m->last_saw_x = dungeon->player.x;
			mon->m->last_saw_y = dungeon->player.y;
		} else {
			mon->m->last_saw_x = -1;
			mon->m->last_saw_y = -1;
		}
		play_x = mon->m->last_saw_x;
		play_y = mon->m->last_saw_y;
	} else {
		play_x = dungeon->player.x;
		play_y = dungeon->player.y;
	}

	if(mon->m->smart) {
		int start_x = mon->x;
		int start_y = mon->y;
		int to_beat = INT_MAX;

		// figure out which path results in the shortest path to the player and move to that square
		if(dungeon->distance[start_y + 1][start_x] < to_beat) {
			mon->y = start_y + 1;
			to_beat = dungeon->distance[start_y + 1][start_x];
		}
		if(dungeon->distance[start_y - 1][start_x] < to_beat) {
			mon->y = start_y - 1;
			to_beat = dungeon->distance[start_y - 1][start_x];
		}
		if(dungeon->distance[start_y][start_x + 1] < to_beat) {
			mon->x = start_x + 1;
			to_beat = dungeon->distance[start_y][start_x + 1];
		}
		if(dungeon->distance[start_y][start_x - 1] < to_beat) {
			mon->x = start_x - 1;
			to_beat = dungeon->distance[start_y][start_x - 1];
		}
		if(dungeon->distance[start_y + 1][start_x - 1] < to_beat) {
			mon->x = start_x - 1;
			mon->y = start_y + 1;
			to_beat = dungeon->distance[start_y + 1][start_x - 1];
		}
		if(dungeon->distance[start_y + 1][start_x + 1] < to_beat) {
			mon->x = start_x + 1;
			mon->y = start_y + 1;
			to_beat = dungeon->distance[start_y + 1][start_x - 1];
		}
		if(dungeon->distance[start_y - 1][start_x - 1] < to_beat) {
			mon->x = start_x - 1;
			mon->y = start_y - 1;
			to_beat = dungeon->distance[start_y - 1][start_x - 1];
		}
		if(dungeon->distance[start_y - 1][start_x + 1] < to_beat) {
			mon->x = start_x + 1;
			mon->y = start_y - 1;
			to_beat = dungeon->distance[start_y - 1][start_x + 1];
		}


	} else {
		if(play_x == -1 || play_y == -1) {
			return 1;
		}

		int start_euc = sqrt(((play_x - mon->x)^2) + ((play_y - mon->y)^2));

		if(dungeon->map[mon->y][mon->x - 1] == ter_room || dungeon->map[mon->y][mon->x - 1] == ter_corridor) {
			if(sqrt(((play_x - (mon->x - 1))^2) + ((play_y - mon->y)^2)) < start_euc) {
				dungeon->map[mon->y][mon->x - 1] = ter_debug;
				mon->x = mon->x - 1;
				mvaddch(mon->x, mon->y, mon->type);
			}
		}
		if(dungeon->map[mon->y][mon->x + 1] == ter_room || dungeon->map[mon->y][mon->x + 1] == ter_corridor) {
			if(sqrt(((play_x - (mon->x + 1))^2) + ((play_y - mon->y)^2)) < start_euc) {
				dungeon->map[mon->y][mon->x + 1] = ter_debug;
				mon->x = mon->x + 1;
			}
		}
		if(dungeon->map[mon->y - 1][mon->x] == ter_room || dungeon->map[mon->y - 1][mon->x] == ter_corridor) {
			if(sqrt(((play_x - (mon->x))^2) + ((play_y - mon->y - 1)^2)) < start_euc) {
				dungeon->map[mon->y - 1][mon->x] = ter_debug;
				mon->y = mon->y - 1;
			}
		}
		if(dungeon->map[mon->y + 1][mon->x] == ter_room || dungeon->map[mon->y + 1][mon->x] == ter_corridor) {
			if(sqrt(((play_x - (mon->x))^2) + ((play_y - mon->y + 1)^2)) < start_euc) {
				dungeon->map[mon->y + 1][mon->x] = ter_debug;
				mon->y = mon->y + 1;
			}
		}
		if(dungeon->map[mon->y + 1][mon->x - 1] == ter_room || dungeon->map[mon->y + 1][mon->x - 1] == ter_corridor) {
			if(sqrt(((play_x - (mon->x - 1))^2) + ((play_y - mon->y + 1)^2)) < start_euc) {
				dungeon->map[mon->y + 1][mon->x - 1] = ter_debug;
				mon->y = mon->y + 1;
				mon->x = mon->x - 1;
			}
		}
		if(dungeon->map[mon->y + 1][mon->x + 1] == ter_room || dungeon->map[mon->y + 1][mon->x + 1] == ter_corridor) {
			if(sqrt(((play_x - (mon->x + 1))^2) + ((play_y - mon->y + 1)^2)) < start_euc) {
				dungeon->map[mon->y + 1][mon->x + 1] = ter_debug;
				mon->y = mon->y + 1;
				mon->x = mon->x + 1;
			}
		}
		if(dungeon->map[mon->y - 1][mon->x + 1] == ter_room || dungeon->map[mon->y - 1][mon->x + 1] == ter_corridor) {
			if(sqrt(((play_x - (mon->x + 1))^2) + ((play_y - mon->y - 1)^2)) < start_euc) {
				dungeon->map[mon->y - 1][mon->x + 1] = ter_debug;
				mon->y = mon->y - 1;
				mon->x = mon->x + 1;
			}
		}
		if(dungeon->map[mon->y - 1][mon->x - 1] == ter_room || dungeon->map[mon->y - 1][mon->x - 1] == ter_corridor) {
			if(sqrt(((play_x - (mon->x - 1))^2) + ((play_y - mon->y - 1)^2)) < start_euc) {
				dungeon->map[mon->y - 1][mon->x - 1] = ter_debug;
				mon->y = mon->y - 1;
				mon->x = mon->x - 1;
			}
		}
	}

	return 1;

}