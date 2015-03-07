#include "config.h"

#ifndef SCREEN_H
#define SCREEN_H

void init_term(void);
int update_term(dungeon_t *dungeon, int x, int y);

#endif