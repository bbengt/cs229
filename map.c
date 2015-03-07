#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <math.h>

#include "config.h"
#include "map.h"
#include "mon.h"
#include "player.h"

/*
*
* valid_room(): determines if room r is valid or not.  Returns 0 if true, 1 if false.
*
*/

int valid_room(dungeon_t *dungeon, room_t *r) {

	if(r->x < 1 || (r->x + r->x_width) < 1 || r->y < 1 || (r->y + r->y_height) < 1 || r->x > DUNG_X - 2 || (r->x + r->x_width) > DUNG_X - 2 || r->y > DUNG_Y - 2 || (r->y + r->y_height) > DUNG_Y - 2) {

		// goes past valid boundaries
		return 0;
	}
	if(abs(r->x - (r->x + r->x_width)) < MIN_ROOM_X || abs(r->y - (r->y + r->y_height)) < MIN_ROOM_Y) {
		// not big enough
		return 0;
	}

	int x1_rad, x2_rad, y1_rad, y2_rad;
	x1_rad = r->x - MIN_ROOM_DIST;
	x2_rad = (r->x + r->x_width) + MIN_ROOM_DIST;
	y1_rad = r->y - MIN_ROOM_DIST;
	y2_rad = (r->y + r->y_height) + MIN_ROOM_DIST;

	if(x1_rad < 1) {
		x1_rad = 1;
	}
	if(x1_rad > DUNG_X) {
		x1_rad = DUNG_X;
	}
	if(x2_rad < 1) {
		x2_rad = 1;
	}
	if(x2_rad > DUNG_X) {
		x2_rad = DUNG_X;
	}
	if(y1_rad < 1) {
		y1_rad = 1;
	}
	if(y1_rad > DUNG_Y) {
		y1_rad = DUNG_Y;
	}
	if(y2_rad < 1) {
		y2_rad = 1;
	}
	if(y2_rad > DUNG_Y) {
		y2_rad = DUNG_Y;
	}

	int i, j;
	for(i = x1_rad; i < x2_rad; i++) {
		for(j = y1_rad; j < y2_rad; j++) {
			if(dungeon->map[j][i] == ter_room) {

				// too close to another room
				return 0;
			}
		}
	}

	return 1;

}

/* 
*
*	generate_room(): Generates a valid room and updates the game map
*
*/

int generate_rooms(dungeon_t *dungeon, int cur_rooms) {

	int x1, x_width, y1, y_height, x2, y2;
	x1 = rand() % DUNG_X;
	x_width = MIN_ROOM_X + rand() % MIN_ROOM_X;
	y1 = rand() % DUNG_Y;
	y_height = MIN_ROOM_Y + rand() % MIN_ROOM_Y;
	x2 = x1 + x_width;
	y2 = y1 + y_height;

	room_t r = { x1, x_width, y1, y_height };

	if(valid_room(dungeon, &r)) {
		int i, j;
		for(i = x1; i < x2; i++) {
			for(j = y1; j < y2; j++) {
				dungeon->map[j][i] = ter_room;
				dungeon->hardness[j][i] = 0;
			}
		}

		dungeon->rooms[cur_rooms] = r;

	} else {

		generate_rooms(dungeon, cur_rooms);

	}

	return 1;

}

/*
*
* connect_rooms_dij(): connects room r1 to room r2 using Dijkstra's shortest path algorithm
*
*/

// path_t *connect_rooms_dij(dungeon_t *dungeon, room_t *r1, room_t *r2) { // this method is horrifically slow when running.  If I have time in the future, try to improve this efficiency

// 	int start_x = r1->x;
// 	int start_y = r1->y + (rand() % r1->y_height);
// 	int end_x = r2->x;
// 	int end_y = r2->y + (rand() % r2->y_height);

// 	dungeon->map[end_y][end_x] = ter_debug;

// 	queue_t *q = malloc(sizeof(q));

