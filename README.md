# PathFindingVisualizer  ／/ ┊ \＼
> A C++ SFML based pathfinding visualizer to get the intuition behind graph algorithms.

### What it does?
Given a source location , a destination location finds the shortest path to it taking into consideration the blocked paths and visually presents it using 2 different graph algoriths viz dijkstra and A-star (A*) algorithms, and once the program is executes it provides the number of nodes visited by each to the algorithm to reach to the solution.

### How to set it up?
Install the SFML Library in C++
https://www.sfml-dev.org/download.php

And use the following terminal commands:
- g++ -c main.cpp
(This compiles the code).
- g++ main.o -o test -lsfml-window -lsfml-graphics -lsfml-system
(Runs the output file and makes a test file to be executed).
- ./test
(Runs the code)


### How to play with it!(Controls)

- Hover the cursor over the block which is to selected as the source node and press "s" key to mark it, do the same for destination node but now press "t" key instead
- To add Walls or obstruction hover over the blocks and keep pressing the "w" key
- To remove Walls hover over the blocks and press the "a" key
- To run the algorithm use "r" key(runs A* by default)
- To run dijkstra use "d" and hit "r" key
- To run A* back again use "z" key
- Use "c" to clear the canvas and start with diffrent set of parameters

### Demo!!!

![pathVisual](https://user-images.githubusercontent.com/43462814/76686066-bb9f3480-663e-11ea-827a-5af26d9b9539.gif)
