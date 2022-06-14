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

        void runBFS(grid gridNodes);

    public: Q_SIGNALS:
        void updatedgridView(UPDATETYPES VISIT, int currentIndex);

    public:
        grid performBfsAlgorithm(grid gridNodes);

        // Retrieving the neighbors of a point in a grid
        std::vector<Node> retrieveNeighborsGrid(const grid* gridNodes, const Node& currentNode, int heightGrid);

        grid gridNodes;
    private:
        ALGOS currentAlgorithm;


};

#endif // PATHALGORITHM_H
