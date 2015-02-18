#include "config.h"

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

	dungeon->player.x = x;
	dungeon->player.y = y;
	dungeon->player.speed = 10;
	
	return 1;
	
}