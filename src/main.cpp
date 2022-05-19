#include <QApplication>
#include "headers/mainWindow.h"
#include "headers/GridPixel.h"

int main(int argc, char *argv[])
{
    // Starting a new QApplication
    QApplication a(argc, argv);

    // Setup of the new Window
    MainWindow window;

    // Create Chart in chartview
    QChartView* chartView = window.findChild<QChartView*>("chartView");

    GridPixel map;

    QChart* chart = map.createChart();
    chartView->setChart(chart);

    window.resize(400, 300);
    window.show();

    return a.exec();
}

