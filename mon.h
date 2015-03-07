#include "config.h"
#include "binary_search.h"

#ifndef MON_H
#define MON_H

void print_dij(dungeon_t *dungeon, path_t *node);
int init_mons(dungeon_t *dungeon);
int can_see(dungeon_t *dungeon, character_t *mon);
int move_mon(dungeon_t *dungeon, character_t *mon);

#endif