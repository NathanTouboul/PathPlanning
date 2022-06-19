#include "headers/PathAlgorithm.h"
#include <iostream>
#include <queue>
#include <map>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <QtConcurrent>
#include <QFuture>
#include <stack>

static int countLauchBFS{};

//Constructor
PathAlgorithm::PathAlgorithm(QObject* parent): QObject (parent)
{
    // The algorithm is not running at startup
    running = false;
    simulationOnGoing = false;
    endReached = false;

    speedVisualization = 10;
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

void PathAlgorithm::runAlgorithm(ALGOS algorithm)
{
    simulationOnGoing=true;
    running=true;

    qInfo() << "Run " << algorithm <<" on" << QThread::currentThread();

    //futureOutput = QtConcurrent::run(&PathAlgorithm::performBfsAlgorithm, this, gridNodes);

    switch (algorithm) {
    case BFS:
        futureOutput = QtConcurrent::run(&pool, &PathAlgorithm::performBFSAlgorithm, this);
        break;
    case DFS:
        futureOutput = QtConcurrent::run(&pool, &PathAlgorithm::performDFSAlgorithm, this);

        break;
    default:
        break;

    }

}

void PathAlgorithm::resumeAlgorithm()
{
    running = true;
    futureOutput.resume();
}

void PathAlgorithm::pauseAlgorithm()
{
    running = false;
    futureOutput.suspend();
}

// BFS Algorithm
void PathAlgorithm::performBFSAlgorithm(QPromise<int>& promise)
{

    // Allow to pause and stop the simulation (to debug)
    promise.suspendIfRequested();
    if (promise.isCanceled())
        return;

    // Display grid
    checkGridNode(gridNodes, heightGrid, widthGrid);

    // Reach the goal
    bool reachEnd = false;

    // Initialize the queue of Nodes to visit in the next step
    std::queue<Node> nextNodes;

    // Initializing a vector of nodes (through copy of the original): to keep track of parents: index: index parent, value: Node child
    std::vector<Node> parentNodes = gridNodes.Nodes;

    // Initializing a vector of nodes (through copy of the original): to recreate the path: index: index parent, value: Node child
    std::vector<Node> pathNodes = gridNodes.Nodes;

    // Starting point
    nextNodes.push(gridNodes.Nodes[gridNodes.startIndex]);

    // Keeping track of the number of nodes left to check in the next layer and in the current layer
    int nodesInNextLayer = 0;
    int nodesLeftInCurrentLayer =1 ;

    // Counting the optimal number of moves needed to go from start to finish
    int moveCount {};

    // Initialization current node
    Node currentNode;

    while(!nextNodes.empty())
    {

        // Current Node
        currentNode =  nextNodes.front(); nextNodes.pop();
        int currentIndex = coordToIndex(currentNode.xCoord, currentNode.yCoord, widthGrid);

        if (currentIndex == gridNodes.endIndex)
        {
            std::cerr << "Reached end \n";
            reachEnd = true;
            break;
        }

        if (currentNode.visited == false && currentNode.obstacle == false)
        {

            // This node is now visited
            currentNode.visited = true;
            gridNodes.Nodes[currentIndex].visited = true;

            // Update this node as visited in the gridView
            emit updatedgridView(VISIT, currentIndex);

            // Retrieve neighbors and pushing it to the next nodes to check
            std::vector<Node> neighbors = retrieveNeighborsGrid(gridNodes, currentNode, widthGrid, heightGrid);

            for (auto nextNode=neighbors.begin(); nextNode < neighbors.end(); nextNode++)
            {
                // Pushing the neighbors in the next nodes to be checked if the node has not been added in the nextNode before
                if ((*nextNode).nextUp == false){

                    int nextIndex = coordToIndex((*nextNode).xCoord, (*nextNode).yCoord, widthGrid);

                    gridNodes.Nodes[nextIndex].nextUp = true;
                    nextNodes.push(*nextNode);

                    emit updatedgridView(NEXT, nextIndex);

                    // Keeping track of the number of nodes in the next layers left to check
                    nodesInNextLayer++;

                    //Keeping track of parent node
                    parentNodes[nextIndex] = currentNode;
                    parentNodes[nextIndex].xCoord = currentNode.xCoord;
                    parentNodes[nextIndex].yCoord = currentNode.yCoord;


                }
            }

            // This node has been visited
            nodesLeftInCurrentLayer--;


            // Time and checking for stop from running button
            std::this_thread::sleep_for(std::chrono::milliseconds(speedVisualization));

        }

        // if all nodes in the current layer have been checked
        if (nodesLeftInCurrentLayer == 0)
        {
            nodesLeftInCurrentLayer = nodesInNextLayer;
            nodesInNextLayer = 0;
            moveCount++;
        }

        //std::cerr << "MOVE COUNT: " << moveCount << "\n";

    }

    // Display grid
    checkGridNode(gridNodes, heightGrid, widthGrid);

    // updating promise
    promise.addResult(moveCount);

    // If the end is reached, we output the path
    if (reachEnd){

        endReached = true;

        Node goal = currentNode;
        Node reverse = goal;

        int count=0;
        while(reverse.xCoord != gridNodes.Nodes[gridNodes.startIndex].xCoord || reverse.yCoord != gridNodes.Nodes[gridNodes.startIndex].yCoord)
        {

            int reverseIndex = coordToIndex(reverse.xCoord, reverse.yCoord, widthGrid);
            Node parentNode = parentNodes[reverseIndex];

            emit updatedgridView(PATH, reverseIndex);
            reverse = parentNode;
            count++;
            std::this_thread::sleep_for(std::chrono::milliseconds(speedVisualization));


        }

    }else{
        endReached = false;
    }

}

// DFS Algorithm
void PathAlgorithm::performDFSAlgorithm(QPromise<int>& promise)
{

    // Allow to pause and stop the simulation (to debug)
    promise.suspendIfRequested();
    if (promise.isCanceled())
        return;

    // Display grid
    checkGridNode(gridNodes, heightGrid, widthGrid);

    // Reach the goal
    bool reachEnd = false;

    // Initialize the queue of Nodes to visit in the next step
    std::stack<Node> nextNodes;

    // Initializing a vector of nodes (through copy of the original): to keep track of parents: index: index parent, value: Node child
    std::vector<Node> parentNodes = gridNodes.Nodes;

    // Initializing a vector of nodes (through copy of the original): to recreate the path: index: index parent, value: Node child
    std::vector<Node> pathNodes = gridNodes.Nodes;

    // Starting point
    nextNodes.push(gridNodes.Nodes[gridNodes.startIndex]);

    // Keeping track of the number of nodes left to check in the next layer and in the current layer
    int nodesInNextLayer = 0;
    int nodesLeftInCurrentLayer =1 ;

    // Counting the optimal number of moves needed to go from start to finish
    int moveCount {};

    // Initialization current node
    Node currentNode;

    while(!nextNodes.empty())
    {
        // Current Node
        currentNode =  nextNodes.top(); nextNodes.pop();
        int currentIndex = coordToIndex(currentNode.xCoord, currentNode.yCoord, widthGrid);

        if (currentIndex == gridNodes.endIndex)
        {
            std::cerr << "Reached end \n";
            reachEnd = true;
            break;
        }

        if (currentNode.visited == false && currentNode.obstacle == false)
        {
            // This node is now visited
            currentNode.visited = true;
            gridNodes.Nodes[currentIndex].visited = true;

            // Update this node as visited in the gridView
            emit updatedgridView(VISIT, currentIndex);

            // Retrieve neighbors and pushing it to the next nodes to check
            std::vector<Node> neighbors = retrieveNeighborsGrid(gridNodes, currentNode, widthGrid, heightGrid);

            for (auto nextNode=neighbors.begin(); nextNode < neighbors.end(); nextNode++)
            {
                // Pushing the neighbors in the next nodes to be checked if the node has not been added in the nextNode before
                if ((*nextNode).nextUp == false){

                    int nextIndex = coordToIndex((*nextNode).xCoord, (*nextNode).yCoord, widthGrid);

                    gridNodes.Nodes[nextIndex].nextUp = true;
                    nextNodes.push(*nextNode);

                    emit updatedgridView(NEXT, nextIndex);

                    // Keeping track of the number of nodes in the next layers left to check
                    nodesInNextLayer++;

                    //Keeping track of parent node
                    parentNodes[nextIndex] = currentNode;
                    parentNodes[nextIndex].xCoord = currentNode.xCoord;
                    parentNodes[nextIndex].yCoord = currentNode.yCoord;


                }
            }

            // This node has been visited
            nodesLeftInCurrentLayer--;

            // Time and checking for stop from running button
            std::this_thread::sleep_for(std::chrono::milliseconds(speedVisualization));

        }

        // if all nodes in the current layer have been checked
        if (nodesLeftInCurrentLayer == 0)
        {
            nodesLeftInCurrentLayer = nodesInNextLayer;
            nodesInNextLayer = 0;
            moveCount++;
        }

        //std::cerr << "MOVE COUNT: " << moveCount << "\n";

    }

    // Display grid
    checkGridNode(gridNodes, heightGrid, widthGrid);

    // updating promise
    promise.addResult(moveCount);

    // If the end is reached, we output the path
    if (reachEnd){

        endReached = true;

        Node goal = currentNode;
        Node reverse = goal;

        int count=0;
        while(reverse.xCoord != gridNodes.Nodes[gridNodes.startIndex].xCoord || reverse.yCoord != gridNodes.Nodes[gridNodes.startIndex].yCoord)
        {

            int reverseIndex = coordToIndex(reverse.xCoord, reverse.yCoord, widthGrid);
            Node parentNode = parentNodes[reverseIndex];

            emit updatedgridView(PATH, reverseIndex);
            reverse = parentNode;
            count++;
            std::this_thread::sleep_for(std::chrono::milliseconds(speedVisualization));


        }

    }else{
        endReached = false;
    }

}
std::vector<Node> PathAlgorithm::retrieveNeighborsGrid(const grid& gridNodes, const Node& currentNode, int widthGrid, int heightGrid)
{

    std::vector<Node> neighbors;


    // diagonal right up : adding +1 to x and +1 to y
    if (currentNode.xCoord + 1 <= widthGrid && currentNode.yCoord + 1 <= heightGrid)
    {
        int rightUpIndex = coordToIndex(currentNode.xCoord + 1, currentNode.yCoord +1, widthGrid);
        neighbors.push_back(gridNodes.Nodes[rightUpIndex]);
    }

    // right: adding +1 to x:
    if (currentNode.xCoord + 1 <= widthGrid)
    {
        int rightIndex = coordToIndex(currentNode.xCoord + 1, currentNode.yCoord, widthGrid);
        neighbors.push_back(gridNodes.Nodes[rightIndex]);

    }

    // diagonal right down : adding +1 to x and -1 to y
    if (currentNode.xCoord + 1 <= widthGrid && currentNode.yCoord - 1 >= 1)
    {
        int rightDownIndex = coordToIndex(currentNode.xCoord + 1, currentNode.yCoord -1, widthGrid);
        neighbors.push_back(gridNodes.Nodes[rightDownIndex]);
    }

    // down: adding -1 to y:
    if (currentNode.yCoord - 1 >= 1)
    {
        int downIndex = coordToIndex(currentNode.xCoord, currentNode.yCoord -1, widthGrid);
        neighbors.push_back(gridNodes.Nodes[downIndex]);
    }

    // diagonal left  down : adding -1 to x and -1 to y
    if (currentNode.xCoord - 1 >= 1 && currentNode.yCoord - 1 >= 1)
    {
        int leftDownIndex = coordToIndex(currentNode.xCoord - 1, currentNode.yCoord - 1, widthGrid);
        neighbors.push_back(gridNodes.Nodes[leftDownIndex]);
    }

    // left: adding -1 to x:
    if (currentNode.xCoord - 1 >= 1)
    {
        int leftIndex = coordToIndex(currentNode.xCoord - 1, currentNode.yCoord, widthGrid);
        neighbors.push_back(gridNodes.Nodes[leftIndex]);
    }

    // diagonal left  up : adding -1 to x and +1 to y
    if (currentNode.xCoord - 1 >= 1 && currentNode.yCoord + 1 <= heightGrid)
    {
        int leftUpIndex = coordToIndex(currentNode.xCoord - 1, currentNode.yCoord + 1, widthGrid);
        neighbors.push_back(gridNodes.Nodes[leftUpIndex]);
    }

    // up: adding +1 to y:
    if (currentNode.yCoord + 1 <= heightGrid)
    {
        int upIndex = coordToIndex(currentNode.xCoord, currentNode.yCoord + 1, widthGrid);
        neighbors.push_back(gridNodes.Nodes[upIndex]);
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

        if (node.xCoord == widthGrid){std::cerr << " \n";}
        else{std::cerr << " | ";}

    }
    std::cerr << "Totals: " << "Visited: " << countVisited
                            << " - Obstacles: " << countObstacle
                            << " - Free:" << countFree << "\n";

    // Check size of vector
    if (static_cast<int>(gridNodes.Nodes.size()) != static_cast<int>(heightGrid * widthGrid))
    {std::cerr << "Number of nodes in gridNodes: " << gridNodes.Nodes.size() << " vs " << heightGrid * widthGrid << " [ISSUE] \n";}
    else{std::cerr << "Number of nodes in gridNodes: " << gridNodes.Nodes.size() << "\n";}

}
