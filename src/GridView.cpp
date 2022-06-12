#include <iostream>
#include <QtCore/QtMath>
#include <QtCore/QDebug>
#include <cmath>
#include <QMessageBox>
#include <QEventLoop>
#include "headers/GridView.h"

#include <queue>
#include <unistd.h>

// Constructor
GridView::GridView(QChartView* parent): QChartView(parent)
{
    std::cerr << "Grid View constructor \n";

    //Initialize QChart
    chart = new QChart();

    // New Series of Elements: free, obstacle, seen, start and goal
    freeElements = new QScatterSeries();
    obstacleElements = new QScatterSeries();
    visitedElements = new QScatterSeries();
    startElement = new QScatterSeries();
    endElement = new QScatterSeries();
    currentElement = new QScatterSeries();

    // Inserting points in the series
    for (int i = 0; i < heightGrid * widthGrid; i++)
    {
        freeElements->append(QPoint());
        obstacleElements->append(QPoint());
        visitedElements->append(QPoint());
    }

    // Start, goal and current elements
    startElement->append(QPoint());
    endElement->append(QPoint());
    currentElement->append(QPointF());

    // Setting up current objects
    currentInteraction = NOINTERACTION;
    currentArrangement = EMPTY;
    currentAlgorithm = NOALGO;
    currentState = false;
}

// Destructor
GridView::~GridView()
{
    std::cerr << "Destroying Grid View \n";
    std::cerr << "Backend Grid: \n" <<
                 "Start index: " << gridNodes.startIndex << "\n" <<
                 "End index: " << gridNodes.endIndex << "\n";

    delete freeElements;
    delete obstacleElements;
    delete visitedElements;
    delete startElement;
    delete endElement;
    delete currentElement;
}

// Setter: currentInteraction set up with index
void GridView::setCurrentInteraction(int index)
{
    currentInteraction = static_cast<INTERACTIONS>(index);
}

// Setter: currentInteraction set up with enum
void GridView::setCurrentInteraction(INTERACTIONS interaction)
{
    currentInteraction = interaction;
}

// Setter: current state
void GridView::setCurrentState(bool state)
{
    currentState = state;
}

// Setter: currentAlgorithm
void GridView::setCurrentAlgorithm(int index)
{
    currentAlgorithm = static_cast<ALGOS>(index);
}


INTERACTIONS GridView::getCurrentInteraction() const
{
    return currentInteraction;
}

// Getter: currentArrangement
ARRANGEMENTS GridView::getCurrentArrangement() const
{
    return currentArrangement;
}

// Getter: grid of nodes
grid& GridView::getGrid()
{
    return gridNodes;
}

// Getter: current Algorithm for the main window
ALGOS GridView::getCurrentAlgorithm() const
{
    return currentAlgorithm;
}

// Getter current state
bool GridView::getCurrentState() const
{
    return currentState;
}

// Getter: height grid
int GridView::getHeightGrid() const
{
  return heightGrid;

}

void GridView::populateGridMap(ARRANGEMENTS arrangement)
{
    std::cerr << "Populating the grid in the chart \n";
    if (arrangement == EMPTY)
    {
        // Setting Default start and end points
        startElement->replace(0, QPointF(1, heightGrid));
        endElement->replace(0, QPointF(widthGrid, 1));

        qreal x{1};
        // Index for gridPoint Vector
        int indexGrid{};

        // Populating the grid with elements
        for (int i=1;  i <= this->widthGrid; i++){

            qreal y{1};
            for (int j=1;  j <= this->heightGrid; j++)
            {
                if (i == startElement->points()[0].x() && j == startElement->points()[0].y()){

                    // Updating the backend grid: starting Element
                    gridNodes.startIndex = indexGrid;

                }else if (i == endElement->points()[0].x() && j == endElement->points()[0].y()){

                    // Updating the backend grid: ending Element
                    gridNodes.endIndex = indexGrid;

                }else{

                    // Populating the QScatter Series of free Elements
                    freeElements->replace(indexGrid, QPointF(x, y));
                }

                // Grid Point: updating coordinated, already initialized as visited == false
                Node gridNodeBackend;
                gridNodeBackend.xCoord = int(x);
                gridNodeBackend.yCoord = int(y);

                // Populating the backend grid with the point (including start and end)
                gridNodes.Nodes.push_back(gridNodeBackend);

                y++;
                indexGrid++;

                // Deleting the obstacle if present
                if (gridNodes.Nodes[indexGrid].obstacle == true)
                {
                    obstacleElements->replace(indexGrid, QPointF());
                }
            }
            x++;
        }
    }else{

        std::cerr << "BAD ARRANGEMENT \n";
    }

    // Setting up the current index
    gridNodes.currentIndex = gridNodes.startIndex;
}

