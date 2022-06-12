#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include <QObject>

class MyThread : public QThread
{
    Q_OBJECT

    public:
        // Constructor
        explicit MyThread(QObject *parent = 0);

        // Overriding the run function
        void run();

        bool stop;

    signals:

        // Signal indicating the run button was clicked on
        void startButtonPressed();

        // Signal indicating the reset button was clicked on
        void resetButtonPressed();

        // Signal indicating data changed in the gridView
        void dataChanged();

};


#endif // MYTHREAD_H
