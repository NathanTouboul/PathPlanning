#include <iostream>
#include <QtCore/QtMath>
#include <QtCore/QDebug>
#include <cmath>
#include <QMessageBox>
#include "headers\MapView.h"

// Constructor
MapView::MapView(QChartView* parent): QChartView(parent),
    freeElements(0), obstacleElements(0)
{
    //Initialize QChart
    std::cout << "Create Chart \n";
    chart = new QChart();

    // New Series of Elements: free, obstacle, seen, start and goal
    freeElements = new QScatterSeries();
    obstacleElements = new QScatterSeries();
    seenElements = new QScatterSeries();
    startElement = new QScatterSeries();
    endElement = new QScatterSeries();

    // Setting Default start and end points
    startElementPoint = QPointF(1, heightGrid);
    endElementPoint = QPointF(widthGrid, 1);

}

// Destructor
MapView::~MapView()
{
    std::cout << "Destroying Grid View \n";

    std::cout << "Backend Grid: \n" <<
                 "Start index: " << gridMap.startIndex << "\n" <<
                 "End index: " << gridMap.endIndex << "\n";


    delete freeElements;
    delete obstacleElements;
    delete seenElements;
    delete startElement;
    delete endElement;
}

// Setters: currentInteraction
void MapView::setCurrentInteraction(int index)
{
    currentInteraction = static_cast<INTERACTIONS>(index);
}

// Creating the QChart
QChart* MapView::createChart()
{
    qreal x{1};
    // Index for gridPoint Vector
    int indexGrid{};
    // Populating the grid with elements
    for (int i=1;  i <= this->widthGrid; i++)
    {
        qreal y{1};
        for (int j=1;  j <= this->heightGrid; j++)
        {
            if (i == startElementPoint.x() && j == startElementPoint.y())
            {
                // Adding the starting Element in the startElement QScatterSeries
                startElement->append(QPointF(x, y));

                // Updating the backend grid: starting Element
                gridMap.startIndex = indexGrid;

                // Adding a null element in the QScatter Series of free Elements
                freeElements->append(QPointF());

            }else if (i == endElementPoint.x()   && j == endElementPoint.y())
            {
                // Adding the ending Element in the endElement QScatterSeries
                endElement->append(QPointF(x, y));

                // Updating the backend grid: ending Element
                gridMap.endIndex = indexGrid;

                // Adding a null element in the QScatter Series of free Elements
                freeElements->append(QPointF());

            } else
            {
                // Populating the QScatter Series of free Elements
                freeElements->append(QPointF(x, y));

            }

            // Populating the QScatter Series of free Elements
             obstacleElements->append(QPointF());
             seenElements->append(QPoint());


             // Grid Point: updating coordinated, already initialized as visited == false
             gridNode gridNodeBackend;
             gridNodeBackend.xCoord = int(x);
             gridNodeBackend.yCoord = int(y);

             // Populating the backend grid with the point (including start and end)
             gridMap.gridNodes.push_back(gridNodeBackend);

            y++;
            indexGrid++;
        }
        x++;
    }

    // Render
    setRenderHint(QPainter::Antialiasing);

    // Set background color
    //chart->setBackgroundBrush(Qt::SolidPattern);

    // Define shape and size and colors
    freeElements->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    obstacleElements->setMarkerShape(QScatterSeries::MarkerShapePentagon);
    seenElements->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    startElement->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    endElement->setMarkerShape(QScatterSeries::MarkerShapeStar);

    freeElements->setMarkerSize(qreal(25));
    obstacleElements->setMarkerSize(qreal(25));
    seenElements->setMarkerSize(qreal(25));
    startElement->setMarkerSize(qreal(25));
    endElement->setMarkerSize(qreal(25));

    // Adding Series in the chart
    chart->addSeries(freeElements);
    chart->addSeries(obstacleElements);
    chart->addSeries(seenElements);
    chart->addSeries(startElement);
    chart->addSeries(endElement);

    // Chart axis
    chart->createDefaultAxes();
    QList<QAbstractAxis*> xAxis = chart->axes(Qt::Horizontal);
    QList<QAbstractAxis*> yAxis = chart->axes(Qt::Vertical);
    xAxis.first()->setRange(0, this->widthGrid + 1);
    yAxis.first()->setRange(0, this->heightGrid + 1);

    // Setting name of the elements
    freeElements->setName("Free Elements");
    obstacleElements->setName("Obstacle Elements");
    seenElements->setName("Seen Elements");
    startElement->setName("Start");
    endElement->setName("Goal");

    // Create legends
    chart->legend()->setMarkerShape(QLegend::MarkerShapeFromSeries);

    // Connecting signals
    connect(freeElements, &QScatterSeries::clicked, this, &MapView::handleClickedPoint);
    connect(obstacleElements, &QScatterSeries::clicked, this, &MapView::handleClickedPoint);


    return chart;

}

