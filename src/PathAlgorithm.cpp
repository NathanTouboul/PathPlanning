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
    int heightGrid = 10; int widthGrid = 10;

    // Display grid
    checkGridNode(gridNodes, heightGrid, widthGrid);

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
        std::cerr << "IN\n";
        // Current Node
        Node currentNode =  nextNodes.front(); nextNodes.pop();
        int currentIndex = coordToIndex(currentNode.xCoord, currentNode.yCoord, heightGrid);

        // Update current in the gridView
        emit updatedgridView(CURRENT, currentIndex);


        if (currentIndex == gridNodes.endIndex)
        {
            std::cerr << "Reached end \n";
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
        std::cerr << "CURRENT NODE: (" << currentNode.xCoord << ", " << currentNode.yCoord << ") \n";

        // Time and checking for stop from running button
        sleep(1);

    }

    // Display grid
    checkGridNode(gridNodes, heightGrid, widthGrid);


    return gridNodes;

}

std::vector<Node> PathAlgorithm::retrieveNeighborsGrid(const grid* gridNodes, const Node& currentNode, int heightGrid)
{
    std::vector<Node> neighbors;

    // right: adding +1 to x:
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

    // left: adding -1 to x:
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


void PathAlgorithm::checkGridNode(grid gridNodes, int heightGrid, int widthGrid)
{
    // Display grid
    std::cerr << "State of grid node \n";
    int countVisited = 0; int countObstacle = 0; int countFree = 0;
    for (Node node: gridNodes.Nodes)
    {
        std::cerr << "(" << node.xCoord << ", " <<  node.yCoord << "): ";

        if (node.visited){std::cerr << ": V"; countVisited++;}

        if (node.obstacle){std::cerr << ": O"; countObstacle++;}
            else{std::cerr << ": F"; countFree++;}
        std::cerr << " \n";
    }
    std::cerr << "Totals: " << "Visited: " << countVisited
                            << " - Obstacles: " << countObstacle
                            << " - Free:" << countFree << "\n";

    // Check size of vector
    if (static_cast<int>(gridNodes.Nodes.size()) != static_cast<int>(heightGrid * widthGrid))
    {std::cerr << "Number of nodes in gridNodes: " << gridNodes.Nodes.size() << " vs " << heightGrid * widthGrid << " [ISSUE] \n";}
    else{std::cerr << "Number of nodes in gridNodes: " << gridNodes.Nodes.size() << "\n";}

}
