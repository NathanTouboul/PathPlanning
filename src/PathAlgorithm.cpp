#include "headers/PathAlgorithm.h"
#include "QtConcurrent/qtconcurrentrun.h"
#include <iostream>
#include <queue>
#include <map>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <QtConcurrent>
#include <QFuture>
#include <stack>
#include <list>

//Constructor
PathAlgorithm::PathAlgorithm(QObject* parent): QObject (parent)
{
    // The algorithm is not running at startup
    running = false;
    simulationOnGoing = false;
    endReached = false;

    speedVisualization = 100;
}

//Destructor
PathAlgorithm::~PathAlgorithm()
{

}

//Getters/Setters: current Algorithm from gridView
ALGOS PathAlgorithm::getCurrentAlgorithm() const
{
    return currentAlgorithm;
}

//Getters/Setters: current Algorithm from gridView
void PathAlgorithm::setCurrentAlgorithm(ALGOS algorithm)
{
    this->currentAlgorithm = algorithm;
}

void PathAlgorithm::setSpeedVizualization(int speed)
{
    this->speedVisualization = speed;
}

// Getters/Setters: Simulation on going
void PathAlgorithm::setSimulationOnGoing(bool onGoing)
{
    this->simulationOnGoing = onGoing;
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

void PathAlgorithm::runAlgorithm(ALGOS algorithm)
{
    simulationOnGoing=true;
    running=true;

    switch (algorithm) {
    case BFS:
        futureOutput = QtConcurrent::run(&pool, &PathAlgorithm::performBFSAlgorithm, this);
        break;
    case DFS:
        futureOutput = QtConcurrent::run(&pool, &PathAlgorithm::performDFSAlgorithm, this);
        break;
    case DIJKSTRA:
        futureOutput = QtConcurrent::run(&pool, &PathAlgorithm::performDijkstraAlgorithm, this);
        break;
    case ASTAR:
        futureOutput = QtConcurrent::run(&pool, &PathAlgorithm::performAStarAlgorithm, this);
        break;
    case BACKTRACK:
        futureOutput = QtConcurrent::run(&pool, &PathAlgorithm::performRecursiveBackTrackerAlgorithm, this);
        break;
    case NOALGO:
        std::cerr <<"NO ALGO \n";
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

void PathAlgorithm::stopAlgorithm()
{
    running = false;
    futureOutput.cancel();
}

// BFS Algorithm
void PathAlgorithm::performBFSAlgorithm(QPromise<int>& promise)
{

    // Allow to pause and stop the simulation (to debug)
    promise.suspendIfRequested();
    if (promise.isCanceled())
        return;

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

    //
    bool addingPoint = true;

    while(!nextNodes.empty())
    {

        // Current Node
        currentNode =  nextNodes.front(); nextNodes.pop();
        int currentIndex = coordToIndex(currentNode.xCoord, currentNode.yCoord, widthGrid);

        // updating Line gridView
        emit updatedLineGridView(QPointF(currentNode.xCoord, currentNode.yCoord), addingPoint);
        addingPoint = false;

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
            emit updatedScatterGridView(VISIT, currentIndex);

            // Retrieve neighbors and pushing it to the next nodes to check
            std::vector<Node> neighbors = retrieveNeighborsGrid(gridNodes, currentNode, widthGrid, heightGrid);

            for (auto nextNode=neighbors.begin(); nextNode < neighbors.end(); nextNode++)
            {
                // Pushing the neighbors in the next nodes to be checked if the node has not been added in the nextNode before
                if ((*nextNode).nextUp == false){

                    int nextIndex = coordToIndex((*nextNode).xCoord, (*nextNode).yCoord, widthGrid);

                    gridNodes.Nodes[nextIndex].nextUp = true;
                    nextNodes.push(*nextNode);

                    emit updatedScatterGridView(NEXT, nextIndex);

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
            addingPoint = true;
        }

    }

    // If the end is reached, we output the path
    if (reachEnd){

        endReached = true;

        Node goal = currentNode;
        Node reverse = goal;

        // Line Path
        emit updatedLineGridView(QPointF(reverse.xCoord, reverse.yCoord), true, true);

        while(reverse.xCoord != gridNodes.Nodes[gridNodes.startIndex].xCoord || reverse.yCoord != gridNodes.Nodes[gridNodes.startIndex].yCoord){

            emit updatedLineGridView(QPointF(reverse.xCoord, reverse.yCoord), true, false);

            int reverseIndex = coordToIndex(reverse.xCoord, reverse.yCoord, widthGrid);
            Node parentNode = parentNodes[reverseIndex];

            reverse = parentNode;

            // Updating the GridView with PATH
            emit updatedScatterGridView(PATH, reverseIndex);
            std::this_thread::sleep_for(std::chrono::milliseconds(speedVisualization));

        }

        emit updatedLineGridView(QPointF(gridNodes.Nodes[gridNodes.startIndex].xCoord, gridNodes.Nodes[gridNodes.startIndex].yCoord), true, false);


    }else{
        endReached = false;
    }

    emit algorithmCompleted();

}

// DFS Algorithm
void PathAlgorithm::performDFSAlgorithm(QPromise<int>& promise)
{

    // Allow to pause and stop the simulation (to debug)
    promise.suspendIfRequested();
    if (promise.isCanceled())
        return;

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
    bool addingPoint = true;
    while(!nextNodes.empty())
    {
        // Current Node
        currentNode =  nextNodes.top(); nextNodes.pop();
        int currentIndex = coordToIndex(currentNode.xCoord, currentNode.yCoord, widthGrid);

        // updating Line gridView
        emit updatedLineGridView(QPointF(currentNode.xCoord, currentNode.yCoord), addingPoint);
        addingPoint = false;

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
            emit updatedScatterGridView(VISIT, currentIndex);

            // Retrieve neighbors and pushing it to the next nodes to check
            std::vector<Node> neighbors = retrieveNeighborsGrid(gridNodes, currentNode, widthGrid, heightGrid);

            for (auto nextNode=neighbors.begin(); nextNode < neighbors.end(); nextNode++)
            {
                // Pushing the neighbors in the next nodes to be checked if the node has not been added in the nextNode before
                if ((*nextNode).nextUp == false){

                    int nextIndex = coordToIndex((*nextNode).xCoord, (*nextNode).yCoord, widthGrid);

                    gridNodes.Nodes[nextIndex].nextUp = true;
                    nextNodes.push(*nextNode);

                    emit updatedScatterGridView(NEXT, nextIndex);

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
        if (nodesLeftInCurrentLayer == 0){
            nodesLeftInCurrentLayer = nodesInNextLayer;
            nodesInNextLayer = 0;
            moveCount++;
            addingPoint = true;

        }

    }

    // updating promise
    promise.addResult(moveCount);

    // If the end is reached, we output the path
    if (reachEnd){

        endReached = true;

        Node goal = currentNode;
        Node reverse = goal;

        // Line Path
        emit updatedLineGridView(QPointF(reverse.xCoord, reverse.yCoord), true, true);

        int count=0;
        while(reverse.xCoord != gridNodes.Nodes[gridNodes.startIndex].xCoord || reverse.yCoord != gridNodes.Nodes[gridNodes.startIndex].yCoord)
        {
            emit updatedLineGridView(QPointF(reverse.xCoord, reverse.yCoord), true, false);

            int reverseIndex = coordToIndex(reverse.xCoord, reverse.yCoord, widthGrid);
            Node parentNode = parentNodes[reverseIndex];

            emit updatedScatterGridView(PATH, reverseIndex);
            reverse = parentNode;
            count++;
            std::this_thread::sleep_for(std::chrono::milliseconds(speedVisualization));
        }
        emit updatedLineGridView(QPointF(gridNodes.Nodes[gridNodes.startIndex].xCoord, gridNodes.Nodes[gridNodes.startIndex].yCoord), true, false);

    }else{
        endReached = false;
    }

    emit algorithmCompleted();

}


void PathAlgorithm::performDijkstraAlgorithm(QPromise<int>& promise)
{
    // Allow to pause and stop the simulation (to debug)
    promise.suspendIfRequested();
    if (promise.isCanceled())
        return;

    // Create connections for each node
    for(Node& node: gridNodes.Nodes)
    {
        // Create connections between nodes (filling the vector neighbours of the Node structure)
        FillNeighboursNode(node);

        // Navigation Graph -> reset all nodes states
        node.localGoal      = INFINITY;
        node.parent         = nullptr;
    }

    // Lambda function to compute distance between points
    auto distance = [](Node* a, Node* b)
    {
        return sqrtf(   (a->xCoord - b->xCoord) * (a->xCoord - b->xCoord)
                       +(a->yCoord - b->yCoord) * (a->yCoord - b->yCoord));
    };


    // Starting conditions
    Node* nodeStart = &(gridNodes.Nodes[gridNodes.startIndex]);
    Node* nodeEnd = &(gridNodes.Nodes[gridNodes.endIndex]);

    Node* nodeCurrent = &(gridNodes.Nodes[gridNodes.startIndex]);
    nodeStart->localGoal = 0.0f;

    std::list<Node*> nodesToTest;
    nodesToTest.push_back(nodeCurrent);

    while(!nodesToTest.empty())
    {

        // Sorting untested nodes by global goal
        nodesToTest.sort([](const Node* a, const Node* b){return a->localGoal < b->localGoal;});

        // The lowest goal can also already been visited, so we pop it in this case
        while(!nodesToTest.empty() && nodesToTest.front()->visited)     {   nodesToTest.pop_front();    }

        // Breaking if list is empty
        if (nodesToTest.empty())    {   break;  }

        // Changing current node and setting it to visited
        nodeCurrent = nodesToTest.front();
        nodeCurrent->visited = true;

        // Updating the gridview
        int indexCurrent = coordToIndex(nodeCurrent->xCoord, nodeCurrent->yCoord, widthGrid);
        emit updatedScatterGridView(VISIT, indexCurrent);

        // Time and checking for stop from running button
        std::this_thread::sleep_for(std::chrono::milliseconds(speedVisualization));

        // Checking each neighbours
        for (Node* nodeNeighbour: nodeCurrent->neighbours)
        {

            // Updating the list of nodes to test
            // If the neighbour has not been visited and is not an obstacle
            if(!nodeNeighbour->visited && !nodeNeighbour->obstacle)
            {
                // Stopping the algorithm if the goal has been found
                int nextUpIndex = coordToIndex(nodeNeighbour->xCoord, nodeNeighbour->yCoord, widthGrid);

                if (nextUpIndex == gridNodes.endIndex){
                    // Break and go to retrieving path (by emptying nodesToTest)
                    nodesToTest.clear();
                    nodeNeighbour->parent = nodeCurrent;
                    break;
                }else{
                    nodesToTest.push_back(nodeNeighbour);
                    emit updatedScatterGridView(NEXT, nextUpIndex);
                }

            }

            // Neighbours potential lowest parent distance
            float potentialLowerGoal = nodeCurrent->localGoal + distance(nodeCurrent, nodeNeighbour);

            // If choosing to path this node is a lower distance that what currently the neighbours has set
            if (potentialLowerGoal < nodeNeighbour->localGoal){
                // Selecting the current node as the neighbour's parent
                nodeNeighbour->parent = nodeCurrent;
                nodeNeighbour->localGoal = potentialLowerGoal;

            }

        }


    }

    // Checking if the end was reached
    if (nodeEnd->parent != nullptr){

        // Retrieving and plotting the path
        Node* reverseNode = nodeEnd;

        // Line Path
        emit updatedLineGridView(QPointF(reverseNode->xCoord, reverseNode->yCoord), true, true);

        while(reverseNode->parent != nullptr)
        {

            reverseNode = reverseNode->parent;
            int reverseIndex = coordToIndex(reverseNode->xCoord, reverseNode->yCoord, widthGrid);

            // Update the gridView
            emit updatedScatterGridView(PATH, reverseIndex);
            emit updatedLineGridView(QPointF(reverseNode->xCoord, reverseNode->yCoord), true, false);

            // Time and checking for stop from running button
            std::this_thread::sleep_for(std::chrono::milliseconds(speedVisualization));
        }
        emit updatedLineGridView(QPointF(gridNodes.Nodes[gridNodes.startIndex].xCoord, gridNodes.Nodes[gridNodes.startIndex].yCoord), true, false);

    }else{
        endReached = -1;
    }

    emit algorithmCompleted();
}
void PathAlgorithm::performAStarAlgorithm(QPromise<int>& promise)
{

    // Allow to pause and stop the simulation (to debug)
    promise.suspendIfRequested();
    if (promise.isCanceled())
        return;

    // Create connections for each node
    for(Node& node: gridNodes.Nodes)
    {

        // Create connections between nodes (filling the vector neighbours of the Node structure)
        FillNeighboursNode(node);

        // Navigation Graph -> reset all nodes states
        node.globalGoal     = INFINITY;
        node.localGoal      = INFINITY;
        node.parent         = nullptr;
    }

    // Lambda function to compute distance between points
    auto distance = [](Node* a, Node* b)
    {
        return sqrtf(   (a->xCoord - b->xCoord) * (a->xCoord - b->xCoord)
                       +(a->yCoord - b->yCoord) * (a->yCoord - b->yCoord));
    };

    // We could use different heuristics
    auto heuristic = [distance](Node* a, Node* b){return distance(a, b);};
    // heuristic = [](Node* a, Node* b){return 1;};


    // Starting conditions
    Node* nodeStart = &(gridNodes.Nodes[gridNodes.startIndex]);
    Node* nodeEnd = &(gridNodes.Nodes[gridNodes.endIndex]);

    Node* nodeCurrent = &(gridNodes.Nodes[gridNodes.startIndex]);
    nodeStart->localGoal = 0.0f;
    nodeStart->globalGoal = heuristic(nodeStart, nodeEnd);

    std::list<Node*> nodesToTest;
    nodesToTest.push_back(nodeCurrent);

    while(!nodesToTest.empty())
    {
        // Sorting untested nodes by global goal
        nodesToTest.sort([](const Node* a, const Node* b){return a->globalGoal < b->globalGoal;});

        // The lowest goal can also already been visited, so we pop it in this case
        while(!nodesToTest.empty() && nodesToTest.front()->visited)     {   nodesToTest.pop_front();    }

        // Breaking if list is empty
        if (nodesToTest.empty())    {   break;  }

        // Changing current node and setting it to visited
        nodeCurrent = nodesToTest.front();
        nodeCurrent->visited = true;

        // Updating the gridview
        int indexCurrent = coordToIndex(nodeCurrent->xCoord, nodeCurrent->yCoord, widthGrid);
        emit updatedScatterGridView(VISIT, indexCurrent);

        // Time and checking for stop from running button
        std::this_thread::sleep_for(std::chrono::milliseconds(speedVisualization));

        // Checking each neighbours
        for (Node* nodeNeighbour: nodeCurrent->neighbours)
        {
            // Updating the list of nodes to test
            int nextUpIndex = coordToIndex(nodeNeighbour->xCoord, nodeNeighbour->yCoord, widthGrid);

            // If the neighbour has not been visited and is not an obstacle
            if(!nodeNeighbour->visited && !nodeNeighbour->obstacle)
            {
                if (nextUpIndex == gridNodes.endIndex){
                    // Break and go to retrieving path (by emptying nodesToTest)
                    nodesToTest.clear();
                    nodeNeighbour->parent = nodeCurrent;
                    break;

                }else{
                    nodesToTest.push_back(nodeNeighbour);
                    emit updatedScatterGridView(NEXT, nextUpIndex);
                }
            }

            // Neighbours potential lowest parent distance
            float potentialLowerGoal = nodeCurrent->localGoal + distance(nodeCurrent, nodeNeighbour);

            // If choosing to path this node is a lower distance that what currently the neighbours has set
            if (potentialLowerGoal < nodeNeighbour->localGoal)
            {
                // Selecting the current node as the neighbour's parent
                nodeNeighbour->parent = nodeCurrent;
                nodeNeighbour->localGoal = potentialLowerGoal;

                // Since the best path length has changed, we update the neighbour's global score
                nodeNeighbour->globalGoal = nodeNeighbour->localGoal + heuristic(nodeNeighbour, nodeEnd);

            }

        }

    }

    // Checking if the end was reached
    if (nodeEnd->parent != nullptr){

        // Retrieving and plotting the path
        Node* reverseNode = nodeEnd;

        // Line Path
        emit updatedLineGridView(QPointF(reverseNode->xCoord, reverseNode->yCoord), true, true);
        while(reverseNode->parent != nullptr)
        {
            reverseNode = reverseNode->parent;
            int reverseIndex = coordToIndex(reverseNode->xCoord, reverseNode->yCoord, widthGrid);

            // Update the gridView
            emit updatedScatterGridView(PATH, reverseIndex);
            emit updatedLineGridView(QPointF(reverseNode->xCoord, reverseNode->yCoord), true, false);


            // Time and checking for stop from running button
            std::this_thread::sleep_for(std::chrono::milliseconds(speedVisualization));
        }
        emit updatedLineGridView(QPointF(gridNodes.Nodes[gridNodes.startIndex].xCoord, gridNodes.Nodes[gridNodes.startIndex].yCoord), true, false);

    }else{
        endReached = -1;
    }

    emit algorithmCompleted();
}

// Maze generation
void PathAlgorithm::performRecursiveBackTrackerAlgorithm(QPromise<int>& promise)
{

    // Allow to pause and stop the simulation (to debug)
    promise.suspendIfRequested();
    if (promise.isCanceled())
        return;

    // First step - fill with obstacles
    for (int index = 0; index < widthGrid * heightGrid; index++){

        if (index != gridNodes.startIndex || index != gridNodes.endIndex){

            gridNodes.Nodes[index].obstacle = true;
            emit updatedScatterGridView(FREETOOBSTACLE, index);
        }
        // Time and checking for stop from running button
        //std::this_thread::sleep_for(std::chrono::milliseconds(1));

    }

    // stack of visited cells:
    std::stack<Node*> stackUnVisitedNodes;

    // Pushing the first Node in the stack -> we start at a random index
    int randomIndex = rand() % (widthGrid * heightGrid);
    stackUnVisitedNodes.push(&(gridNodes.Nodes[randomIndex]));
    gridNodes.Nodes[randomIndex].visited = true;

    int numberVisitedCells = 1;

    int offset = 2;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    while(numberVisitedCells < widthGrid * heightGrid && !stackUnVisitedNodes.empty() ){

        // Current node from the stack
        Node* currentNode = stackUnVisitedNodes.top();
        int currentIndex = coordToIndex(currentNode->xCoord, currentNode->yCoord, widthGrid);

        std::vector<int> availableNeighbours;

        // east: adding +1 to x:
        int eastIndex = coordToIndex(currentNode->xCoord + offset - 1, currentNode->yCoord, widthGrid);
        int eastIndexOffset = coordToIndex(currentNode->xCoord + offset, currentNode->yCoord, widthGrid);
        if (currentNode->xCoord + offset <= widthGrid && !gridNodes.Nodes[eastIndexOffset].visited){
            availableNeighbours.push_back(0);
        }

        // South: adding -1 to y:
        int southIndex          =   coordToIndex(currentNode->xCoord,currentNode->yCoord - offset + 1, widthGrid);
        int southIndexOffset    =   coordToIndex(currentNode->xCoord,currentNode->yCoord - offset, widthGrid);
        if (currentNode->yCoord - offset >= 1 && !gridNodes.Nodes[southIndexOffset].visited){
            availableNeighbours.push_back(1);
        }
        // West: adding -1 to x:
        int westIndex = coordToIndex(currentNode->xCoord - offset + 1, currentNode->yCoord, widthGrid);
        int westIndexOffset = coordToIndex(currentNode->xCoord - offset, currentNode->yCoord, widthGrid);

        if (currentNode->xCoord - offset >= 1 && !gridNodes.Nodes[westIndexOffset].visited){
            availableNeighbours.push_back(2);
        }

        // north: adding +1 to y:
        int northIndex = coordToIndex(currentNode->xCoord, currentNode->yCoord + offset -1, widthGrid);
        int northIndexOffset = coordToIndex(currentNode->xCoord, currentNode->yCoord + offset, widthGrid);
        if (currentNode->yCoord + offset <= heightGrid && !gridNodes.Nodes[northIndexOffset].visited){
            availableNeighbours.push_back(3);
        }

        // Available neighbours
        if (!(availableNeighbours.empty())){

            // Choosing one random neighbour
            int randomNeighour = availableNeighbours[rand() % availableNeighbours.size()];

            // Creating links from the nodes using the inbuilt neighbours vectors in the Node structure
            switch (randomNeighour) {
            case 0:

                emit updatedScatterGridView(OBSTACLETOFREE, eastIndex);
                emit updatedScatterGridView(OBSTACLETOFREE, eastIndexOffset);
                stackUnVisitedNodes.push(&(gridNodes.Nodes[eastIndexOffset]));

                break;
            case 1:

                emit updatedScatterGridView(OBSTACLETOFREE, southIndex);
                emit updatedScatterGridView(OBSTACLETOFREE, southIndexOffset);

                stackUnVisitedNodes.push(&(gridNodes.Nodes[southIndexOffset]));

               // stackVisitedNodes.push(&(gridNodes.Nodes[southIndex]));

                break;
            case 2:

                emit updatedScatterGridView(OBSTACLETOFREE, westIndex);
                emit updatedScatterGridView(OBSTACLETOFREE, westIndexOffset);

                stackUnVisitedNodes.push(&(gridNodes.Nodes[westIndexOffset]));

                break;
            case 3:

                emit updatedScatterGridView(OBSTACLETOFREE, northIndex);
                emit updatedScatterGridView(OBSTACLETOFREE, northIndexOffset);

                stackUnVisitedNodes.push(&(gridNodes.Nodes[northIndexOffset]));

                break;
            default:
                break;
            }

            numberVisitedCells++;
            gridNodes.Nodes[currentIndex].visited = true;

            // Time and checking for stop from running button
            std::this_thread::sleep_for(std::chrono::milliseconds(speedVisualization));

        }else{

            gridNodes.Nodes[currentIndex].visited = true;

            // If there is no available neighbours, we pop
            if (!stackUnVisitedNodes.empty()){
                stackUnVisitedNodes.pop();
            }else{
                break;
            }


        }

    }

    // We need to reset the visited property to use in the other Algorithms
    for(Node& node: gridNodes.Nodes)    {   node.visited = false;   }

    emit algorithmCompleted();
}


void PathAlgorithm::FillNeighboursNode(Node& node)
{

    // east: adding +1 to x:
    if (node.xCoord + 1 <= widthGrid)
    {
        int eastIndex = coordToIndex(node.xCoord + 1, node.yCoord, widthGrid);
        node.neighbours.push_back(&(gridNodes.Nodes[eastIndex]));
    }

    // South: adding -1 to y:
    if (node.yCoord - 1 >= 1)
    {
        int southIndex = coordToIndex(node.xCoord, node.yCoord -1, widthGrid);
        node.neighbours.push_back(&(gridNodes.Nodes[southIndex]));
    }

    // West: adding -1 to x:
    if (node.xCoord - 1 >= 1)
    {
        int westIndex = coordToIndex(node.xCoord - 1, node.yCoord, widthGrid);
        node.neighbours.push_back(&(gridNodes.Nodes[westIndex]));
    }

    // north: adding +1 to y:
    if (node.yCoord + 1 <= heightGrid)
    {
        int northIndex = coordToIndex(node.xCoord, node.yCoord + 1, widthGrid);
        node.neighbours.push_back(&(gridNodes.Nodes[northIndex]));
    }


    // NorthEst : adding +1 to x and +1 to y
    if (node.xCoord + 1 <= widthGrid && node.yCoord + 1 <= heightGrid)
    {
        int northEstIndex = coordToIndex(node.xCoord + 1, node.yCoord +1, widthGrid);
        node.neighbours.push_back(&(gridNodes.Nodes[northEstIndex]));
    }


    // SouthEst : adding +1 to x and -1 to y
    if (node.xCoord + 1 <= widthGrid && node.yCoord - 1 >= 1)
    {
        int southEstIndex = coordToIndex(node.xCoord + 1, node.yCoord -1, widthGrid);
        node.neighbours.push_back(&(gridNodes.Nodes[southEstIndex]));
    }

    // SouthWest : adding -1 to x and -1 to y
    if (node.xCoord - 1 >= 1 && node.yCoord - 1 >= 1)
    {
        int southWestIndex = coordToIndex(node.xCoord - 1, node.yCoord - 1, widthGrid);
        node.neighbours.push_back(&(gridNodes.Nodes[southWestIndex]));
    }


    // NorthWest: adding -1 to x and +1 to y
    if (node.xCoord - 1 >= 1 && node.yCoord + 1 <= heightGrid)
    {
        int northWestIndex = coordToIndex(node.xCoord - 1, node.yCoord + 1, widthGrid);
        node.neighbours.push_back(&(gridNodes.Nodes[northWestIndex]));
    }
}
