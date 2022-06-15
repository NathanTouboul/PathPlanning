#include "headers/PathAlgorithm.h"
#include <iostream>
#include <queue>
#include <map>
#include <unistd.h>

#include <QtConcurrent>
#include <QFuture>


static int countLauchBFS{};

//Constructor
PathAlgorithm::PathAlgorithm(QObject* parent): QObject (parent)
{

}

//Destructor
PathAlgorithm::~PathAlgorithm()
{

}

//Getters: current Algorithm from gridView
ALGOS PathAlgorithm::getCurrentAlgorithm() const
{
    return currentAlgorithm;
}

void PathAlgorithm::runBFS(grid gridNodes)
{

    //qInfo() << "Run BFS on" << QThread::currentThread();
    //QFuture<grid> futureOutput = QtConcurrent::run(&PathAlgorithm::performBfsAlgorithm, this, gridNodes);
    performBfsAlgorithm(gridNodes);
}

// Methods
grid PathAlgorithm::performBfsAlgorithm(grid gridNodes)
{

    countLauchBFS++; std::cerr << countLauchBFS << " - ";
    //hardcoded
    int heightGrid = 10;

    // Display grid
    std::cerr << "State of grid node \n";
    int countVisited{}; int countObstacle{}; int countFree{};
    for (int i = 0; gridNodes.Nodes.size(); i++)
    {
        if (i % heightGrid == 0){std::cerr << "\n";}else{std::cerr << " - ";}

        std::cerr << "(" << gridNodes.Nodes[i].xCoord << ", " <<  gridNodes.Nodes[i].yCoord << ")";

        if (gridNodes.Nodes[i].visited){std::cerr << ": V"; countVisited++;}

        if (gridNodes.Nodes[i].obstacle){std::cerr << ": O"; countObstacle++;}
            else{std::cerr << ": F"; countFree++;}

    }
    std::cerr << "Totals: " << "Visited: " << countVisited
                            << "Obstacles: " << countObstacle
                            << "Free:" << countFree;


    return gridNodes;

    // Reach the goal
    bool reachEnd = false;

    // Initialize the queue of Nodes to visit in the next step
    std::queue<Node> nextNodes;

    // Initializing a vector of nodes (through copy of the original): to keep track of parents: index: indexparent, value: Node child
    std::vector<Node> parentNodes = gridNodes.Nodes;

    // Starting point
    nextNodes.push(gridNodes.Nodes[gridNodes.startIndex]);

    // Keeping track of the number of nodes left to check in the next layer and in the current layer
    int nodesInNextLayer = 0;
    int nodesLeftInCurrentLayer =1 ;

    // Counting the optimal number of moves needed to go from start to finish
    int moveCount {};

    while(!nextNodes.empty())
    {
        // Current Node
        Node currentNode =  nextNodes.front(); nextNodes.pop();
        int currentIndex = coordToIndex(currentNode.xCoord, currentNode.yCoord, heightGrid);

        // Update current in the gridView
        emit updatedgridView(CURRENT, currentIndex);


        if (currentIndex == gridNodes.endIndex)
        {
            reachEnd = true;
            break;
        }

        if (currentNode.visited == false || currentNode.obstacle == true)
        {
            // This node is now visited
            currentNode.visited = true;

            // Update this node as visited in the gridView
            emit updatedgridView(VISIT, currentIndex);


            // Retrieve neighbors and pushing it to the next nodes to check
            std::vector<Node> neighbors = retrieveNeighborsGrid(&gridNodes, currentNode, heightGrid);
            for (auto nextNode=neighbors.begin(); nextNode < neighbors.end(); nextNode++)
            {
                // Pushing the neighbors in the next nodes to be checked
                nextNodes.push(*nextNode);

                // Keeping track of the number of nodes in the next layers left to check
                nodesInNextLayer++;

                //Keeping track of parent node
                int parentIndex = coordToIndex((*nextNode).xCoord, (*nextNode).yCoord, heightGrid);
                parentNodes[parentIndex] = currentNode;
            }

            // This node has been visited
            nodesLeftInCurrentLayer--;

        }

        // if all nodes in the current layer have been checked
        if (nodesLeftInCurrentLayer == 0)
        {
            nodesLeftInCurrentLayer = nodesInNextLayer;
            nodesInNextLayer = 0;
            moveCount++;
        }

        std::cerr << "MOVE COUNT: " << moveCount << "\n";
        std::cerr << "CURRENT NODE INDEX: " <<currentIndex << "\n";

        // Time and checking for stop from running button
        sleep(2);

    }

    // Return -1 if goal not reached

}

std::vector<Node> PathAlgorithm::retrieveNeighborsGrid(const grid* gridNodes, const Node& currentNode, int heightGrid)
{
    std::vector<Node> neighbors;

    // rigth: adding +1 to x:
    if (currentNode.xCoord + 1 <= gridNodes->Nodes[-1].xCoord)
    {
        int rightIndex = coordToIndex(currentNode.xCoord + 1, currentNode.yCoord, heightGrid);
        neighbors.push_back(gridNodes->Nodes[rightIndex]);
    }

    // down: adding -1 to y:
    if (currentNode.yCoord - 1 >= gridNodes->Nodes[0].yCoord)
    {
        int downIndex = coordToIndex(currentNode.xCoord, currentNode.yCoord -1, heightGrid);
        neighbors.push_back(gridNodes->Nodes[downIndex]);
    }

    // rigth: adding -1 to x:
    if (currentNode.xCoord - 1 >= gridNodes->Nodes[0].xCoord)
    {
        int leftIndex = coordToIndex(currentNode.xCoord - 1, currentNode.yCoord, heightGrid);
        neighbors.push_back(gridNodes->Nodes[leftIndex]);
    }

    // up: adding +1 to y:
    if (currentNode.yCoord + 1 <= gridNodes->Nodes[-1].yCoord)
    {
        int upIndex = coordToIndex(currentNode.xCoord, currentNode.yCoord + 1, heightGrid);
        neighbors.push_back(gridNodes->Nodes[upIndex]);
    }

    return neighbors;
}
