## Computer Science 229 Rogue-Like Game

### Overview
This game was written for Computer Science 229 using C and C++.  It's an ASCII-based terminal rogue-like game like Nethack (see [this Wikipedia article](https://en.wikipedia.org/wiki/Roguelike) for more on rogue-like games).  It's currently under heavy development, so please forgive any bugs that may occur.

### Progress
The game is still (very much) a work in progress.  It can currently:

* Generate a game map, complete with rooms, corridors, and stairs leading to new levels
* Save and load the game state (currently only the game map)
* Support moving the player around the map
* Monsters can chase the player (depending on the monster's characteristics)

### Running the Game
If you want to try running this game, clone the repo, or download and extract the Zip copy of the archive.  Once you have the files:
  * On Linux: open a Terminal window in the directory that contains the files you downloaded, then type `make` to compile the program, then `./dungeon` to run the program.
  * On any other platform: Sorry, I can't be of any help here.  This was written on and designed for Linux.  I'm not sure if it'll work on other platforms.

### Credits

  * Jeremy Sheaffer - my Computer Science 229 professor. He wrote the code for the heap (heap.c & heap.h) as well as the slightly 
unusual implementation of Dijkstra's algorithm (dijkstra() in rungame.c).
