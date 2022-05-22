#include <QChartView>
#include <QMessageBox>
#include "headers\mainWindow.h"
#include "ui_mainWindow.h"
#include "headers\GridView.h"



MainWindow::MainWindow(QWidget *parent): QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Setting up the Interaction Combo Box
    setupInteractionComboBox();

    // Setting up the Algorithms Combo Box
    setupAlgorithmsComboBox();

    // Setting up the chart view
    setupChartView("chartView");

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupInteractionComboBox()
{
    // Default text
    ui->interactionBox->setPlaceholderText(QStringLiteral("--Select Interaction--"));
    ui->interactionBox->setCurrentIndex(-1);

    // Adding first interation: Add starting point
    ui->interactionBox->addItem("Add Start");

    // Adding second interaction: Add end point
    ui->interactionBox->addItem("Add Goal");

    // Adding first interation: Add starting point
    ui->interactionBox->addItem("Add Obstacles");


}

void MainWindow::setupAlgorithmsComboBox()
{
    ui->algorithmsBox->setPlaceholderText(QStringLiteral("--Select Algorithm--"));
    ui->algorithmsBox->setCurrentIndex(-1);

    // Adding first interation: BFS
    ui->algorithmsBox->addItem("BFS");

}

void MainWindow::setupChartView(QString chartviewName)
{

    // Setting up chartview
    ui->chartView->setObjectName(chartviewName);
    ui->chartView->setMinimumWidth(qreal(200));
    ui->chartView->setMinimumHeight(qreal(500));

    // Create Chart in chartview
    QChart* chart = gridView.createChart();
    ui->chartView->setChart(chart);

}

void MainWindow::on_startButton_clicked()
{
    if (ui->algorithmsBox->currentIndex() == -1)
    {
        QMessageBox::information(this, "Information", "Please select a path finding algorithm");
    } else{
        // call path finding

        // need to be updated at each steps
    }
}


void MainWindow::on_interactionBox_currentIndexChanged(int index)
{
    // Updating the current interaction chosen by the user
    gridView.setCurrentInteraction(index);
}


