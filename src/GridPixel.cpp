#include <iostream>
#include <QtCore/QtMath>
#include <QtCore/QDebug>

#include "headers\GridPixel.h"

// Constructor
GridPixel::GridPixel(QChartView* parent): QChartView(parent),
    m_free(0), m_obstacle(0)
{
    //Initialize QChart
    std::cout << "Create Chart \n";
    chart = new QChart();

    // New Grids: free and obstacles
    m_free = new QScatterSeries();
    m_obstacle = new QScatterSeries();

    // Sizing and position of Qchart
    QPoint positionParent = parent->pos();
    QRectF RectangleSize(positionParent.x(), positionParent.y(),  parent->width(), parent->height());
    chart->setPlotArea(RectangleSize);

}

// Deconstructor
GridPixel::~GridPixel() 
{
    std::cout << "Destroying Grid Pixel \n";
    delete m_free;
    delete m_obstacle;
};


QChart* GridPixel::createChart()
{

    // Render
    setRenderHint(QPainter::Antialiasing);

    m_free->setName("Free");
    m_obstacle->setName("Obstacle");

    qreal x{};
    for (int i=0;  i < this->WIDTH_GRID; i++)
    {
        qreal y{};
        for (int j=0;  j < this->HEIGHT_GRID; j++)
        {
            *m_free << QPointF(x, y);
            *m_obstacle << QPointF(x, y);
            y++;
        }
        x++;
    }

    // Define visual shape and sizes
    m_free->setMarkerSize(qreal(10));
    m_free->setMarkerShape(QScatterSeries::MarkerShapeRectangle);

    m_obstacle->setMarkerSize(qreal(10));
    m_obstacle->setMarkerShape(QScatterSeries::MarkerShapeStar);


    // Grids in plot
    chart->addSeries(m_free);
    chart->addSeries(m_obstacle);
    chart->createDefaultAxes();
    chart->axes(Qt::Horizontal).first()->setRange(0, 10);
    chart->axes(Qt::Vertical).first()->setRange(0, 10);

    // Create legends
    chart->legend()->setMarkerShape(QLegend::MarkerShapeFromSeries);

    // Connecting signals
    connect(m_free, &QScatterSeries::clicked, this, &GridPixel::handleClickedPoint);

    return chart;

}

void GridPixel::handleClickedPoint(const QPointF& point)
{
    // Copy of the point
    QPointF clickedPoint = point;

    // Find the closest point from series 1
    QPointF closest(INT_MAX, INT_MAX);

    qreal distance(INT_MAX);
    const auto points = m_free->points();
    for (const QPointF &currentPoint : points) {
        qreal currentDistance = qSqrt((currentPoint.x() - clickedPoint.x())
                                      * (currentPoint.x() - clickedPoint.x())
                                      + (currentPoint.y() - clickedPoint.y())
                                      * (currentPoint.y() - clickedPoint.y()));
        if (currentDistance < distance) {
            distance = currentDistance;
            closest = currentPoint;
        }
    }

    // Change the color of the point (closest)
    std::cout << "Modifying points \n";
    m_free->remove(closest);
    m_obstacle->append(closest);

}
