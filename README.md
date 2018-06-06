# Arduboy2DRaycast
Testing project for 2d tile based raycasting

### Problem
There is a 2D tilebased map with a player character on one tile.
The question is: what can the character see?

It's not enough to fill a circle with a given radius because there can be obstacles which are opaque.

### Solutions
There are noumerous possible solutions with different pros/cons. You can reade [more about it here](http://www.adammil.net/blog/v125_Roguelike_Vision_Algorithms.html "more about it here").
In this project it is solved with raycasting unsing Bresenham's Circle and Line algorithm.
