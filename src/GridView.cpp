#include <iostream>
#include <QtCore/QtMath>
#include <QtCore/QDebug>
#include <cmath>
#include <QMessageBox>
#include <QEventLoop>
#include "headers/GridView.h"
#include <QCategoryAxis>
#include <queue>
#include <unistd.h>
#include <QTest>

// Constructor
GridView::GridView(int widthGrid, int heightGrid, int markerSize, QChartView* parent): QChartView(parent)
{
    // Default dimensions
    this->widthGrid = widthGrid;
    this->heightGrid = heightGrid;
    this->markerSize = markerSize;

    //Initialize QChart
    chart = new QChart();
    chart->setBackgroundVisible(true);

    // New Series of scatter elements
    freeElements        = new QScatterSeries();
    obstacleElements    = new QScatterSeries();
    visitedElements     = new QScatterSeries();
    nextElements        = new QScatterSeries();
    pathElements        = new QScatterSeries();
    startElement        = new QScatterSeries();
    endElement          = new QScatterSeries();

    // Start, goal elements
    startElement    ->append(QPoint());
    endElement      ->append(QPoint());

    // Line series
    pathLine      = new QLineSeries();

    // Setting up current objects
    currentInteraction  = NOINTERACTION;
    currentArrangement  = EMPTY;
    currentAlgorithm    = NOALGO;
    simulationRunning   = false;

    // Initializing gridNode
    grid gridNodes;

}

