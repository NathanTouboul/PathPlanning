#ifndef GRIDVIEW_H
#define GRIDVIEW_H

#include <QtCharts/QChartGlobal>
#include <QtCharts/QChartView>
#include <QtCharts/QScatterSeries>
#include <QGridLayout>
#include <QObject>
#include <array>

QT_USE_NAMESPACE

// Possible interactions in the gridview chosen in the Interaction Box
typedef enum {START, END, OBSTACLE, NOINTERACTION} INTERACTIONS;

// Possible grid arrangement chosen in the Maze Box
typedef enum {EMPTY, MAZE, NOARRANG} ARRANGEMENTS;

// Possible Algorithm chosen in the Algorithm Box
typedef enum {BFS, NOALGO} ALGOS;

// Possible update in the grid view from the Path Algorithm
typedef enum {CURRENT, VISIT} UPDATETYPES;


// Node structure
struct Node
{
    int xCoord, yCoord;
    bool visited = false; // false -> free, true -> visited
    bool obstacle = false;
};

// Grid structure
struct grid //: public QObject
{
    //Q_OBJECT

    public:
    std::vector<Node> Nodes; // Need to be a vector since we will give the user the choice of number of nodes
    int startIndex;
    int endIndex;
    int currentIndex;
};



// Converting point coordinates to index
int coordToIndex(const QPointF& point, int heightGrid);
int coordToIndex(int x, int y, int heightGrid);

class GridView: public QChartView
{
    Q_OBJECT

    public:

        //Constructor
        explicit GridView(QChartView* parent=0);

        // Destructor
        virtual ~GridView();

        // Setter: currentInteraction
        void setCurrentInteraction(int index);
        void setCurrentInteraction(INTERACTIONS interaction);

        // Setter: currentAlgorithm from Box to PathAlgorithm
        void setCurrentAlgorithm(int index);

        // Setter: current state
        void setCurrentState(bool state);

        // getter: grid of Nodes for the path planning algorithms
        grid& getGrid();

        // Getter current interaction
        INTERACTIONS getCurrentInteraction() const;

        // Getter current Arrangement
        ARRANGEMENTS getCurrentArrangement() const;

        // Getter: current Algorithm for the main window
        ALGOS getCurrentAlgorithm() const;

        // Getter current state
        bool getCurrentState() const;

        // Getter: height grid
        int getHeightGrid() const;

        // Create the chart and the grid
        QChart* createChart();

        // Populating the grid with points, depending on the arrangement selected
        void populateGridMap(ARRANGEMENTS arrangement);

        // Computing the distance between two points
        qreal computeDistanceBetweenPoints(const QPointF& pointA, const QPointF& pointB);

        // Setup view for algorithm
        void AlgorithmView(bool on);

        // Method to find neighbors of a node
        std::vector<Node> retrieveNeighborsGrid(const grid* gridNodes, const Node& currentNode, int heightGrid);


    public Q_SLOTS:

        // Event handleClickedPoint
        void handleClickedPoint(const QPointF& point);

        // Handles the changes in the gridView by the path planning algorithm
        bool handleUpdatedgridView(UPDATETYPES updateType, int updateIndex);

    public:
        // Launching BFS multi threaded
        void launchingEventBFS();

    public:

        QChart* chart;

        QScatterSeries* freeElements;
        QScatterSeries* obstacleElements;
        QScatterSeries* visitedElements;
        QScatterSeries* startElement;
        QScatterSeries* endElement;
        QScatterSeries* currentElement;

        int widthGrid = 10;
        int heightGrid = 10;
        qreal markerSize = 20;

        INTERACTIONS currentInteraction;
        ARRANGEMENTS currentArrangement;
        ALGOS currentAlgorithm;
        bool currentState;

        grid gridNodes;

};

#endif // GRIDVIEW_H