void MapView::handleClickedPoint(const QPointF& point)
{
    // Copy of the point
    QPointF clickedPoint = point;

    // Free and obstacle points
    QList<QPointF> freeElementsPoints = freeElements->points();
    QList<QPointF> obstacleElementsPoints = obstacleElements->points();
    QList<QPointF> startElementPoint = startElement->points();
    QList<QPointF> endElementPoint = endElement->points();


    // Adding and deleting through indexing and null points
    qreal elementsDistance(INT_MAX);
    int closestIndexPos{};

    // Null QPoint
    QPointF nullQPoint = QPointF();
    QPointF currentPoint = QPointF();

    // We go through every points (besides start and end)
    for (int indexPos{}; indexPos < widthGrid * heightGrid; indexPos++)
    {
        // At each element, the state is either free or obstacle (later would also include start and end elements)
        if (obstacleElementsPoints[indexPos] != nullQPoint)
        {
            currentPoint = obstacleElementsPoints.at(indexPos);
        } else {
            currentPoint = freeElementsPoints.at(indexPos);
        }
        qreal currentDistance = computeDistanceBetweenPoints(clickedPoint, currentPoint);
        if (currentDistance < elementsDistance)
        {
            elementsDistance = currentDistance;
            closestIndexPos = indexPos;
        }
    }

    // If the user choose to place obstacles
    if (currentInteraction == OBSTACLE)
    {
        std::cout << "elementsDistance: " << elementsDistance << "\nclosestIndexPos: " << closestIndexPos << "\n";
        // If the point at closestIndex is a free element, then we add the obstacle
        if (freeElementsPoints.at(closestIndexPos) != nullQPoint)
        {
            // Creating obstacle
            obstacleElements->replace(closestIndexPos, freeElementsPoints[closestIndexPos]);
            freeElements->replace(closestIndexPos, nullQPoint);

            // Updating point as an obstacle in backend grid
            gridMap.gridNodes[closestIndexPos].obstacle = true;

        } else {
            // Deleting obstacle
            freeElements->replace(closestIndexPos, obstacleElementsPoints[closestIndexPos]);
            obstacleElements->replace(closestIndexPos, nullQPoint);

            // Updating point as a free element in the backend grid
            gridMap.gridNodes[closestIndexPos].obstacle = false;

        }

    } else if (currentInteraction == START)
    {
        // Saving the previous starting point index
        int previousStartIndex = gridMap.startIndex;
        QPointF& previousStartElement = startElementPoint[0];

        // Updating Starting point in backend grid
        gridMap.startIndex = closestIndexPos;

        // if the point at closestIndex is a free element, we add the start here and the previous start becomes free
        if (freeElementsPoints.at(closestIndexPos) != nullQPoint)
        {
            // Modyfing StartElement QScatter Series
            startElement->replace(previousStartElement, freeElementsPoints[closestIndexPos]);
            freeElements->replace(freeElementsPoints[closestIndexPos], previousStartElement);

            // Making sure the previous point is set as free in the backend grid
            gridMap.gridNodes[previousStartIndex].obstacle = false;

        } else // if the point at closestIndex is an obstacle element
        {

            // We add the starting point here and the previous start becomes an obstacle
            startElement->replace(previousStartElement, obstacleElementsPoints[closestIndexPos]);
            obstacleElements->replace(obstacleElementsPoints[closestIndexPos], previousStartElement);

            // Making sure the point is set as an obstacle in the backend grid
            gridMap.gridNodes[previousStartIndex].obstacle = true;

        }
    } else if (currentInteraction == END)
    {
        // Saving the previous ending point index
        int previousEndIndex = gridMap.endIndex;
        QPointF& previousEndElement = endElementPoint[0];

        // Updating ending point in backend grid
        gridMap.endIndex = closestIndexPos;

        // if the point at closestIndex is a free element, we add the end here and the previous end becomes free
        if (freeElementsPoints.at(closestIndexPos) != nullQPoint)
        {
            // We add the ending point here and the previous end becomes a free element
            endElement->replace(previousEndElement, freeElementsPoints[closestIndexPos]);
            freeElements->replace(freeElementsPoints[closestIndexPos], previousEndElement);

            // Making sure the previous end point is set as free in the backend grid
            gridMap.gridNodes[previousEndIndex].obstacle = false;


        } else// if the point at closestIndex is an obstacle element, we add the end here and the previous end becomes obstacle
        {
            // We add the ending point here and the previous end becomes a free element
            endElement->replace(previousEndElement, obstacleElementsPoints[closestIndexPos]);
            obstacleElements->replace(obstacleElementsPoints[closestIndexPos], previousEndElement);

            // Making sure the previous end point is set as obstacle in the backend grid
            gridMap.gridNodes[previousEndIndex].obstacle = true;

        }
    } else {
        QMessageBox::information(this, "Information", "Please select an interaction type");
    }

}

qreal MapView::computeDistanceBetweenPoints(const QPointF& pointA, const QPointF& pointB)
{
    return qSqrt(std::pow(pointA.x() - pointB.x(), 2)
                 + std::pow(pointA.y() - pointB.y(), 2));
}

