#include "config.h"

#ifndef MAP_H
#define MAP_H

int valid_room(dungeon_t *dungeon, room_t *r);
int generate_rooms(dungeon_t *dungeon, int cur_rooms);
int connect_rooms_sim(dungeon_t *dungeon, room_t *r1, room_t *r2);
int connect_rooms(dungeon_t *dungeon);
int generate_map(dungeon_t *dungeon);
int print_map(dungeon_t *dungeon);

#endif