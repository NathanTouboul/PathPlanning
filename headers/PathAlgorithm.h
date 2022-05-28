#ifndef PATHALGORITHM_H
#define PATHALGORITHM_H

typedef enum {BFS} algos;

class PathAlgorithm
{
    public:

        //Constructor
        PathAlgorithm();

        //Destructor
        virtual ~PathAlgorithm();

        //Setters
        void setCurretAlgorithm();

        //Getters
        algos getCurrentAlgorithm() const;

    private:

};

#endif // PATHALGORITHM_H
