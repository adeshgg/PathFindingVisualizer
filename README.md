# PathFindingVisualizer  ／/ ┊ \＼
> A C++ SFML based visualizer to get the intuition behind graph algorithms.

### What it does?
Given a source location , a destination location finds the shortest path to it taking into consideration the blocked paths and visually presents it using 2 different graph algoriths viz dijkstra and A-star (A*) algorithms, and once the program is executes it provides the number of nodes visited by each to the algorithm to reach to the solution.

### Who to set it up?
Install the SFML Library in C++
https://www.sfml-dev.org/download.php

And use the following terminal commands:
-g++ -c main.cpp
(This compiles the code).
-g++ main.o -o test -lsfml-window -lsfml-graphics -lsfml-system
(Runs the output file and makes a test file to be executed).
-./test
(Runs the code)

