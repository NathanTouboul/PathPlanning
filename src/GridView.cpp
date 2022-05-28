#include <iostream>
#include <QtCore/QtMath>
#include <QtCore/QDebug>
#include <cmath>
#include <QMessageBox>
#include "headers\GridView.h"

// Constructor
GridView::GridView(QChartView* parent): QChartView(parent),
    freeElements(0), obstacleElements(0)
{
    //Initialize QChart
    std::cerr << "Create Chart \n";
    chart = new QChart();

    // New Series of Elements: free, obstacle, seen, start and goal
    freeElements = new QScatterSeries();
    obstacleElements = new QScatterSeries();
    seenElements = new QScatterSeries();
    startElement = new QScatterSeries();
    endElement = new QScatterSeries();

    // Inserting points in the series
    for (int i = 0; i < heightGrid * widthGrid; i++)
    {
        freeElements->append(QPoint());
        obstacleElements->append(QPoint());
        seenElements->append(QPoint());
    }
    startElement->append(QPoint());
    endElement->append(QPoint());
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
    delete seenElements;
    delete startElement;
    delete endElement;
}

// Setters: currentInteraction
void GridView::setCurrentInteraction(int index)
{
    currentInteraction = static_cast<INTERACTIONS>(index);
}

ARRANGEMENTS GridView::getCurrentArrangement() const
{
    return currentArrangement;
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
    int clickedIndex = coordToIndex(clickedPoint);
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

    } else if (currentInteraction == RUNNING)
    {
        QMessageBox::information(this, "Information", "Please stop the simulation first");

    }else
    {
        QMessageBox::information(this, "Information", "Please select an interaction type");
    }

}

qreal GridView::computeDistanceBetweenPoints(const QPointF& pointA, const QPointF& pointB)
{
    return qSqrt(std::pow(pointA.x() - pointB.x(), 2)
                 + std::pow(pointA.y() - pointB.y(), 2));
}
int GridView::coordToIndex(const QPointF& point)
{
    return (point.x() - 1) * heightGrid + point.y() - 1;
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
        for (int i=1;  i <= this->widthGrid; i++)
        {
            qreal y{1};
            for (int j=1;  j <= this->heightGrid; j++)
            {
                if (i == startElement->points()[0].x() && j == startElement->points()[0].y())
                {
                    // Updating the backend grid: starting Element
                    gridNodes.startIndex = indexGrid;

                }else if (i == endElement->points()[0].x() && j == endElement->points()[0].y())
                {
                    // Updating the backend grid: ending Element
                    gridNodes.endIndex = indexGrid;

                }else
                {
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
}


