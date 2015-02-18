## Computer Science 229 Rogue-Like Game

### Overview
This game was written for Computer Science 229 using C and C++.  It's an old-school rogue-like game (see [this Wikipedia article](https://en.wikipedia.org/wiki/Roguelike) for more on rogue-like games.  Eventually, it will only render the area of the game map around the user, have monsters that will chase the player around the dungeon, and many other features common to rogue-like games.

### Progress
The game is still (very much) a work in progress.  It can currently:

-Generate a game map, complete with rooms and corridors
-Save and load the game state (currently only the game map)
-Some legwork towards a monster AI (using Dijkstra's shortest path algorithm)