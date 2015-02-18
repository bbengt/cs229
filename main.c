#include "map.c"

int main(int argc, char *argv[]) {

	dungeon_t dungeon;

	srand(time(NULL));

	int do_save, do_load, do_mob;
	do_save = do_load = do_mob = 0;

	if(argc > 1) {

		int i;
		for(i = 0; i < argc; i++) {
			if(strcmp(argv[i], "--save") == 0) {
				do_save = 1;
			}
			if(strcmp(argv[i], "--load") == 0) {
				do_load = 1;
			}
			if(strcmp(argv[i], "--nummon") == 0) {
				if(i + 1 >= argc || !sscanf(argv[i + 1], "%d", &dungeon.num_mobs)) {
					fprintf(stderr, "Invalid number of mobs\n");
					return 1;
				}
				do_mob = 1;
				continue;
			}
		}

	}

	if(!do_mob) {
		dungeon.num_mobs = 0;
	}


	if(do_load) {

		load(&dungeon);

	} else {

		generate_map(&dungeon);

	}

	if(do_save) {

		save(&dungeon);

	}

	// int i;
	// for(i = 0; i < dungeon.num_mobs; i++) {
	// 	dungeon.mobs[i].smart = 1;
	// 	move_mob(&dungeon, &dungeon.mobs[i]);
	// }

	print_map(&dungeon);

	return 0;

}