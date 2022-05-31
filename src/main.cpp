#include <QApplication>
#include "headers/mainWindow.h"
#include "headers/GridView.h"
#include "headers/PathAlgorithm.h"

int main(int argc, char *argv[])
{
    // Starting a new QApplication
    QApplication a(argc, argv);

    // Setup of the new Window
    MainWindow window;
    window.resize(500, 350);
    window.show();

    return a.exec();
}
