#include <ncurses.h>
#include <unistd.h>
#include <limits.h>

#include "config.h"
#include "rungame.h"
#include "screen.h"
#include "heap.h"
#include "mon.h"
#include "player.h"
// #include "heap.c"

int32_t compare_characters_by_next_turn(const void *character1,
                                        const void *character2)
{
  return (((character_t *) character1)->next_turn -
          ((character_t *) character2)->next_turn);
}

int32_t compare_distance_from_player(const void *cell1, const void *cell2) {

	return (((cell_t *) cell1)->cost - ((cell_t *) cell2)->cost);
}

/*
*
* dijkstra(): assigns a cost to each cell in the map based on distance from the player
*
*/
int dijkstra(dungeon_t *dungeon) {

	heap_t h;
	heap_init(&h, compare_distance_from_player, NULL);

	// create a path representation of each cell on the map - set each cost to "infinite" unless it's the source node (which has cost 0)
	uint32_t x, y;
	for(y = 0; y < DUNG_Y; y++) {
		for(x = 0; x < DUNG_X; x++) {
			int cost = INT_MAX;
			if(x == dungeon->player.x && y == dungeon->player.y) { // source node
				cost = 0;
			}
			cell_t c = { x, y, NULL, cost, 0 };
			heap_insert(&h, &c);
		}
	}

	// loop through the queue, removing the node with the smallest cost each time
	while(h.size > 0) {
		cell_t *u = heap_remove_min(&h);
		u->scanned = 1;

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

				// find node at (i, j)
				heap_node_t *hn = h.min;
				cell_t v_tmp;
				while(hn != NULL) {
					cell_t *tmp_c = hn->datum;
					if(tmp_c->x == i && tmp_c->y == j) {
						v_tmp = *tmp_c;
						break;
					}
					hn = hn->next;
				}

				cell_t *v = &v_tmp;

				// if we haven't already scanned v, make sure it's an open space before updating the cost (u's cost + 1) 
				if(v->scanned == 0) {
					if(dungeon->map[j][i] == ter_corridor || dungeon->map[j][i] == ter_room) {
						int tmp = u->cost + 1;
						if(tmp < v->cost) {
							v->cost = tmp;
							v->prev = u;
							dungeon->distance[v->y][v->x] = v->cost;
						}
					}
				}
			}
		}
	}

	return 1;

}

int run_game(dungeon_t *dungeon) {
	dungeon->look_mode = 0;
	dungeon->look_mode_x = -1;
	dungeon->look_mode_y = -1;

	init_term();
	update_term(dungeon, dungeon->player.x, dungeon->player.y);

	heap_t h;
	heap_init(&h, compare_characters_by_next_turn, NULL);
	heap_insert(&h, &dungeon->player);

	int i;
	for(i = 0; i < dungeon->num_mons; i++) {
		heap_insert(&h, &dungeon->mons[i]);
	}

	character_t *c;
	while((c = heap_remove_min(&h))) {

		if(c == NULL) {
			fprintf(stderr, "Invalid character has the next turn.\n");
			exit(1);
		}

		/* character is a monster */
		if(c->p == NULL) {
			usleep(3000);
			move_mon(dungeon, c);

		/* character is the player */
		} else if(c->m == NULL) {
			
			move_player(dungeon);
			
		} else {
			fprintf(stderr, "Something went wrong with the game.\n");
			exit(1);
		}

		int new_prio = c->next_turn + (100 / c->speed);
		c->next_turn = new_prio;
		heap_insert(&h, c);

		/* check to see if any characters are on the cell we just moved to */
		int i;
		for(i = 0; i < dungeon->num_mons; i++) {
			if(c == &dungeon->mons[i]) {
				continue;
			}
			if(c->x == dungeon->mons[i].x && c->y == dungeon->mons[i].y) {
				c->alive = 0;
			}
		}

		if(c->p == NULL && c->x == dungeon->player.x && c->y == dungeon->player.y) {
			printf("Player died.\n");
			endwin();
			exit(0);
		}

		update_term(dungeon, dungeon->player.x, dungeon->player.y);
		
	}
	
	return 0;
}