// Creating the QChart
QChart* GridView::createChart()
{
    // Populate grid
    GridView::populateGridMap(currentArrangement);

    // Render
    setRenderHint(QPainter::Antialiasing);

    // Set background color
    //chart->setBackgroundBrush(Qt::SolidPattern);

    // Set marker shape
    freeElements->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    obstacleElements->setMarkerShape(QScatterSeries::MarkerShapePentagon);
    visitedElements->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    startElement->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    endElement->setMarkerShape(QScatterSeries::MarkerShapeStar);
    currentElement->setMarkerShape(QScatterSeries::MarkerShapeRectangle);

    // Set marker size
    freeElements->setMarkerSize(markerSize);
    obstacleElements->setMarkerSize(markerSize);
    visitedElements->setMarkerSize(markerSize);
    startElement->setMarkerSize(markerSize);
    endElement->setMarkerSize(markerSize);
    currentElement->setMarkerSize(markerSize);

    // Current QScatter Series point not visible until start of run
    currentElement->setPointsVisible(false);

    // Set Marker color
    obstacleElements->setColor(QColorConstants::Gray);
    currentElement->setColor(QColorConstants::Red);

    // Adding Series in the chart
    chart->addSeries(freeElements);
    chart->addSeries(obstacleElements);
    chart->addSeries(visitedElements);
    chart->addSeries(startElement);
    chart->addSeries(endElement);
    chart->addSeries(currentElement);

    // Chart axis
    chart->createDefaultAxes();
    QList<QAbstractAxis*> xAxis = chart->axes(Qt::Horizontal);
    QList<QAbstractAxis*> yAxis = chart->axes(Qt::Vertical);
    xAxis.first()->setRange(0, this->widthGrid + 1);
    yAxis.first()->setRange(0, this->heightGrid + 1);

    // Setting name of the elements
    freeElements->setName("Free nodes");
    obstacleElements->setName("Obstacle nodes");
    visitedElements->setName("Visited nodes");
    startElement->setName("Start");
    endElement->setName("Goal");
    currentElement->setName("Current node");

    // Create legends
    chart->legend()->setMarkerShape(QLegend::MarkerShapeFromSeries);

    // Connecting signals
    connect(freeElements, &QScatterSeries::clicked, this, &GridView::handleClickedPoint);
    connect(obstacleElements, &QScatterSeries::clicked, this, &GridView::handleClickedPoint);
    return chart;

}

