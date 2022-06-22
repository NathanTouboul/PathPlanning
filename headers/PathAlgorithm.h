#ifndef PATHALGORITHM_H
#define PATHALGORITHM_H
#include <QObject>
#include <QDebug>

#include <QtConcurrent>
#include <QFuture>
#include "headers/GridView.h"

class PathAlgorithm : public QObject
{

    Q_OBJECT
    public:

        //Constructor
        explicit PathAlgorithm(QObject* parent = nullptr);

        //Destructor
        virtual ~PathAlgorithm();

        //Getters: current Algorithm from gridView
        ALGOS getCurrentAlgorithm() const;


        // Running pausing and canceling algorithms
        void runAlgorithm(ALGOS algorithm);
        void pauseAlgorithm();
        void resumeAlgorithm();

        // BFS Algorithm
        void performBFSAlgorithm(QPromise<int>& promise);
        void performDFSAlgorithm(QPromise<int>& promise);

        // Retrieving the neighbors of a point in a grid
        std::vector<Node> retrieveNeighborsGrid(const grid& gridNodes, const Node& currentNode, int widthGrid, int heightGrid);

        void checkGridNode(grid gridNodes, int heightGrid, int widthGrid);

    public: Q_SIGNALS:
        void updatedgridView(UPDATETYPES VISIT, int currentIndex);
        void algorithmCompleted();

    public:

        ALGOS currentAlgorithm;
        bool running;
        bool simulationOnGoing;
        bool endReached;
        int speedVisualization;

        // grid nodes manipulated by the path planning object
        grid gridNodes;
        int heightGrid;
        int widthGrid;

        // Multithreading
        QThreadPool pool;
        QFuture<int> futureOutput;

};

#endif // PATHALGORITHM_H
