#include "config.h"

#ifndef MON_H
#define MON_H

int init_mons(dungeon_t *dungeon);
int can_see(dungeon_t *dungeon, character_t *mon);
int move_mon(dungeon_t *dungeon, character_t *mon);

#endif