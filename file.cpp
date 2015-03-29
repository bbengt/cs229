#include <iostream>
using std::cout;
using std::endl;

#include <fstream>
using std::ifstream;

#include <string>
using std::string;

#include "file.h"
#include "utils.h"

int parse_monster_defs(dungeon_t *d) {

	char *home;
  	char *filename;
  	FILE *f;
  	size_t len;

	if (!(home = getenv("HOME"))) {
		fprintf(stderr, "\"HOME\" is undefined.  Using working directory.\n");
	    home = ".";
	}

	len = (strlen(home) + strlen(SAVE_DIR) + strlen("monster_desc.txt") +
		1 /* The NULL terminator */                                 +
	    2 /* The slashes */);

	filename = malloc(len * sizeof (*filename));
	sprintf(filename, "%s/%s/", home, SAVE_DIR);
	makedirectory(filename);
	strcat(filename, "monster_desc.txt");

	ifstream in;
	in.open(filename);
	if(!in.good()) {
		return 1;
	}

	string line;
	int count = 0;
	int reading_monster = 0;
	character_t m;
	while(getline(in, line)) {

		// make sure first line is correct
		if(count == 0 && line.compare("RLG229 MONSTER DESCRIPTION 1") != 0) {
			return 1;
		}

		if(line.compare("BEGIN MONSTER") == 0) {
			reading_monster = 1;
		}

		if(line.)

		if(line.compare("END") == 0) {
			reading_monster = 0;
		}
	}
	
	return 0;
}