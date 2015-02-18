#define DUNG_X 160 		// dungeon measurement in the x direction
#define DUNG_Y 96		// dungeon measurement in the y direction
#define MIN_ROOM_DIST 3	// minimum distance between rooms
#define MIN_ROOM_X 8 	// minimum measurement of each room in the x direction
#define MIN_ROOM_Y 5 	// minimum measurement of each room in the y direction
#define MIN_ROOMS 12 	// minimum number of rooms to generate

#define PLAYER_CHAR '@'	// character to represent a player on the game map

#include <stdio.h>

#ifndef CONFIG_H
#define CONFIG_H

typedef enum terrain {
	ter_wall,
	ter_room,
	ter_corridor,
	ter_immutable,
	ter_debug,
} terrain_t;

typedef struct room {
	uint32_t x;
	uint32_t x_width;
	uint32_t y;
	uint32_t y_height;
} room_t;

typedef struct mob {
	uint32_t x;
	uint32_t y;
	uint8_t telepathic;
	uint8_t smart;
	uint8_t speed;
	char type;

	uint32_t last_saw_x;
	uint32_t last_saw_y;
} mob_t;

typedef struct player {
	uint32_t x;
	uint32_t y;
	uint8_t speed;
} player_t;

typedef struct dungeon {
	uint8_t map[DUNG_Y][DUNG_X];
	uint8_t hardness[DUNG_Y][DUNG_X];
	uint32_t num_rooms;
	uint32_t num_mobs;
	mob_t mobs[DUNG_Y * DUNG_X];
	room_t rooms[MIN_ROOMS * 2];
	player_t player;
} dungeon_t;

#endif