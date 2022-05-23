#include <iostream>
#include <QtCore/QtMath>
#include <QtCore/QDebug>
#include <cmath>
#include <QMessageBox>
#include "headers\GridView.h"

// Constructor
GridView::GridView(QChartView* parent): QChartView(parent),
    freeNodes(0), obstacleNodes(0)
{
    //Initialize QChart
    std::cout << "Create Chart \n";
    chart = new QChart();

    // New Series of Nodes: free, obstacle, seen, start and goal
    freeNodes = new QScatterSeries();
    obstacleNodes = new QScatterSeries();
    seenNodes = new QScatterSeries();
    startNode = new QScatterSeries();
    endNode = new QScatterSeries();

    // Setting Default start and end points
    startNodePoint = QPointF(1, heightGrid);
    endNodePoint = QPointF(widthGrid, 1);

}

// Destructor
GridView::~GridView()
{
    std::cout << "Destroying Grid View \n";

    delete freeNodes;
    delete obstacleNodes;
    delete seenNodes;
    delete startNode;
    delete endNode;
};

// Setters: currentInteraction
void GridView::setCurrentInteraction(int index)
{
    currentInteraction = static_cast<INTERACTIONS>(index);
}

// Creating the QChart
QChart* GridView::createChart()
{
    // Populating the grid with nodes
    qreal x{1};

    // Index for gridPoint Vector
    int indexGrid{};
    for (int i=1;  i <= this->widthGrid; i++)
    {
        qreal y{1};
        for (int j=1;  j <= this->heightGrid; j++)
        {
                 if (i == startNodePoint.x() && j == startNodePoint.y())
                 {
                     // Adding the starting Node in the startNode QScatterSeries
                     startNode->append(QPointF(x, y));
                     // Updating the backend grid: starting Node
                     gridBackend.startIndex = indexGrid;

                 }
            else if (i == endNodePoint.x()   && j == endNodePoint.y()  )
                 {
                     // Adding the ending Node in the endNode QScatterSeries
                     endNode->append(QPointF(x, y));
                     // Updating the backend grid: ending Node
                     gridBackend.endIndex = indexGrid;

                 }
            else
            {
                // Populating the QScatter Series
                freeNodes->append(QPointF(x, y));
                obstacleNodes->append(QPointF());
                seenNodes->append(QPoint());   
            }

             // Grid Point: updating coordinated, already initialized as visited == false
             gridPoint gridPointBackend;
             gridPointBackend.xCoord = int(x);
             gridPointBackend.yCoord = int(y);

             // Populating the backend grid with the point (including start and end)
             gridBackend.gridPoints.push_back(gridPointBackend);

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
    freeNodes->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    obstacleNodes->setMarkerShape(QScatterSeries::MarkerShapePentagon);
    seenNodes->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    startNode->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    endNode->setMarkerShape(QScatterSeries::MarkerShapeStar);

    freeNodes->setMarkerSize(qreal(25));
    obstacleNodes->setMarkerSize(qreal(25));
    seenNodes->setMarkerSize(qreal(25));
    startNode->setMarkerSize(qreal(25));
    endNode->setMarkerSize(qreal(25));

    // Adding Series in the chart
    chart->addSeries(freeNodes);
    chart->addSeries(obstacleNodes);
    chart->addSeries(seenNodes);
    chart->addSeries(startNode);
    chart->addSeries(endNode);

    // Chart axis
    chart->createDefaultAxes();
    QList<QAbstractAxis*> xAxis = chart->axes(Qt::Horizontal);
    QList<QAbstractAxis*> yAxis = chart->axes(Qt::Vertical);
    xAxis.first()->setRange(0, this->widthGrid + 1);
    yAxis.first()->setRange(0, this->heightGrid + 1);

    // Setting name of the nodes
    freeNodes->setName("Free Nodes");
    obstacleNodes->setName("Obstacle Nodes");
    seenNodes->setName("Seen Nodes");
    startNode->setName("Start");
    endNode->setName("Goal");

    // Create legends
    chart->legend()->setMarkerShape(QLegend::MarkerShapeFromSeries);

    // Connecting signals
    connect(freeNodes, &QScatterSeries::clicked, this, &GridView::handleClickedPoint);
    connect(obstacleNodes, &QScatterSeries::clicked, this, &GridView::handleClickedPoint);


    return chart;

}

void GridView::handleClickedPoint(const QPointF& point)
{
    // Copy of the point
    QPointF clickedPoint = point;

    // Free and obstacle points
    QList<QPointF> freeNodesPoints = freeNodes->points();
    QList<QPointF> obstacleNodesPoints = obstacleNodes->points();
    QList<QPointF> startNodePoint = startNode->points();
    QList<QPointF> endNodePoint = endNode->points();


    // Adding and deleting through indexing and null points
    qreal nodesDistance(INT_MAX);
    int closestIndexPos{};

    // Null QPoint
    QPointF nullQPoint = QPointF();
    QPointF currentPoint = QPointF();

    // We go through every points (besides start and end)
    for (int indexPos{}; indexPos < widthGrid * heightGrid - 2; indexPos++)
    {
        // At each node, the state is either free or obstacle (later would also include start and end nodes)
        if (obstacleNodesPoints[indexPos] != nullQPoint)
        {
            currentPoint = obstacleNodesPoints.at(indexPos);
        } else {
            currentPoint = freeNodesPoints.at(indexPos);
        }
        qreal currentDistance = computeDistanceBetweenPoints(clickedPoint, currentPoint);
        if (currentDistance < nodesDistance)
        {
            nodesDistance = currentDistance;
            closestIndexPos = indexPos;
        }
    }
    if (currentInteraction == OBSTACLE)
    {
        std::cout << "nodesDistance: " << nodesDistance << "\nclosestIndexPos: " <<closestIndexPos <<"\n";
        // If the point at closestIndex is a free node, then we add the obstacle
        if (freeNodesPoints.at(closestIndexPos) != nullQPoint)
        {
            // Creating obstacle
            obstacleNodes->replace(closestIndexPos, freeNodesPoints[closestIndexPos]);
            freeNodes->replace(closestIndexPos, nullQPoint);

            // Updating point as an obstacle in backend grid
            // WRONG CANNOT USE CLOSEST INDEX gridBackend.gridPoints[closestIndexPos].obstacle = true;

        } else {
            // Deleting obstacle
            freeNodes->replace(closestIndexPos, obstacleNodesPoints[closestIndexPos]);
            obstacleNodes->replace(closestIndexPos, nullQPoint);

            // Updating point as a free node in backend grid
            // WRONG CANNOT USE CLOSEST INDEX  (maybe just - 2 ??) gridBackend.gridPoints[closestIndexPos].obstacle = false;

        }

    } else if (currentInteraction == START)
    {
        QPointF previousStartNode = startNodePoint[0];

        // if the point at closestIndex is a free node, we add the start here and the previous start becomes free
        if (freeNodesPoints.at(closestIndexPos) != nullQPoint)
        {
            // Modyfing StartNode QScatter Series
            startNode->replace(previousStartNode, freeNodesPoints[closestIndexPos]);
            freeNodes->replace(freeNodesPoints[closestIndexPos], previousStartNode);

            // Updating Starting point in backend grid
            gridBackend.startIndex =

            // Making sure the previous point is set as free in the backend grid
            int previousStartIndex = gridBackend
            gridBackend.gridPoints[previousStartIndex].obstacle = false;

        } else{ // if the point at closestIndex is an obstacle node

            // We add the starting point here and the previous start becomes an obstacle
            startNode->replace(previousStartNode, obstacleNodesPoints[closestIndexPos]);
            obstacleNodes->replace(obstacleNodesPoints[closestIndexPos], previousStartNode);

            // Making sure the point is set as an obstacle in the backend grid


        }
    } else if (currentInteraction == END){
        QPointF previousEndNode = endNodePoint[0];

        // if the point at closestIndex is a free node, we add the end here and the previous end becomes free
        if (freeNodesPoints.at(closestIndexPos) != nullQPoint)
        {
            endNode->replace(previousEndNode, freeNodesPoints[closestIndexPos]);
            freeNodes->replace(freeNodesPoints[closestIndexPos], previousEndNode);
        } else{ // if the point at closestIndex is an obstacle node, we add the end here and the previous end becomes obstacle
            endNode->replace(previousEndNode, obstacleNodesPoints[closestIndexPos]);
            obstacleNodes->replace(obstacleNodesPoints[closestIndexPos], previousEndNode);
        }
    } else {
        QMessageBox::information(this, "Information", "Please select an interaction type");

    }

}

qreal GridView::computeDistanceBetweenPoints(const QPointF& pointA, const QPointF& pointB)
{
    return qSqrt(std::pow(pointA.x() - pointB.x(), 2)
                 + std::pow(pointA.y() - pointB.y(), 2));
}