void GridView::handleClickedPoint(const QPointF& point)
{
    // Copy of the point
    QPointF clickedPoint = point;
    std::cerr << "clickedPoint: (" << clickedPoint.x() << ", " << clickedPoint.y() << ")\n" ;

    // Null QPoint
    QPointF nullQPoint = QPointF();

    // ClosestIndexPos
    int clickedIndex = coordToIndex(clickedPoint, heightGrid);
    std::cerr << "clickedIndex: " << clickedIndex << "\n";

    // If the user choose to insert obstacles
    if (currentInteraction == OBSTACLE)
    {
        // if the clicked point is free
        if (gridNodes.Nodes[clickedIndex].obstacle == false)
        {
            // Creating obstacle
            obstacleElements->replace(clickedIndex, clickedPoint);
            freeElements->replace(clickedPoint, nullQPoint);

            // Updating point as an obstacle in backend grid
            gridNodes.Nodes[clickedIndex].obstacle = true;

        } else // the clicked point is an obstacle
        {
            // Deleting obstacle
            freeElements->replace(clickedIndex, clickedPoint);
            obstacleElements->replace(clickedPoint, nullQPoint);

            // Updating point as a free element in the backend grid
            gridNodes.Nodes[clickedIndex].obstacle = false;
        }

    } else if (currentInteraction == START)
    {
        // Saving the previous starting point index
        int previousStartGridIndex = gridNodes.startIndex;
        QPointF previousStartElement = startElement->points()[0];

        std::cerr << "previousStartIndex: " << previousStartGridIndex << "\n";
        std::cerr << "previousStartElement: (" << previousStartElement.x() << ", " << previousStartElement.y() << ")\n";

        // Updating Starting point in backend grid
        gridNodes.startIndex = clickedIndex;

        // if the clicked point is free
        if (gridNodes.Nodes[clickedIndex].obstacle == false)
        {
            // Modyfing StartElement QScatter Series
            startElement->replace(0, clickedPoint);
            freeElements->replace(clickedPoint, previousStartElement);

            // Making sure the previous point is set as free in the backend grid
            gridNodes.Nodes[previousStartGridIndex].obstacle = false;

        } else  // the clicked point is an obstacle
        {
            std::cerr << "currentInteraction == START - clickedPoint is an obstacle\n";

            // We add the starting point here and the previous start becomes an obstacle
            startElement->replace(0, clickedPoint);
            obstacleElements->replace(clickedPoint, previousStartElement);

            // Making sure the point is set as an obstacle in the backend grid
            gridNodes.Nodes[previousStartGridIndex].obstacle = true;
        }
        std::cerr << "new start element: (" << startElement->points()[0].x() << ", " << startElement->points()[0].y() << ")\n";

    } else if (currentInteraction == END)
    {

        // Saving the previous ending point index
        int previousEndGridIndex = gridNodes.endIndex;
        QPointF previousEndElement = endElement->points()[0];

        std::cerr << "previousEndIndex: " << previousEndGridIndex << "\n";
        std::cerr << "previousEndElement: (" << previousEndElement.x() << ", " << previousEndElement.y() << ")\n";

        // Updating ending point in backend grid
        gridNodes.endIndex = clickedIndex;

        // if the clicked point is free
        if (gridNodes.Nodes[clickedIndex].obstacle == false)
        {
            std::cerr << "currentInteraction == END - clickedPoint is free \n";

            // We add the ending point here and the previous end becomes a free element
            endElement->replace(0, clickedPoint);
            freeElements->replace(clickedPoint, previousEndElement);

            // Making sure the previous end point is set as free in the backend grid
            gridNodes.Nodes[previousEndGridIndex].obstacle = false;


        } else  // the clicked point is an obstacle
        {
            // We add the ending point here and the previous end becomes a free element
            endElement->replace(previousEndElement, clickedPoint);
            obstacleElements->replace(clickedPoint, previousEndElement);

            // Making sure the previous end point is set as obstacle in the backend grid
            gridNodes.Nodes[previousEndGridIndex].obstacle = true;
        }
        std::cerr << "new end element: (" << endElement->points()[0].x() << ", " << endElement->points()[0].y() << ")\n";

    } else if (currentInteraction == NOINTERACTION)
    {
        QMessageBox::information(this, "Information", "Please select an interaction type");

    } else if (currentState == true)
    {
        QMessageBox::information(this, "Information", "Please stop the simulation first");
    }

}


qreal GridView::computeDistanceBetweenPoints(const QPointF& pointA, const QPointF& pointB)
{
    return qSqrt(std::pow(pointA.x() - pointB.x(), 2)
                 + std::pow(pointA.y() - pointB.y(), 2));
}

int coordToIndex(const QPointF& point, int heightGrid)
{
    return (point.x() - 1) * heightGrid + point.y() - 1;
}

int coordToIndex(int x, int y,  int heightGrid)
{
    return (x - 1) * heightGrid + y - 1;
}


void GridView::AlgorithmView(bool on)
{
    if (on)
    {
        currentElement->setPointsVisible(true);
    }else{
        currentElement->setPointsVisible(false);

    }

    //QtConcurrent::run(this, &GridView::launchingEventBFS);

}


////grid* GridView::launchingEventBFS(grid gridNodes)
//void GridView::launchingEventBFS()
//{
//    // Reach the goal
//    bool reachEnd = false;

//    // Initialize the queue of Nodes to visit in the next step
//    std::queue<Node> nextNodes;

//    // Initializing a vector of nodes (through copy of the original): to keep track of parents: index: indexparent, value: Node child
//    std::vector<Node> parentNodes = gridNodes.Nodes;

//    // Starting point
//    nextNodes.push(gridNodes.Nodes[gridNodes.startIndex]);

//    // Keeping track of the number of nodes left to check in the next layer and in the current layer
//    int nodesInNextLayer = 0;
//    int nodesLeftInCurrentLayer =1 ;

//    // Counting the optimal number of moves needed to go from start to finish
//    int moveCount {};

//    while(!nextNodes.empty())
//    {
//        // Current Node
//        Node currentNode =  nextNodes.front(); nextNodes.pop();
//        int currentIndex = coordToIndex(currentNode.xCoord, currentNode.yCoord, heightGrid);

//        // Update current in the gridView
//        //updateGridView(CURRENT, currentIndex);

//        if (currentIndex == gridNodes.endIndex)
//        {
//            reachEnd = true;
//            break;
//        }

//        if (currentNode.visited == false || currentNode.obstacle == true)
//        {
//            // This node is now visited
//            currentNode.visited = true;

