#include <iostream>
#include <QtCore/QtMath>
#include <QtCore/QDebug>
#include <cmath>
#include <exception>
#include "headers\GridPixel.h"

// Constructor
GridPixel::GridPixel(QChartView* parent): QChartView(parent),
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
GridPixel::~GridPixel()
{
    std::cout << "Destroying Grid Pixel \n";
    delete freeNodes;
    delete obstacleNodes;
    delete seenNodes;
};


QChart* GridPixel::createChart()
{
    // Populating the grid with nodes
    qreal x{1};
    for (int i=1;  i <= this->widthGrid; i++)
    {
        qreal y{1};
        for (int j=1;  j <= this->heightGrid; j++)
        {
                 if (i == startNodePoint.x() && j == startNodePoint.y()){ startNode->append(QPointF(x, y)); }
            else if (i == endNodePoint.x()   && j == endNodePoint.y()  ){   endNode->append(QPointF(x, y)); }
            else
            {
                freeNodes->append(QPointF(x, y));
                obstacleNodes->append(QPointF());
                seenNodes->append(QPoint());
            }
            y++;
        }
        x++;
    }

    // Render
    setRenderHint(QPainter::Antialiasing);

    // Set background color
    chart->setBackgroundBrush(Qt::SolidPattern);

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
    startNode->setName("Start Node");
    endNode->setName("End Nodes");

    // Create legends
    chart->legend()->setMarkerShape(QLegend::MarkerShapeFromSeries);

    // Connecting signals
    connect(freeNodes, &QScatterSeries::clicked, this, &GridPixel::handleClickedPoint);
    connect(obstacleNodes, &QScatterSeries::clicked, this, &GridPixel::handleClickedPoint);


    return chart;

}

void GridPixel::handleClickedPoint(const QPointF& point)
{
    // Copy of the point
    QPointF clickedPoint = point;

    // Free and obstacle points
    QList<QPointF> freeNodesPoints = freeNodes->points();
    QList<QPointF> obstacleNodesPoints = obstacleNodes->points();


    // Adding and deleting through indexing and null points
    qreal nodesDistance(INT_MAX);
    int closestIndexPos{};

    // Null QPoint
    QPointF nullQPoint = QPointF();
    QPointF currentPoint = QPointF();

    for (int indexPos{}; indexPos < numCells -2; indexPos++)
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

    std::cout << "nodesDistance: " << nodesDistance << "\nclosestIndexPos: " <<closestIndexPos <<"\n";
    // If the point at closestIndex is a free node, then we add the obstacle
    if (freeNodesPoints.at(closestIndexPos) != nullQPoint)
    {
        // Creating obstactle
        std::cout << "Creating obstacle at [x=" << obstacleNodesPoints[closestIndexPos].x()
                                      << ", y=" << obstacleNodesPoints[closestIndexPos].y() << "] \n";

        //obstacleNodes->points()[closestIndexPos] = freeNodes->points()[closestIndexPos];
        //freeNodes->points()[closestIndexPos] = nullQPoint;

        obstacleNodes->replace(closestIndexPos, freeNodesPoints[closestIndexPos]);
        freeNodes->replace(closestIndexPos, nullQPoint);

    } else {
        // Deleting obstacle
        std::cout << "Deleting obstacle at [x=" << freeNodesPoints[closestIndexPos].x()
                                      << ", y=" << freeNodesPoints[closestIndexPos].y() << "] \n";
        //freeNodes->points()[closestIndexPos] = obstacleNodes->points()[closestIndexPos];
        //obstacleNodes->points()[closestIndexPos] = nullQPoint;

        freeNodes->replace(closestIndexPos, obstacleNodesPoints[closestIndexPos]);
        obstacleNodes->replace(closestIndexPos, nullQPoint);
    }

}

qreal GridPixel::computeDistanceBetweenPoints(const QPointF& pointA, const QPointF& pointB)
{
    return qSqrt(std::pow(pointA.x() - pointB.x(), 2)
                 + std::pow(pointA.y() - pointB.y(), 2));
}

