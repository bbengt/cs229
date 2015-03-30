#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

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
  	size_t len;

	home = getenv("HOME");

	len = (strlen(home) + strlen(SAVE_DIR) + strlen("monster_desc.txt") +
		1 /* The NULL terminator */                                 +
	    2 /* The slashes */);

	filename = (char*) malloc(len * sizeof (*filename));
	sprintf(filename, "%s/%s/", home, SAVE_DIR);
	mkdir(filename, 0755);
	strcat(filename, "monster_desc.txt");

	ifstream in;
	in.open(filename);
	if(!in.good()) {
		reading_monster = 0;
	}

	string line;
	int count = 0;
	int reading_monster = 0;
	int in_desc = 0;
	character_t m;
	std::string last("NOTHING");
	int desc_arr_size = 255;
	int desc_size = 0;

	m.desc = malloc(sizeof(char) * desc_arr_size);

	while(getline(in, line)) {

		// make sure first line is correct
		if(count == 0 && line.compare("RLG229 MONSTER DESCRIPTION 1") != 0) {
			exit(1);
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
				reading_monster = 0;
			}

			char *tokens[255];
			int n = 0;
			tokens[0] = strtok(line, " ");
			while(tokens[n] != NULL) {
				tokens[n] = strtok(NULL, " ");
				n++;
			}

			// copy each character of each element of tokens[1] through tokens[n] into m.name, inserting space after each token

			last = name_search;
		}

		if(reading_monster && line.compare(0, symb.length(), symb) == 0) {

			// make sure we're reading things in the correct order
			if(last.compare(name_search) != 0) {
				reading_monster = 0;
			}

			// split line at a space and discard the first half = we know it's SYMB
			strtok(line, " ");

			// the second half of line should be a single character containing the monster's symbol.  If there's more, we'll just grab the first character
			char *token = strtok(NULL, " ");
			m.symbol = token[0];

			last = symb;
		}

		if(reading_monster && line.compare(0, color_search.length(), color_search) == 0) {

			// make sure we're reading things in the correct order
			if(last.compare(symb) != 0) {
				reading_monster = 0;
			}

			// split line at a space and discard the first half = we know it's COLOR
			strtok(line, " ");

			// the second half of line will be the color
			char *token = strtok(NULL, " ");

			// make sure color is a valid color
			if(token.compare("RED") == 0 || token.compare("GREEN") == 0 || token.compare("BLUE") == 0 || token.compare("CYAN") == 0 || token.compare("YELLOW") == 0 || token.compare("MAGENTA") == 0 || token.compare("WHITE") == 0 || token.compare("BLACK") == 0) {
				m.color = token;
			} else {
				cout << "Invalid color: " << token;
				return 1;
			}

			last = color_search; 
		}

		if(reading_monster && line.compare(desc) == 0) {

			// make sure we're reading things in the correct order
			if(last.compare(color_search) != 0) {
				reading_monster = 0;
			}

			in_desc = 1;
			last = desc;
		}

		if(reading_monster && in_desc && last.compare(desc) == 0) {
			if(line.compare(period) == 0) {
				last = period;
			} else {

				// make sure we have enough space to hold a full line
				if(desc_size + 77 >= desc_arr_size) {

					// malloc more space into new array
					// copy contents of current array into new array
					// change array references 

				}

				// count how long line is, add it to desc_size
				// concatenate line to m.desc

			}

		}

		if(reading_monster && line.compare(0, speed_search.length(), speed_search) == 0) {

			// make sure we're reading things in the correct order
			if(last.compare(period) != 0) {
				reading_monster = 0;
			}

			// split line at a space and discard the first half = we know it's SPEED
			strtok(line, " ");

			// the second half of line will be the dice representation
			char *token = strtok(NULL, " ");

			// roll the dice to generate a fixed speed
			int speed = roll_dice(token);

			// check for a bad return value

			m.speed = speed;

			last = speed_search; 
		}

		if(reading_monster && line.compare(0, dam.length(), dam) == 0) {

			// make sure we're reading things in the correct order
			if(last.compare(speed_search) != 0) {
				reading_monster = 0;
			}

			// split line at a space and discard the first half = we know it's DAM
			strtok(line, " ");

			// the second half of line will be the dice representation
			char *token = strtok(NULL, " ");

			m.damage = token;

			last = dam;
		}

		if(reading_monster && line.compare(0, hp.length(), hp) == 0) {

			// make sure we're reading things in the correct order
			if(last.compare(dam) != 0) {
				reading_monster = 0;
			}

			// split line at a space and discard the first half = we know it's HP
			strtok(line, " ");

			// the second half of line will be the dice representation
			char *token = strtok(NULL, " ");

			// roll the dice to generate a fixed speed
			int health = roll_dice(token);

			// check for a bad return value

			m.hp = health;

			last = hp;
		}

		if(reading_monster && line.compare(0, abil.length(), abil) == 0) {

			// make sure we're reading things in the correct order
			if(last.compare(hp) != 0) {
				reading_monster = 0;
			}

			char *tokens[255];
			int n = 0;
			tokens[0] = strtok(line, " ");
			while(tokens[n] != NULL) {
				tokens[n] = strtok(NULL, " ");
				n++;
			}

			int i;
			for(i = 1; i < n; i++) {
				switch(tokens[i]) {
					case "SMART":
						m.characteristics |= NPC_SMART;
						break;
					case "TELE":
						// make m telepathic
						break;
					case "TUNNEL":
						// make m tunnel through rock
						break;
					case "PASS":
						// make m ethereal
						break;
				}
			}

			last = abil;
		}

		if(reading_monster && line.compare(end) == 0) {

			// make sure we're reading things in the correct order
			if(last.compare(abil) != 0) {
				reading_monster = 0;
			}

			// add monster to monster array
			// reset m

			last = end;
			reading_monster = 0;

		}

	}
	
	return 0;
}