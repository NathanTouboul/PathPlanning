#ifndef GRIDPIXEL_H
#define GRIDPIXEL_H

#include <QtCharts/QChartGlobal>
#include <QtCharts/QChartView>
#include <QtCharts/QScatterSeries>
#include <QGridLayout>
QT_USE_NAMESPACE

class GridPixel: public QChartView
{

    Q_OBJECT

    public:
        //Constructor
        GridPixel(QChartView* parent=0);

        // Destructor
        virtual ~GridPixel();


        // Methods
        QChart* createChart();
        qreal computeDistanceBetweenPoints(const QPointF& pointA, const QPointF& pointB);

    private Q_SLOTS:
        void handleClickedPoint(const QPointF& point);

    private:

        QChart* chart;
        QScatterSeries *freeNodes;
        QScatterSeries *obstacleNodes;
        QScatterSeries *seenNodes;
        QScatterSeries *startNode;
        QScatterSeries *endNode;

        QPointF startNodePoint;
        QPointF endNodePoint;

        const int widthGrid = 10;
        const int heightGrid = 10;
        const int numCells = widthGrid * heightGrid;
        const float nodeSpan = 1;


};


#endif // GRIDPIXEL_H
