#include <ncurses.h>
#include <unistd.h>
#include <limits.h>

#include "config.h"
#include "rungame.h"
#include "screen.h"
#include "heap.h"
#include "mon.h"
#include "player.h"

int32_t compare_characters_by_next_turn(const void *character1,
                                        const void *character2)
{
  return (((character_t *) character1)->next_turn -
          ((character_t *) character2)->next_turn);
}

int32_t compare_distance_from_player(const void *cell1, const void *cell2) {

	return (((cell_t *) cell1)->cost - ((cell_t *) cell2)->cost);
}

typedef struct path {
  heap_node_t *hn;
  uint8_t x, y;
} path_t;

/*
*
* dijkstra(): assigns a cost to each cell in the map based on distance from the player
*
*/
static dungeon_t *dungeon;

static int32_t dist_cmp(const void *key, const void *with) {
  return ((int32_t) dungeon->distance[((path_t *) key)->y]
                                        [((path_t *) key)->x] -
          (int32_t) dungeon->distance[((path_t *) with)->y]
                                        [((path_t *) with)->x]);
}

void dijkstra(dungeon_t *d)
{
  /* Currently assumes that monsters only move on floors.  Will *
   * need to be modified for tunneling and pass-wall monsters.  */

  heap_t h;
  uint32_t x, y;
  static path_t p[DUNG_Y][DUNG_X], *c;
  static uint32_t initialized = 0;

  if (!initialized) {
    initialized = 1;
    dungeon = d;
    for (y = 0; y < DUNG_Y; y++) {
      for (x = 0; x < DUNG_X; x++) {
        p[y][x].y = y;
        p[y][x].x = x;
      }
    }
  }

  for (y = 0; y < DUNG_Y; y++) {
    for (x = 0; x < DUNG_X; x++) {
      d->distance[y][x] = 255;
    }
  }
  d->distance[d->player.y][d->player.x] = 0;

  heap_init(&h, dist_cmp, NULL);

  for (y = 0; y < DUNG_Y; y++) {
    for (x = 0; x < DUNG_X; x++) {
      if (dungeon->map[y][x] == ter_room || dungeon->map[y][x] == ter_corridor) {
        p[y][x].hn = heap_insert(&h, &p[y][x]);
      }
    }
  }

  while ((c = heap_remove_min(&h))) {
    c->hn = NULL;
    if ((p[c->y - 1][c->x - 1].hn) &&
        (d->distance[c->y - 1][c->x - 1] >
         d->distance[c->y][c->x] + 1)) {
      d->distance[c->y - 1][c->x - 1] =
        d->distance[c->y][c->x] + 1;
      heap_decrease_key_no_replace(&h,
                                   p[c->y - 1][c->x - 1].hn);
    }
    if ((p[c->y - 1][c->x    ].hn) &&
        (d->distance[c->y - 1][c->x    ] >
         d->distance[c->y][c->x] + 1)) {
      d->distance[c->y - 1][c->x    ] =
        d->distance[c->y][c->x] + 1;
      heap_decrease_key_no_replace(&h,
                                   p[c->y - 1][c->x    ].hn);
    }
    if ((p[c->y - 1][c->x + 1].hn) &&
        (d->distance[c->y - 1][c->x + 1] >
         d->distance[c->y][c->x] + 1)) {
      d->distance[c->y - 1][c->x + 1] =
        d->distance[c->y][c->x] + 1;
      heap_decrease_key_no_replace(&h,
                                   p[c->y - 1][c->x + 1].hn);
    }
    if ((p[c->y    ][c->x - 1].hn) &&
        (d->distance[c->y    ][c->x - 1] >
         d->distance[c->y][c->x] + 1)) {
      d->distance[c->y    ][c->x - 1] =
        d->distance[c->y][c->x] + 1;
      heap_decrease_key_no_replace(&h,
                                   p[c->y    ][c->x - 1].hn);
    }
    if ((p[c->y    ][c->x + 1].hn) &&
        (d->distance[c->y    ][c->x + 1] >
         d->distance[c->y][c->x] + 1)) {
      d->distance[c->y    ][c->x + 1] =
        d->distance[c->y][c->x] + 1;
      heap_decrease_key_no_replace(&h,
                                   p[c->y    ][c->x + 1].hn);
    }
    if ((p[c->y + 1][c->x - 1].hn) &&
        (d->distance[c->y + 1][c->x - 1] >
         d->distance[c->y][c->x] + 1)) {
      d->distance[c->y + 1][c->x - 1] =
        d->distance[c->y][c->x] + 1;
      heap_decrease_key_no_replace(&h,
                                   p[c->y + 1][c->x - 1].hn);
    }
    if ((p[c->y + 1][c->x    ].hn) &&
        (d->distance[c->y + 1][c->x    ] >
         d->distance[c->y][c->x] + 1)) {
      d->distance[c->y + 1][c->x    ] =
        d->distance[c->y][c->x] + 1;
      heap_decrease_key_no_replace(&h,
                                   p[c->y + 1][c->x    ].hn);
    }
    if ((p[c->y + 1][c->x + 1].hn) &&
        (d->distance[c->y + 1][c->x + 1] >
         d->distance[c->y][c->x] + 1)) {
      d->distance[c->y + 1][c->x + 1] =
        d->distance[c->y][c->x] + 1;
      heap_decrease_key_no_replace(&h,
                                   p[c->y + 1][c->x + 1].hn);
    }
  }
  heap_delete(&h);
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
	   
      // delete save file

			endwin();
			exit(0);
		}

		update_term(dungeon, dungeon->player.x, dungeon->player.y);
		
	}
	
	return 0;
}
