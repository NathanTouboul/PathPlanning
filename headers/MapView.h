#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QtCharts/QChartGlobal>
#include <QtCharts/QChartView>
#include <QtCharts/QScatterSeries>
#include <QGridLayout>
#include <array>
QT_USE_NAMESPACE


struct gridNode
{
    int xCoord, yCoord;
    bool visited = false; // false -> free, true -> seen
    bool obstacle = false;
};

struct grid
{
    std::vector<gridNode> gridNodes;
    int startIndex;
    int endIndex;
};

class MapView: public QChartView
{

    Q_OBJECT

    public:

        //Constructor
        MapView(QChartView* parent=0);

        // Destructor
        virtual ~MapView();

        // Possible interactions in the chartview
        enum INTERACTIONS {START, END, OBSTACLE, NONE};

        // Setters: currentInteraction
        void setCurrentInteraction(int index);

        // getters: grid
        std::vector<gridNode> getGrid() const;

        // Methods
        QChart* createChart();
        qreal computeDistanceBetweenPoints(const QPointF& pointA, const QPointF& pointB);

    private Q_SLOTS:
        void handleClickedPoint(const QPointF& point);

    private:

        QChart* chart;
        QScatterSeries *freeElements;
        QScatterSeries *obstacleElements;
        QScatterSeries *seenElements;
        QScatterSeries *startElement;
        QScatterSeries *endElement;

        QPointF startElementPoint;
        QPointF endElementPoint;

        const int widthGrid = 10;
        const int heightGrid = 10;

        INTERACTIONS currentInteraction = NONE;

        grid gridMap;

};


#endif // MAPVIEW_H
