#include "config.h"

#ifndef RUNGAME_H
#define RUNGAME_H

int32_t compare_characters_by_next_turn(const void *character1, const void *character2);
int32_t compare_distance_from_player(const void *cell1, const void *cell2);
void dijkstra(dungeon_t *dungeon);
int run_game(dungeon_t *dungeon);

#endif