// 	// create a path representation of each cell on the map - set each cost to "infinite" unless it's the source node (which has cost 0)
// 	int x, y;
// 	for(y = 0; y < DUNG_Y; y++) {
// 		for(x = 0; x < DUNG_X; x++) {
// 			path_t *v = malloc(sizeof(v));
// 			v->x = x;
// 			v->y = y;
// 			v->cost = INT_MAX;
// 			v->prev = NULL;
// 			if(x == start_x && y == start_y) { // source node
// 				v->cost = 0;
// 			}
// 			add_element(q, v);
// 		}
// 	}

// 	// loop through the queue, removing the node with the smallest cost each time
// 	while(q->num_elements > 0) {
// 		path_t *u = remove_min(q);
// 		u->scanned = 1;

// 		if(u->x == end_x && u->y == end_y) {
// 			return u;
// 		}
// 		int x, y;
// 		x = u->x;
// 		y = u->y;
// 		int i, j;

// 		// look at each neighbor of u.  If we haven't already scanned it, calculate its cost. 
// 		for(j = y - 1; j < y + 2; j++) {

// 			// make sure we don't run off the edge of the map
// 			for(i = x - 1; i < x + 2; i++) {
// 				if(i < 0) {
// 					i = 0;
// 				}
// 				if(i > DUNG_X - 1) {
// 					i = DUNG_X - 1;
// 				}
// 				if(j < 0) {
// 					j = 0;
// 				}
// 				if(j > DUNG_Y - 1) {
// 					j = DUNG_Y - 1;
// 				}

// 				// skip diagonals
// 				if(j == y - 1 && i == x - 1) {
// 					continue;
// 				}
// 				if(j == y - 1 && i == x + 1) {
// 					continue;
// 				}
// 				if(j == y + 1 && i == x - 1) {
// 					continue;
// 				}
// 				if(j == y + 1 && i == x + 1) {
// 					continue;
// 				}

// 				path_t *v = fetch_at_pos(q, i, j);
// 				// if we haven't already scanned v, make sure it's an open space before updating the cost
// 				if(v->scanned == 0) {
// 					if(dungeon->map[j][i] != ter_immutable) {
// 						int tmp = u->cost + dungeon->hardness[j][i];
// 						if(tmp < v->cost) {
// 							v->cost = tmp;
// 							v->prev = u;
// 						}
// 					}
// 				}
// 			}
// 		}
// 	}

// 	fprintf(stderr, "No Dijkstra's path found!\n");
// 	exit(1);

// }

/*
*
* connect_rooms_sim(): connects room r1 to room r2 by drawing straight lines between them
*
*/

int connect_rooms_sim(dungeon_t *dungeon, room_t *r1, room_t *r2) {

	int r1midx = ((r1->x + r1->x_width) + r1->x) / 2;
	int r1midy = ((r1->y + r1->y_height) + r1->y) / 2;
	int r2midx = ((r2->x + r2->x_width) + r2->x) / 2;
	int r2midy = ((r2->y + r2->y_height) + r2->y) / 2;

	// figure out which direction r2 is from r1
	int i, j;
	i = r1midx;

	if(r1midy < r2midy) { // r1 is north of r2
	
		for(j = r1midy; j < r2midy; j++) {
			dungeon->map[j][r1midx] = ter_corridor;
			dungeon->hardness[j][r1midx] = 0;
		}

	} else { // r1 is south of r2

		for(j = r1midy; j > r2midy; j--) {
			dungeon->map[j][r1midx] = ter_corridor;
			dungeon->hardness[j][r1midx] = 0;
		}

	}

	if(r1midx < r2midx) { // r1 is north of r2

		for(i = r1midx; i < r2midx; i++) {
			dungeon->map[j][i] = ter_corridor;
			dungeon->hardness[j][i] = 0;
		}

	} else { // r1 is south of r2

		for(i = r1midx; i > r2midx; i--) {
			dungeon->map[j][i] = ter_corridor;
			dungeon->hardness[j][i] = 0;
		}

	}

	return 1;

}

