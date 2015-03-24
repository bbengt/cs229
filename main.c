#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "map.h"
#include "rungame.h"
#include "io.h"

int main(int argc, char *argv[]) {

	dungeon_t dungeon;

	srand(time(NULL));

	int do_mon = 0;

	if(argc > 1) {

		int i;
		for(i = 0; i < argc; i++) {
			if(strcmp(argv[i], "--nummon") == 0) {
				if(i + 1 >= argc || !sscanf(argv[i + 1], "%d", &dungeon.num_mons)) {
					fprintf(stderr, "Invalid number of mons\n");
					return 1;
				}
				do_mon = 1;
				continue;
			}
		}

	}

	if(!do_mon) {
		dungeon.num_mons = 0;
	}


	if(save_file_exists()) {
		if(!load(&dungeon)) {

			generate_map(&dungeon);

		}

	} else {

		generate_map(&dungeon);

	}

	run_game(&dungeon);

	return 0;

}