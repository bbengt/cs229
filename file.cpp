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
#include "npc.h"

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
		exit(1);
	}

	string line;
	int count = 0;
	int reading_monster = 0;
	int in_desc = 0;
	character_t *m;
	std::string last("NOTHING");
	int desc_arr_size = 255;
	int desc_size = 0;

	while(getline(in, line)) {

		const char *line_c = line.c_str();
		char *line_c_mut = const_cast<char*>(line_c);


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
			m = (character_t *) malloc(sizeof(m));
			m->desc = (char *) malloc(sizeof(char) * desc_arr_size);
			generate_coords(d, m);
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
			tokens[0] = strtok(line_c_mut, " ");
			while(tokens[n] != NULL) {
				tokens[n] = strtok(NULL, " ");
				n++;
			}

			std::string name;

			// copy each character of each element of tokens[1] through tokens[n] into m.name, inserting space after each token
			int i;
			for(i = 0; i < n; i++) {
				std::string to_add(tokens[i]);
				name += to_add;
			}
			m->name = const_cast<char*>(name.c_str());

			last = name_search;
		}

		if(reading_monster && line.compare(0, symb.length(), symb) == 0) {

			// make sure we're reading things in the correct order
			if(last.compare(name_search) != 0) {
				reading_monster = 0;
			}

			// split line at a space and discard the first half = we know it's SYMB
			strtok(line_c_mut, " ");

			// the second half of line should be a single character containing the monster's symbol.  If there's more, we'll just grab the first character
			char *token = strtok(NULL, " ");
			m->symbol = token[0];

			last = symb;
		}

		if(reading_monster && line.compare(0, color_search.length(), color_search) == 0) {

			// make sure we're reading things in the correct order
			if(last.compare(symb) != 0) {
				reading_monster = 0;
			}

			// split line at a space and discard the first half = we know it's COLOR
			strtok(line_c_mut, " ");

			// the second half of line will be the color
			char *token_c = strtok(NULL, " ");
			std::string token(token_c);

			// make sure color is a valid color
			if(token.compare("RED") == 0 || token.compare("GREEN") == 0 || token.compare("BLUE") == 0 || token.compare("CYAN") == 0 || token.compare("YELLOW") == 0 || token.compare("MAGENTA") == 0 || token.compare("WHITE") == 0 || token.compare("BLACK") == 0) {
				m->color = const_cast<char*>(token.c_str());
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

					char *tmp = (char *) malloc(2 * sizeof(tmp) * desc_size);
					strcpy(tmp, m->desc);
					m->desc = tmp;

				}

				desc_size += line.length();
				std::string tmp = m->desc + line;
				m->desc = const_cast<char*>(tmp.c_str());

			}

		}

		if(reading_monster && line.compare(0, speed_search.length(), speed_search) == 0) {

			// make sure we're reading things in the correct order
			if(last.compare(period) != 0) {
				reading_monster = 0;
			}

			// split line at a space and discard the first half = we know it's SPEED
			strtok(line_c_mut, " ");

			// the second half of line will be the dice representation
			char *token = strtok(NULL, " ");

			// roll the dice to generate a fixed speed
			int speed = roll_dice(token);

			// check for a bad return value

			m->speed = speed;

			last = speed_search; 
		}

		if(reading_monster && line.compare(0, dam.length(), dam) == 0) {

			// make sure we're reading things in the correct order
			if(last.compare(speed_search) != 0) {
				reading_monster = 0;
			}

			// split line at a space and discard the first half = we know it's DAM
			strtok(line_c_mut, " ");

			// the second half of line will be the dice representation
			char *token = strtok(NULL, " ");

			m->damage = token;

			last = dam;
		}

		if(reading_monster && line.compare(0, hp.length(), hp) == 0) {

			// make sure we're reading things in the correct order
			if(last.compare(dam) != 0) {
				reading_monster = 0;
			}

			// split line at a space and discard the first half = we know it's HP
			strtok(line_c_mut, " ");

			// the second half of line will be the dice representation
			char *token = strtok(NULL, " ");

			// roll the dice to generate a fixed speed
			int health = roll_dice(token);

			// check for a bad return value

			m->hp = health;

			last = hp;
		}

		if(reading_monster && line.compare(0, abil.length(), abil) == 0) {

			// make sure we're reading things in the correct order
			if(last.compare(hp) != 0) {
				reading_monster = 0;
			}

			char *tokens[255];
			int n = 0;
			tokens[0] = strtok(line_c_mut, " ");
			while(tokens[n] != NULL) {
				tokens[n] = strtok(NULL, " ");
				n++;
			}

			std::string smart_str("SMART");
			std::string tele_str("TELE");
			std::string tunnel_str("TUNNEL");
			std::string pass_str("PASS");
			int i;
			for(i = 1; i < n; i++) {

				std::string cpp_str(tokens[i]);
				if(cpp_str.compare(pass_str) == 0) {
					// make m smart
					continue;
				}
				if(cpp_str.compare(tele_str) == 0) {
					//make m telepathic
					continue;
				}
				if(cpp_str.compare(tunnel_str) == 0) {
					// make m tunnel through rock
					continue;
				}
				if(cpp_str.compare(smart_str) == 0) {
					// make m ethereal
					continue;
				}
			}

			last = abil;
		}

		if(reading_monster && line.compare(end) == 0) {

			// make sure we're reading things in the correct order
			if(last.compare(abil) != 0) {
				reading_monster = 0;
			}

			d->character[m->position[dim_y]][m->position[dim_x]] = m;

			last = end;
			reading_monster = 0;

		}

	}
	
	return 0;
}