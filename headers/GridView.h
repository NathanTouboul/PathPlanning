#ifndef GRIDVIEW_H
#define GRIDVIEW_H

#include <QtCharts/QChartGlobal>
#include <QtCharts/QChartView>
#include <QtCharts/QScatterSeries>
#include <QGridLayout>
#include <array>
QT_USE_NAMESPACE


struct gridPoint
{
    int xCoord, yCoord;
    bool visited = false; // false -> free, true -> seen
    bool obstacle = false;
};

struct grid
{
    std::vector<gridPoint> gridPoints;
    int startIndex;
    int endIndex;
};

class GridView: public QChartView
{

    Q_OBJECT

    public:

        //Constructor
        GridView(QChartView* parent=0);

        // Destructor
        virtual ~GridView();

        // Possible interactions in the chartview
        enum INTERACTIONS {START, END, OBSTACLE, NONE};

        // Setters: currentInteraction
        void setCurrentInteraction(int index);

        // getters: grid
        std::vector<gridPoint> getGrid() const;

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

        INTERACTIONS currentInteraction = NONE;

        grid gridBackend;

};


#endif // GRIDVIEW_H
