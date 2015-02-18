#include "binary_search.c"

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

int dijkstra(dungeon_t *dungeon, int start_x, int start_y, int end_x, int end_y) {

	queue_t *q = malloc(sizeof(queue_t));
	q->num_elements = 0;

	// create a path representation of each cell on the map - set each cost to "infinite" unless it's the source node (which has cost 0)
	uint32_t x, y;
	for(y = 0; y < DUNG_Y; y++) {
		for(x = 0; x < DUNG_X; x++) {
			int cost = INT_MAX;
			if(x == start_x && y == start_y) { // source node
				cost = 0;
			}
			add_element(q, x, y, cost);
		}
	}

	path_t *u = malloc(sizeof(u));

	// loop through the queue, removing the node with the smallest cost each time
	while(q->num_elements > 0) {
		u = remove_min(q);
		u->scanned = 1;
		if(u->x == end_x && u->y == end_y) {
			break;
		}

		uint32_t x, y;
		x = u->x;
		y = u->y;
		uint32_t i, j;

		// look at each neighbor of u.  If we haven't already scanned it, calculate its cost. 
		for(j = y - 1; j < y + 2; j++) {

			for(i = x - 1; i < x + 2; i++) {

				// make sure we don't run off the edge of the map
				if(i < 0) {
					i = 0;
				}
				if(i > DUNG_X - 1) {
					i = DUNG_X - 1;
				}
				if(j < 0) {
					j = 0;
				}
				if(j > DUNG_Y - 1) {
					j = DUNG_Y - 1;
				}

				path_t *v = fetch_at_pos(q, i, j);

				// if we haven't already scanned v, make sure it's an open space before updating the cost (u's cost + 1) 
				if(v->scanned == 0) {
					if(dungeon->map[j][i] == ter_corridor || dungeon->map[j][i] == ter_room) {
						int tmp = u->cost + 1;
						if(tmp < v->cost) {
							v->cost = tmp;
							v->prev = u;
						}
					}
				}
			}
		}
	}

	print_dij(dungeon, u);
	free(q);

	return 1;

}

/*
*
* init_mobs(): initialize monsters & place on dungeon map
*
*/

int init_mobs(dungeon_t *dungeon) {

	char mob_types[] = { 'p', 'P', 'D', 'g' };
	int i;
	for(i = 0; i < dungeon->num_mobs; i++) {

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

		mob_t mob = { x, y, rand() % 2, rand() % 2, 5 + rand() % 16, mob_types[rand() % 4] };
		dungeon->mobs[i] = mob;

	}

	return 0;

}

/*
*
* move_mob(): moves a monster mob
*
*/

int move_mob(dungeon_t *dungeon, mob_t *mob) {

	int play_x, play_y;
	if(!mob->telepathic) {
		play_x = mob->last_saw_x;
		play_y = mob->last_saw_y;
	} else {
		play_x = dungeon->player.x;
		play_y = dungeon->player.y;
	}

	if(mob->smart) {
		dijkstra(dungeon, mob->x, mob->y, play_x, play_y);
		// path_t tmp = *node;
		// path_t *last = node->prev;

		// while(node->prev != NULL) {
		// 	last = node;
		// 	node = node->prev;
		// }

		// // printf("moving from (%d, %d) to (%d, %d)\n", mob->x, mob->y, last->x, last->y);
		// mob->x = last->x;
		// mob->y = last->y;

	} else {

	}

	return 1;

}