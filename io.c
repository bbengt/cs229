#include <stdio.h>
#include <stdint.h>

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

	fwrite("RLG229", 1, strlen("RLG229"), f);

	uint32_t version = htobe32(0);
	fwrite(&version, sizeof(version), 1, f);

	int x, y;
	uint8_t zero = 0;
	uint8_t non_zero = 1;
	for(y = 0; y < DUNG_Y; y++) {
		for(x = 0; x < DUNG_X; x++) {
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
		printf("Invalid save file.\n");
		exit(1);
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

	return 0;

}