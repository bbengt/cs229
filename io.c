#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "io.h"

/*
*
* save(): Saves the game state to $HOME/.rlg229/dungeon
*
*/

int save(dungeon_t *dungeon) {

	// create save file
	char *home = getenv("HOME");
	char *filename;
	FILE *f;

	size_t length = (strlen(home) + strlen(".rlg229") + strlen("dungeon") + 2 + 1);

	filename = malloc(length * sizeof(*filename));
	sprintf(filename, "%s/.rlg229/", home);
	mkdir(filename, 0755);

	strcat(filename, "dungeon");

	if(!(f = fopen(filename, "w"))) {
		fprintf(stderr, "Could not open file\n");
		return 1;
	}

	// write filetype & version number
	fwrite("RLG229", 1, strlen("RLG229"), f);

	uint32_t version = htobe32(1);
	fwrite(&version, sizeof(version), 1, f);


	// ADD IN USER BLOCK BYTES

	// write map data
	int x, y;
	uint8_t zero = 0;
	uint8_t non_zero = 1;
	uint8_t two = 2;
	for(y = 0; y < DUNG_Y; y++) {
		for(x = 0; x < DUNG_X; x++) {

			// Open?
			switch(dungeon->map[y][x]) {
				case ter_wall:
				case ter_immutable:
					fwrite(&zero, sizeof(zero), 1, f);
					break;
				case ter_room:
				case ter_corridor:
					fwrite(&non_zero, sizeof(non_zero), 1, f);
					break;
				case ter_debug:
					fwrite(&non_zero, sizeof(non_zero), 1, f);
					break;
			}

			// Part of room?
			switch(dungeon->map[y][x]) {
				case ter_wall:
				case ter_immutable:
				case ter_corridor:
					fwrite(&zero, sizeof(zero), 1, f);
					break;
				case ter_room:
					fwrite(&non_zero, sizeof(non_zero), 1, f);
					break;
				case ter_debug:
					fwrite(&non_zero, sizeof(non_zero), 1, f);
					break;
			}

			// Part of corridor?
			switch(dungeon->map[y][x]) {
				case ter_wall:
				case ter_immutable:
				case ter_room:
					fwrite(&zero, sizeof(zero), 1, f);
					break;
				case ter_corridor:
					fwrite(&non_zero, sizeof(non_zero), 1, f);
					break;
				case ter_debug:
					fwrite(&non_zero, sizeof(non_zero), 1, f);
					break;
			}

			fwrite(&dungeon->hardness[y][x], sizeof(dungeon->hardness[y][x]), 1, f);

			// Stairs? (1 if up, 2 if down)
			switch(dungeon->map[y][x]) {
				case ter_stair_up:
					fwrite(&two, sizeof(two), 1, f);
					break;
				case ter_stair_down:
					fwrite(&non_zero, sizeof(non_zero), 1, f);
					break;
			}
		}
	}

	uint16_t room_count = htobe16(dungeon->num_rooms);
	fwrite(&room_count, sizeof(room_count), 1, f);

	int i;
	for(i = 0; i < dungeon->num_rooms; i++) {
		fwrite(&dungeon->rooms[i].x, 1, 1, f);
		fwrite(&dungeon->rooms[i].y, 1, 1, f);
		fwrite(&dungeon->rooms[i].x_width, 1, 1, f);
		fwrite(&dungeon->rooms[i].y_height, 1, 1, f);
	}

	uint8_t play_x = dungeon->player.x;
	uint8_t play_y = dungeon->player.y;

	fwrite(&play_x, sizeof(play_x), 1, f);
	fwrite(&play_y, sizeof(play_y), 1, f);

	// GAME TURN

	// MONSTER GENERATION SEQUENCE NUMBER

	for(i = 0; i < dungeon->num_mons; i++) {

		char type = dungeon->mons[i].type;
		uint8_t mon_x = dungeon->mons[i].x;
		uint8_t mon_y = dungeon->mons[i].y;
		uint8_t mon_speed = dungeon->mons[i].speed;
		uint8_t smart = dungeon->mons[i].m->smart;
		uint8_t telepathic = dungeon->mons[i].m->telepathic;
		uint8_t last_saw_x = dungeon->mons[i].m->last_saw_x;
		uint8_t last_saw_y = dungeon->mons[i].m->last_saw_y;
		// sequence number
		uint32_t mon_next_turn = dungeon->mons[i].next_turn;
		uint32_t empty = 0;

		fwrite(&mon_x, sizeof(mon_x), 1, f);
		fwrite(&mon_y, sizeof(mon_y), 1, f);
		fwrite(&mon_speed, sizeof(mon_speed), 1, f);
		fwrite(&smart, sizeof(smart), 1, f);
		fwrite(&telepathic, sizeof(smart), 1, f);
		fwrite(&last_saw_x, sizeof(last_saw_x), 1, f);
		fwrite(&last_saw_y, sizeof(last_saw_y), 1, f);
		// sequence number
		fwrite(&mon_next_turn, sizeof(mon_next_turn), 1, f);

		// write 20 bytes of zeros
		fwrite(&empty, sizeof(empty), 5, f);

	}	

	fclose(f);

	return 0;

}

