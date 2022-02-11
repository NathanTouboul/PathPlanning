#include <QChartView>
#include "headers\mainWindow.h"
#include "ui_mainWindow.h"
#include "headers\GridPixel.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Set up central widget
    QWidget* centralWidget = findChild<QWidget*>("centralWidget");
    setCentralWidget(centralWidget);

    // Set up chartview
    QChartView* chartView = new QChartView();
    chartView->setObjectName("chartView");
    ui->gridLayout->addWidget(chartView, 1, 0);

}

MainWindow::~MainWindow()
{
    delete ui;
}

