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

        // Deconstructor
        virtual ~GridPixel();


        // Methods
        QChart* createChart();


    private Q_SLOTS:
        void handleClickedPoint(const QPointF& point);

    private:
        // New chart
        QChart* chart;
        QScatterSeries *m_free;
        QScatterSeries *m_obstacle;
        const int WIDTH_GRID = 16;
        const int HEIGHT_GRID = 16;

};


#endif // GRIDPIXEL_H
