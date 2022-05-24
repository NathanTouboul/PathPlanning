#include <QApplication>
#include "headers/mainWindow.h"
#include "headers/MapView.h"

int main(int argc, char *argv[])
{
    // Starting a new QApplication
    QApplication a(argc, argv);

    // Setup of the new Window
    MainWindow window;
    window.resize(400, 300);
    window.show();

    return a.exec();
}

