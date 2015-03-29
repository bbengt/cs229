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
	int in_desc = 0;
	character_t m;
	std::string last("NOTHING");
	while(getline(in, line)) {

		// make sure first line is correct
		if(count == 0 && line.compare("RLG229 MONSTER DESCRIPTION 1") != 0) {
			return 1;
		}

		// create strings to compare lines to
		std::string begin("BEGIN MONSTER");
		std::string name_search("NAME");
		std::string symb("SYMB");
		std::string color_search("COLOR");
		std::string desc("DESC");
		std::string period(".");
		std::string speed_search("SPEED");
		std::string dam("DAM");
		std::string hp("HP");
		std::string abil("ABIL");
		std::string end("END");

		// BEGIN MONSTER
		if(line.compare(0, begin.length(), begin) == 0) {
			reading_monster = 1;
			last = begin;
		}


		if(reading_monster && line.compare(0, name_search.length(), name_search) == 0) {

			// make sure we're reading things in the correct order
			if(last.compare(begin) != 0) {
				return 1;
			}

			// TODO
			last = name_search;
		}

		if(reading_monster && line.compare(0, symb.length(), symb) == 0) {

			// make sure we're reading things in the correct order
			if(last.compare(name_search) != 0) {
				return 1;
			}

			// TODO

			last = symb;
		}

		if(reading_monster && line.compare(0, color_search.length(), color_search) == 0) {

			// make sure we're reading things in the correct order
			if(last.compare(symb) != 0) {
				return 1;
			}

			// TODO

			last = color_search; 
		}

		if(reading_monster && line.compare(desc) == 0) {

			// make sure we're reading things in the correct order
			if(last.compare(color_search) != 0) {
				return 1;
			}

			in_desc = 1;
			last = desc;
		}

		if(reading_monster && in_desc && last.compare(desc) == 0) {
			if(line.compare(period) == 0) {
				last = period;
			} else {

				// TODO
			}

		}

	}
	
	return 0;
}