/*
*
* load(): loads a game previously saved by the save() function
*
*/

int load(dungeon_t *dungeon) {

	char id[6];
	char *home = getenv("HOME");
	char *filename;
	FILE *f;

	size_t length = (strlen(home) + strlen(".rlg229") + strlen("dungeon") + 2 + 1);

	filename = malloc(length * sizeof(*filename));
	sprintf(filename, "%s/.rlg229/dungeon", home);

	f = fopen(filename, "r");

	// read filetype identifier
	fread(id, sizeof(id), 1, f);
	if(strncmp(id, "RLG229", 6)) {
		return 0;
	}
	uint32_t i32;
	uint16_t i16;

	// read version number
	fread(&i32, sizeof(i32), 1, f);

	// read in remaining file size
	fread(&i32, sizeof(i32), 1, f);

	// read in map
	uint8_t open, room, corridor;
	int x, y;
	for(y = 0; y < DUNG_Y; y++) {
		for(x = 0; x < DUNG_X; x++) {
			fread(&open, sizeof(open), 1, f);
			fread(&room, sizeof(room), 1, f);
			fread(&corridor, sizeof(corridor), 1, f);
			fread(&dungeon->hardness[y][x], sizeof(dungeon->hardness[y][x]), 1, f);

			if(room) {
				dungeon->map[y][x] = ter_room;
			} else if(corridor) {
				dungeon->map[y][x] = ter_corridor;
			} else if(y == 0 || y == DUNG_Y - 1 || x == 0 || x == DUNG_X - 1) {
				dungeon->map[y][x] = ter_immutable;
			} else {
				dungeon->map[y][x] = ter_wall;
			}
		}
	}

	// read in number of rooms
	fread(&i16, sizeof(i16), 1, f);
	dungeon->num_rooms = be16toh(i16);

	// read in rooms
	int i;
	for(i = 0; i < dungeon->num_rooms; i++) {
		fread(&dungeon->rooms[i].x, 1, 1, f);
		fread(&dungeon->rooms[i].y, 1, 1, f);
		fread(&dungeon->rooms[i].x_width, 1, 1, f);
		fread(&dungeon->rooms[i].y_height, 1, 1, f);
	}

	fclose(f);

	return 1;

}

int save_file_exists() {

	char *home = getenv("HOME");
	char *filename;

	size_t length = (strlen(home) + strlen(".rlg229") + strlen("dungeon") + 2 + 1);

	filename = malloc(length * sizeof(*filename));
	sprintf(filename, "%s/.rlg229/", home);
	strcat(filename, "dungeon");

	if(access(filename, R_OK) == 0) {
		free(filename);
		return 1;
	}
	free(filename);
	return 0;
}