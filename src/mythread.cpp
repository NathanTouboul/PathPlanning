#include "headers/mythread.h"
#include <QThread>
#include <QtCore>

#include <iostream>
#include <string>


// Thread Constructor
MyThread::MyThread(QObject *parent) : QThread(parent){}

// Overriding the run function
void MyThread::run()
{
//    std::cerr << "Thread started! \n";

//    while (true)
//    {
//        QMutex mutex;
//        mutex.lock();

//        if (this->stop) {break;}

//        mutex.unlock();
//        this->msleep(100);

//    }



}

