#define DUNG_X 160 		// dungeon measurement in the x direction
#define DUNG_Y 96		// dungeon measurement in the y direction
#define MIN_ROOM_DIST 3	// minimum distance between rooms
#define MIN_ROOM_X 8 	// minimum measurement of each room in the x direction
#define MIN_ROOM_Y 5 	// minimum measurement of each room in the y direction
#define MIN_ROOMS 12 	// minimum number of rooms to generate
#define DRAW_X 80
#define DRAW_Y 24

#define PLAYER_CHAR '@'	// character to represent a player on the game map

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef CONFIG_H
#define CONFIG_H

typedef enum terrain {
	ter_wall,
	ter_room,
	ter_corridor,
	ter_immutable,
	ter_debug,
	ter_stair_up,
	ter_stair_down,
} terrain_t;

typedef struct room {
	uint8_t x;
	uint8_t x_width;
	uint8_t y;
	uint8_t y_height;
} room_t;

typedef struct mon {
	uint8_t telepathic;
	uint8_t smart;
	uint32_t last_saw_x;
	uint32_t last_saw_y;
} mon_t;

typedef struct player {
	uint8_t x;
	uint8_t y;
	uint8_t speed;
} player_t;

typedef struct character {
	uint8_t x;
	uint8_t y;
	uint8_t speed;
	char type;
	mon_t *m;
	player_t *p;
	uint32_t next_turn;
	uint8_t alive;
} character_t;

typedef struct cell {
	uint32_t x, y;
	struct cell *prev;
	uint32_t cost;
	uint8_t scanned;
} cell_t;

typedef struct dungeon {
	uint8_t map[DUNG_Y][DUNG_X];
	uint8_t hardness[DUNG_Y][DUNG_X];
	uint32_t num_rooms;
	uint32_t num_mons;
	character_t mons[DUNG_Y * DUNG_X];
	room_t rooms[MIN_ROOMS * 2];
	character_t player;
	uint8_t look_mode;
	uint32_t look_mode_x, look_mode_y; 
	uint32_t distance[DUNG_Y][DUNG_X];
} dungeon_t;

#endif