#ifndef GRIDVIEW_H
#define GRIDVIEW_H

#include <QtCharts/QChartGlobal>
#include <QtCharts/QChartView>
#include <QtCharts/QScatterSeries>
#include <QGridLayout>
#include <array>
QT_USE_NAMESPACE

// Possible interactions in the chartview
typedef enum {START, END, OBSTACLE, RUNNING, NONE} INTERACTIONS;

// Possible grid arrangement
typedef enum {EMPTY, MAZE} ARRANGEMENTS;

struct Node
{
    int xCoord, yCoord;
    bool visited = false; // false -> free, true -> seen
    bool obstacle = false;
};

struct grid
{
    std::vector<Node> Nodes;
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

        // Setters: currentInteraction
        void setCurrentInteraction(int index);

        // getters: grid
        grid* getGrid();
        ARRANGEMENTS getCurrentArrangement() const;

        // Methods
        QChart* createChart();
        qreal computeDistanceBetweenPoints(const QPointF& pointA, const QPointF& pointB);
        int coordToIndex(const QPointF& point);
        void populateGridMap(ARRANGEMENTS arrangement);

    private Q_SLOTS:
        void handleClickedPoint(const QPointF& point);

    private:

        QChart* chart;
        QScatterSeries *freeElements;
        QScatterSeries *obstacleElements;
        QScatterSeries *seenElements;
        QScatterSeries *startElement;
        QScatterSeries *endElement;

        const int widthGrid = 10;
        const int heightGrid = 10;

        INTERACTIONS currentInteraction = NONE;
        ARRANGEMENTS currentArrangement = EMPTY;
        grid gridNodes;

};


#endif // GRIDVIEW_H
