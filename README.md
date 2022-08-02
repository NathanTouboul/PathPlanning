# Path Finding with Qt and C++

This project is centered around path-finding algorithms and the Qt framework. The idea was for me to find a good way to visualize those algorithms searching for the goal while building with a "pretty" GUI. The entire application is coded with C++ and built with qmake.

VIDEO


## Algorithms
The algorithms implemented here are the following:
- A*
- Dijkstra
- BFS
- DFS
- Back-Tracking (for maze generation)

Apart from using Qt data types, I used multithreading for real-time simulation and visualization.This is helful to have a GUI with great performance. Also, the nice thing about multithreading is that we can change the speed of the simulation dynamically.

## GUI 
The GUI was created using Qt Creator. There are two main tabs:
- Simulation: Selecting the algorithm, placing the start and goal, and adding/removing obstacles
    PICTURE TAB SIMULATION
- Vizualization: Increasing the number of nodes horizontally and vertically, and changing the size of these nodes.
    PICTURE TAB VISUALIZATION

