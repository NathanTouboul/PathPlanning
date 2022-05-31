#ifndef PATHALGORITHM_H
#define PATHALGORITHM_H

#include "headers/GridView.h"

class PathAlgorithm
{
    public:

        //Constructor
        PathAlgorithm();

        //Destructor
        virtual ~PathAlgorithm();

        //Getters: current Algorithm from gridView
        ALGOS getCurrentAlgorithm() const;

        // Methods
        int bfsAlgorithm(GridView* gridView);

        // Retrieving the neighbors of a point in the grid
        std::vector<Node> retrieveNeighborsGrid(const grid* gridNodes, const Node& currentNode, int heightGrid);

    private:
        ALGOS currentAlgorithm;
};

#endif // PATHALGORITHM_H