/*
*
* connect_rooms(): connects all the rooms in dungeon
*
*/

int connect_rooms(dungeon_t *dungeon) {

	int i, j;
	for(i = 0; i < dungeon->num_rooms; i++) {
		room_t *r1 = &dungeon->rooms[i];

		int r1midx = ((r1->x + r1->x_width) + r1->x) / 2;
		int r1midy = ((r1->y + r1->y_height) + r1->y) / 2;

		int min_dist = INT_MAX;
		room_t *r2;
		room_t *tmp;
		for(j = 0; j < dungeon->num_rooms; j++) {
			tmp = &dungeon->rooms[j];
			if(i == j) { // make sure we don't connect a room to itself
				continue;
			}

			int r2midx = ((tmp->x + tmp->x_width) + tmp->x) / 2;
			int r2midy = ((tmp->y + tmp->y_height) + tmp->y) / 2;

			int dist = sqrt(((r1midx - r2midx)^2) + ((r1midy - r2midy)^2));
			if(dist < min_dist) {
				r2 = &dungeon->rooms[j];
				min_dist = dist;
			} 

		}

		connect_rooms_sim(dungeon, r1, r2);

	}


	return 1;

}

/*
*
* generate_map(): Generates a game map with rooms and corridors.  Initializes everything to wall (or immutable if on the outside border), generates rooms, then connects them.
*
*/

int generate_map(dungeon_t *dungeon) {

	dungeon->num_rooms = (rand() % MIN_ROOMS) + MIN_ROOMS;

	// fill map with random hardness & wall material
	int x, y;
	for(y = 0; y < DUNG_Y; y++) {
		for(x = 0; x < DUNG_X; x++) {
			dungeon->map[y][x] = ter_wall;
			dungeon->hardness[y][x] = 1 + rand() % 254;
			if(x == DUNG_X - 1 || x == 0 || y == DUNG_Y - 1 || y == 0) {
				dungeon->map[y][x] = ter_immutable;
				dungeon->hardness[y][x] = 255;
			}
		}
	}

	int cur_rooms;
	for(cur_rooms = 0; cur_rooms < dungeon->num_rooms; cur_rooms++) {

		generate_rooms(dungeon, cur_rooms);

	}

	connect_rooms(dungeon);

	for(;;) {

		x = rand() % DUNG_X;
		y = rand() % DUNG_Y;
		if(dungeon->map[y][x] == ter_room) {
			break;
		}

	}

	dungeon->map[y][x] = ter_stair_up;

	for(;;) {

		x = rand() % DUNG_X;
		y = rand() % DUNG_Y;

		if(dungeon->map[y][x] == ter_room) {
			break;
		}
	}

	dungeon->map[y][x] = ter_stair_down;

	init_player(dungeon);
	init_mons(dungeon);

	return 0;

}

/*
*
* print_map(): prints out the game map, including monsters and player character
*
*/

int print_map(dungeon_t *dungeon) {

	char tmp_map[DUNG_Y][DUNG_X];

	int x, y;
	for(y = 0; y < DUNG_Y; y++) {
		for(x = 0; x < DUNG_X; x++) {
			
			switch(dungeon->map[y][x]) {
				case ter_wall:
				case ter_immutable:
					tmp_map[y][x] = '#';
					break;
				case ter_corridor:
				case ter_room:
					tmp_map[y][x] = '.';
					break;
				case ter_debug:
					tmp_map[y][x] = '*';
					break;
			}
			
		}
	}
	int i;
	for(i = 0; i < dungeon->num_mons; i++) {
		tmp_map[dungeon->mons[i].y][dungeon->mons[i].x] = dungeon->mons[i].type;
	}

	tmp_map[dungeon->player.y][dungeon->player.x] = PLAYER_CHAR;

	for(y = 0; y < DUNG_Y; y++) {
		for(x = 0; x < DUNG_X; x++) {
			putchar(tmp_map[y][x]);
		}
		printf("\n");
	}

	return 0;

}

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