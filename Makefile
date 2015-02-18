dungeon:
	gcc -Wall -Werror main.c -ggdb -o dungeon -lm

clean:
	rm -f dungeon
