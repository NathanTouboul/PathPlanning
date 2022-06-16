#include <iostream>
#include <QtCore/QtMath>
#include <QtCore/QDebug>
#include <cmath>
#include <QMessageBox>
#include <QEventLoop>
#include "headers/GridView.h"

#include <queue>
#include <unistd.h>
#include <QTest>

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

        // Index for gridPoint Vector
        int indexGrid{};

        qreal y{1};
        // Populating the grid with elements
        for (int j=1;  j <= this->heightGrid; j++)
        {

            qreal x{1};
            for (int i=1;  i <= this->widthGrid; i++)
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

                x++;
                indexGrid++;

                // Deleting the obstacle if present (useful for reset)
                if (gridNodes.Nodes[indexGrid].obstacle == true)
                {
                    obstacleElements->replace(indexGrid, QPointF());
                }

            }
            y++;
        }
    }else{

        std::cerr << "BAD ARRANGEMENT \n";
    }

    // Setting up the current index
    gridNodes.currentIndex = gridNodes.startIndex;

    // Check size of vector
    std::cerr << "\nNumber of nodes in gridNodes: " << gridNodes.Nodes.size() << " vs " << this->heightGrid * this->widthGrid << "\n";

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

    // Label Points
    freeElements->setPointLabelsVisible();
    obstacleElements->setPointLabelsVisible();
    visitedElements->setPointLabelsVisible();
    startElement->setPointLabelsVisible();
    endElement->setPointLabelsVisible();
    currentElement->setPointLabelsVisible();

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
    if (on){
        currentElement->setPointsVisible(true);
    }else{
        currentElement->setPointsVisible(false);
    }

}


// Updating the view (for the path planning algorithms)
bool GridView::handleUpdatedgridView(UPDATETYPES updateType, int updateIndex)
{
    // Return True when this is done

    if (updateType == CURRENT)
    {
        currentElement->setPointsVisible();
        if (updateIndex == gridNodes.startIndex){                   // Current node is the start point

            // Retrieving the current point coordinates
            QList<QPointF> startElementsPoints = startElement->points();
            QPointF currentPoint = startElementsPoints[0];

            startElement->replace(currentPoint, QPointF());
            currentElement->replace(0, currentPoint);
        }

        if (gridNodes.Nodes[updateIndex].visited == false)
        {

            if (gridNodes.Nodes[updateIndex].obstacle == false){   // Current node is free

                // Retrieving the current point coordinates
                QList<QPointF> freeElementsPoints = freeElements->points();
                QPointF currentPoint = freeElementsPoints[updateIndex];

                freeElements->replace(updateIndex, QPointF());
                currentElement->replace(0, currentPoint);

            }else{  // Current node is an obstacle

                // Retrieving the current point coordinates
                QList<QPointF> obstacleElementsPoints = obstacleElements->points();
                QPointF currentPoint = obstacleElementsPoints[updateIndex];

                obstacleElements->replace(updateIndex, QPointF());
                currentElement->replace(0, currentPoint);
            }

        }


    }else if(updateType == VISIT)
    {
        // Current node is (must be) free
        QList<QPointF> freeElementsPoints = freeElements->points();
        QPointF visitedPoint = freeElementsPoints[updateIndex];

        freeElements->replace(updateIndex, QPointF());
        visitedElements->replace(updateIndex, visitedPoint);
    }

    update();

    return true;

}