//            // Update this node as visited in the gridView
//            emit updatedgridView(VISIT, currentIndex);

//            // Retrieve neighbors and pushing it to the next nodes to check
//            std::vector<Node> neighbors = retrieveNeighborsGrid(&gridNodes, currentNode, heightGrid);
//            for (auto nextNode=neighbors.begin(); nextNode < neighbors.end(); nextNode++)
//            {
//                // Pushing the neighbors in the next nodes to be checked
//                nextNodes.push(*nextNode);

//                // Keeping track of the number of nodes in the next layers left to check
//                nodesInNextLayer++;

//                //Keeping track of parent node
//                int parentIndex = coordToIndex((*nextNode).xCoord, (*nextNode).yCoord, heightGrid);
//                parentNodes[parentIndex] = currentNode;
//            }

//            // This node has been visited
//            nodesLeftInCurrentLayer--;

//        }

//        // if all nodes in the current layer have been checked
//        if (nodesLeftInCurrentLayer == 0)
//        {
//            nodesLeftInCurrentLayer = nodesInNextLayer;
//            nodesInNextLayer = 0;
//            moveCount++;
//        }

//        std::cerr << "MOVE COUNT: " << moveCount << "\n";
//        std::cerr << "CURRENT NODE INDEX: " <<currentIndex << "\n";

//        // Time and checking for stop from running button
//        sleep(1);
//        std::cerr << "Current state " << std::boolalpha << currentState << "\n";

//    }

//    // Return -1 if goal not reached

//    // Reversing the path -> needs to clear the gridview first

//    // Using the seen nodes and a line to indicate the path

//    //return &gridNodes;
//}


// Updating the view (for the path planning algorithms)
bool GridView::handleUpdatedgridView(UPDATETYPES updateType, int updateIndex)
{
    // Return True when this is done

    if (updateType == CURRENT)
    {
        currentElement->setPointsVisible();
        if (updateIndex == gridNodes.startIndex){

            // Retrieving the current point coordinates
            QPointF currentPoint = startElement->points()[0];

            startElement->replace(currentPoint, QPointF());
            currentElement->replace(0, currentPoint);
        }
        else if (gridNodes.Nodes[updateIndex].obstacle == false){ // Current node is free

            // Retrieving the current point coordinates
            QPointF currentPoint = freeElements->points()[updateIndex];
            freeElements->replace(updateIndex, QPointF());
            currentElement->replace(0, currentPoint);


        }else{  // Current node is an obstacle

            // Retrieving the current point coordinates
            QPointF currentPoint = obstacleElements->points()[updateIndex];
            obstacleElements->replace(updateIndex, QPointF());
            currentElement->replace(0, currentPoint);
        }


    }else if(updateType == VISIT)
    {
        // Current node is (must be) free
        QPointF visitedPoint = freeElements->points()[updateIndex];

        freeElements->replace(updateIndex, QPointF());
        visitedElements->replace(updateIndex, visitedPoint);
    }

    update();
    std::cerr << "View updated \n";

    return true;

}
//std::vector<Node> GridView::retrieveNeighborsGrid(const grid* gridNodes, const Node& currentNode, int heightGrid)
//{
//    std::vector<Node> neighbors;

//    // rigth: adding +1 to x:
//    if (currentNode.xCoord + 1 <= gridNodes->Nodes[-1].xCoord)
//    {
//        int rightIndex = coordToIndex(currentNode.xCoord + 1, currentNode.yCoord, heightGrid);
//        neighbors.push_back(gridNodes->Nodes[rightIndex]);
//    }

//    // down: adding -1 to y:
//    if (currentNode.yCoord - 1 >= gridNodes->Nodes[0].yCoord)
//    {
//        int downIndex = coordToIndex(currentNode.xCoord, currentNode.yCoord -1, heightGrid);
//        neighbors.push_back(gridNodes->Nodes[downIndex]);
//    }

//    // rigth: adding -1 to x:
//    if (currentNode.xCoord - 1 >= gridNodes->Nodes[0].xCoord)
//    {
//        int leftIndex = coordToIndex(currentNode.xCoord - 1, currentNode.yCoord, heightGrid);
//        neighbors.push_back(gridNodes->Nodes[leftIndex]);
//    }

//    // up: adding +1 to y:
//    if (currentNode.yCoord + 1 <= gridNodes->Nodes[-1].yCoord)
//    {
//        int upIndex = coordToIndex(currentNode.xCoord, currentNode.yCoord + 1, heightGrid);
//        neighbors.push_back(gridNodes->Nodes[upIndex]);
//    }

//    return neighbors;
//}
