## Computer Science 229 Rogue-Like Game

### Overview
This game was written for Computer Science 229 using C and C++.  It's an old-school rogue-like game like Nethack (see [this Wikipedia article](https://en.wikipedia.org/wiki/Roguelike) for more on rogue-like games).  Eventually, it will only render the area of the game map around the user, have monsters that will chase the player around the dungeon, support user interaction (obviously) and many other features common to rogue-like games.

### Progress
The game is still (very much) a work in progress.  It can currently:

* Generate a game map, complete with rooms and corridors
* Save and load the game state (currently only the game map)
* Calculate the shortest path between two random open points on the map (for monster AI)

### Running the Game
If you want to try running this game, clone the repo, or download and extract the Zip copy of the archive.  Once you have the files:
  * On Linux: open a Terminal window in the directory that contains the files you downloaded, then type `make` to compile the program, then `./dungeon` to run the program.
  * On any other platform: Sorry, I can't be of any help here.  I've only ever tested this on Linux.  See if Google has any insight on compiling and running C files on your platform.