// Destructor
GridView::~GridView()
{
    std::cerr << "Destroying Grid View \n";
    std::cerr << "Backend Grid: \n" <<
                 "Start: (" << gridNodes.Nodes[gridNodes.startIndex].xCoord << ", " << gridNodes.Nodes[gridNodes.startIndex].yCoord << "): "
                            <<gridNodes.startIndex << "\n" <<
                 "End: (" << gridNodes.Nodes[gridNodes.endIndex].xCoord << ", " << gridNodes.Nodes[gridNodes.endIndex].yCoord << "): "
                            <<gridNodes.endIndex << "\n";

    delete freeElements;
    delete obstacleElements;
    delete visitedElements;
    delete nextElements;
    delete startElement;
    delete endElement;
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
void GridView::setSimulationRunning(bool state)
{
    simulationRunning = state;
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
bool GridView::getSimulationRunning() const
{
    return simulationRunning;
}

// Getter: height grid
int GridView::getHeightGrid() const
{
  return heightGrid;

}

void GridView::populateGridMap(ARRANGEMENTS arrangement, bool reset)
{

    if (simulationRunning)
    {
        QMessageBox::information(this, "Information", "Please stop the simulation first");
    }

    if (reset){
        // Removing all points
        freeElements    ->removePoints(0, freeElements      ->points().size());
        obstacleElements->removePoints(0, obstacleElements  ->points().size());
        visitedElements ->removePoints(0, visitedElements   ->points().size());
        nextElements    ->removePoints(0, nextElements      ->points().size());
        pathElements    ->removePoints(0, pathElements      ->points().size());
        pathLine        ->removePoints(0, pathLine          ->points().size());

        // Remove nodes from gridNodes
        gridNodes.Nodes.clear();

        // Modifying
        chart->axes(Qt::Horizontal).first() ->setRange(qreal(0.4), qreal(this->widthGrid  + 0.5));
        chart->axes(Qt::Vertical).first()   ->setRange(qreal(0.4), qreal(this->heightGrid + 0.5));

    }

    // Inserting points in the series
    for (int i = 0; i < heightGrid * widthGrid; i++)
    {
        freeElements->append(QPoint());
        obstacleElements->append(QPoint());
        visitedElements->append(QPoint());
        nextElements->append(QPoint());
        pathElements->append(QPoint());
    }


    // Creating the right number of nodes in the grid (should avoid push_back after initialization)
    Node gridNodeBackend;
    for (int i{}; i < widthGrid * heightGrid; i++){gridNodes.Nodes.push_back(gridNodeBackend);}

    if (arrangement == EMPTY)
    {
        // Setting Default start and end points
        startElement    ->replace(0, QPointF(1, heightGrid));
        endElement      ->replace(0, QPointF(widthGrid, 1));

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

                // Deleting the obstacle if present (useful for reset)
                if (gridNodes.Nodes[indexGrid].obstacle == true)
                {
                    obstacleElements->replace(indexGrid, QPointF());
                }

                // Grid Point: updating coordinates
                Node gridNodeBackend;
                gridNodeBackend.xCoord = int(x);
                gridNodeBackend.yCoord = int(y);

                // Populating the backend grid with the point (including start and end)
                gridNodes.Nodes[indexGrid] = gridNodeBackend;

                // Reset of properties
                gridNodes.Nodes[indexGrid].obstacle = false;
                gridNodes.Nodes[indexGrid].visited = false;
                gridNodes.Nodes[indexGrid].nextUp = false;

                // Reset of Elements
                visitedElements ->replace(indexGrid, QPointF());
                nextElements    ->replace(indexGrid, QPointF());
                pathElements    ->replace(indexGrid, QPointF());

                x++;
                indexGrid++;

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
    GridView::populateGridMap(currentArrangement, false);

    // Render
    setRenderHint(QPainter::Antialiasing);


    // Set marker shape
    freeElements    ->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    obstacleElements->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    visitedElements ->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    nextElements    ->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    pathElements    ->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    startElement    ->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    endElement      ->setMarkerShape(QScatterSeries::MarkerShapeRectangle);

    // Set marker size
    setElementsMarkerSize();

    // Label Points
//    freeElements    ->setPointLabelsVisible();
//    obstacleElements->setPointLabelsVisible();
//    visitedElements ->setPointLabelsVisible();
//    nextElements    ->setPointLabelsVisible();
//    startElement    ->setPointLabelsVisible();
//    endElement      ->setPointLabelsVisible();

    // Set Marker color
    //freeElements    ->setColor(QColorConstants::White);
    obstacleElements->setColor(QColorConstants::Black);
    visitedElements ->setColor(QColorConstants::DarkGreen);
    nextElements    ->setColor(QColorConstants::DarkYellow);
    pathElements    ->setColor(QColorConstants::Red);
    startElement    ->setColor(QColorConstants::DarkBlue);
    endElement      ->setColor(QColorConstants::DarkRed);

    // Set opacity
    freeElements    ->setOpacity(qreal(0.2));
    visitedElements ->setOpacity(qreal(0.75));
    nextElements    ->setOpacity(qreal(0.75));
    pathElements    ->setOpacity(qreal(0.75));
    startElement    ->setOpacity(qreal(0.95));
    endElement      ->setOpacity(qreal(0.95));

    // Set marker border color
    freeElements    ->setBorderColor(QColorConstants::Black);
    obstacleElements->setBorderColor(QColorConstants::Black);
    visitedElements ->setBorderColor(QColorConstants::Black);
    nextElements    ->setBorderColor(QColorConstants::Black);
    pathElements    ->setBorderColor(QColorConstants::Black);
    startElement    ->setBorderColor(QColorConstants::Black);
    endElement      ->setBorderColor(QColorConstants::Black);

    // Adding Series in the chart
    chart->addSeries(freeElements);
    chart->addSeries(obstacleElements);
    chart->addSeries(visitedElements);
    chart->addSeries(nextElements);
    chart->addSeries(pathElements);
    chart->addSeries(startElement);
    chart->addSeries(endElement);
    chart->addSeries(pathLine);

    // Setting name of the elements
    startElement    ->setName("Start");
    endElement      ->setName("Goal");
    freeElements    ->setName("Free");
    obstacleElements->setName("Obstacles");
    visitedElements ->setName("Visited");
    nextElements    ->setName("Next");
    pathElements    ->setName("Path");

    // Not visible until start of simulation
    visitedElements->setPointsVisible(false);
    nextElements->setPointsVisible(false);
    pathElements->setPointsVisible(false);

    // Create legends
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->setMarkerShape(QLegend::MarkerShapeFromSeries);

    // Customize chart background
    QLinearGradient backgroundGradient;
    backgroundGradient.setStart(QPointF(0, 0));
    backgroundGradient.setFinalStop(QPointF(0, 1));
    backgroundGradient.setColorAt(0.0, QRgb(0xd2d0d1));
    backgroundGradient.setColorAt(1.0, QRgb(0x4c4547));
    backgroundGradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    chart->setBackgroundBrush(backgroundGradient);

    // Chart axis
    chart->createDefaultAxes();
    chart->setPlotAreaBackgroundVisible(false);

    QList<QAbstractAxis*> axisX = chart->axes(Qt::Horizontal);
    QList<QAbstractAxis*> axisY = chart->axes(Qt::Vertical);

    axisX.first()->setRange(qreal(0.4), qreal(this->widthGrid  + 0.4));
    axisY.first()->setRange(qreal(0.4), qreal(this->heightGrid + 0.5));

    // Customize grid lines
    axisX.first()->setGridLineVisible(false);
    axisY.first()->setGridLineVisible(false);

    // Hide axes
    axisX.first()->setVisible(true);
    axisX.first()->setVisible(true);

    // Connecting signals
    connect(freeElements, &QScatterSeries::clicked, this, &GridView::handleClickedPoint);
    connect(obstacleElements, &QScatterSeries::clicked, this, &GridView::handleClickedPoint);

    return chart;

}

void GridView::setElementsMarkerSize()
{
    // Set marker size of all elements
    freeElements    ->setMarkerSize(markerSize);
    obstacleElements->setMarkerSize(markerSize);
    visitedElements ->setMarkerSize(markerSize);
    nextElements    ->setMarkerSize(markerSize);
    pathElements    ->setMarkerSize(markerSize);
    startElement    ->setMarkerSize(markerSize);
    endElement      ->setMarkerSize(markerSize);
}

void GridView::handleClickedPoint(const QPointF& point)
{
    // Copy of the point
    QPointF clickedPoint = point;
    std::cerr << "clickedPoint: (" << clickedPoint.x() << ", " << clickedPoint.y() << ")\n" ;

    // Null QPoint
    QPointF nullQPoint = QPointF();

    // ClosestIndexPos
    int clickedIndex = coordToIndex(clickedPoint, widthGrid);
    std::cerr << "clickedIndex: " << clickedIndex << "\n";

    // If the user choose to insert obstacles
    if (currentInteraction == OBST)
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
        int     previousStartGridIndex  = gridNodes.startIndex;
        QPointF previousStartElement    = startElement->points()[0];

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

    } else if (simulationRunning == true)
    {
        QMessageBox::information(this, "Information", "Please stop the simulation first");
    }

}


qreal GridView::computeDistanceBetweenPoints(const QPointF& pointA, const QPointF& pointB)
{
    return qSqrt(   std::pow(pointA.x() - pointB.x(), 2)
                  + std::pow(pointA.y() - pointB.y(), 2));
}

int coordToIndex(const QPointF& point, int widthGrid)
{
    return (point.y() - 1) * widthGrid + point.x() - 1;
}

int coordToIndex(int x, int y,  int widthGrid)
{
    return (y - 1) * widthGrid + x - 1;
}


void GridView::AlgorithmView(bool on)
{
    if (on){
        nextElements    ->setPointsVisible(true);
        pathElements    ->setPointsVisible(true);
        visitedElements ->setPointsVisible(true);


    }else{
//        nextElements    ->setPointsVisible(false);
//        visitedElements ->setPointsVisible(false);
//        pathElements    ->setPointsVisible(false);

    }

}


// Updating the view (for the path planning algorithms)
bool GridView::handleUpdatedScatterGridView(UPDATETYPES updateType, int updateIndex)
{

    switch (updateType) {
    case VISIT:
        replaceNextbyVisited(updateIndex);  break;
    case NEXT:
        replaceFreebyNext(updateIndex);     break;
    case PATH:
        replaceVisitedbyPath(updateIndex);  break;
    case FREE:
        replaceNextbyFree(updateIndex);     break;
    case FREETOOBSTACLE:
        replaceFreebyObstacle(updateIndex); break;
    case OBSTACLETOFREE:
        replaceObstaclebyFree(updateIndex); break;
    default:
        break;
    }
    return true;
}
bool GridView::handleUpdatedLineGridView(QPointF updatePoint, bool addingPoint, bool clearPriorToUpdate)
{
    if (clearPriorToUpdate){
        pathLine->clear();
    }
    updateLine(updatePoint, addingPoint);

    return true;
}

// Replacing functions

void GridView::replaceFreebyVisited(int updateIndex)
{
    // Current node is free
    QList<QPointF> freeElementsPoints = freeElements->points();
    QPointF visitedPoint = freeElementsPoints[updateIndex];

    if (visitedPoint !=QPointF())
    {
    freeElements->replace(updateIndex, QPointF());
    visitedElements->replace(updateIndex, visitedPoint);
    }
}

void GridView::replaceFreebyObstacle(int updateIndex)
{
    // Current node is free
    QList<QPointF> freeElementsPoints = freeElements->points();
    QPointF obstaclePoint = freeElementsPoints[updateIndex];

    if (obstaclePoint !=QPointF()){
    freeElements->replace(updateIndex, QPointF());
    obstacleElements->replace(updateIndex, obstaclePoint);

    gridNodes.Nodes[updateIndex].obstacle = true;

    }

}

void GridView::replaceObstaclebyFree(int updateIndex)
{
    // Current node is obstacle
    QList<QPointF> obstacleElementsPoints = obstacleElements->points();
    QPointF obstaclePoint = obstacleElementsPoints[updateIndex];

    if (obstaclePoint != QPointF()){
        obstacleElements    ->replace(updateIndex, QPointF());
        freeElements        ->replace(updateIndex, obstaclePoint);
        gridNodes.Nodes[updateIndex].obstacle = false;

    }else{
        std::cerr <<"No points \n";
    }
}

void GridView::replaceNextbyVisited(int updateIndex)
{
    // Current node is next
    QList<QPointF> nextElementsPoints = nextElements->points();
    QPointF visitedPoint = nextElementsPoints[updateIndex];

    if (visitedPoint !=QPointF())
    {
    nextElements->replace(updateIndex, QPointF());
    visitedElements->replace(updateIndex, visitedPoint);
    }
}

void GridView::replaceVisitedbyPath(int updateIndex)
{
    // Current node is visited
    QList<QPointF> visitedElementsPoints = visitedElements->points();
    QPointF pathPoint = visitedElementsPoints[updateIndex];

    if (pathPoint !=QPointF())
    {
    visitedElements ->replace(updateIndex, QPointF());
    pathElements    ->replace(updateIndex, pathPoint);
    }
}

void GridView::replaceFreebyNext(int updateIndex)
{
    // Current node is free
    QList<QPointF> freeElementsPoints = freeElements->points();
    QPointF nextPoint = freeElementsPoints[updateIndex];

    if (nextPoint !=QPointF()){
        freeElements->replace(updateIndex, QPointF());
        nextElements->replace(updateIndex, nextPoint);
    }else{
        //std::cerr <<"No points \n";
    }
}

void GridView::replaceNextbyFree(int updateIndex)
{
    // Current node is next
    QList<QPointF> nextElementsPoints = nextElements->points();
    QPointF freePoint = nextElementsPoints[updateIndex];

    if (freePoint != QPointF()){
        nextElements    ->replace(updateIndex, QPointF());
        freeElements    ->replace(updateIndex, freePoint);
    }else{
       // std::cerr <<"No points \n";
    }
}

void GridView::updateLine(QPointF updatePoint, bool addingPoint)
{
    if (addingPoint){
        pathLine->append(updatePoint);
    }else{
        pathLine->replace(pathLine->points().size() - 1, updatePoint);
    }
}
