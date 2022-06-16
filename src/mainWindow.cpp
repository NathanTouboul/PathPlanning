#include <iostream>
#include <QChartView>
#include <QMessageBox>

#include "headers/mainWindow.h"
#include "ui_mainWindow.h"
#include "headers/GridView.h"


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow), pathAlgorithm()
{
    // Setup of the window
    ui->setupUi(this);

    // Setting up the chart view
    setupGridView("gridView");

    // Setting up the Interaction Combo Box
    setupInteractionComboBox();

    // Setting up the Algorithms Combo Box
    setupAlgorithmsComboBox();

    // A change in the grid view create a change in the chartview
    connect(&pathAlgorithm, &PathAlgorithm::updatedgridView, &gridView, &GridView::handleUpdatedgridView);
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

void MainWindow::setupGridView(QString gridViewName)
{

    // Setting up chartview
    ui->gridView->setObjectName(gridViewName);
    ui->gridView->setMinimumWidth(qreal(200));
    ui->gridView->setMinimumHeight(qreal(500));

    // Create Chart in chartview
    QChart* chart = gridView.createChart();
    ui->gridView->setChart(chart);
}

GridView& MainWindow::getGridView()
{
    return gridView;
}

void MainWindow::on_runButton_clicked()
{
    if (ui->algorithmsBox->currentIndex() == -1){
        QMessageBox::information(this, "Information", "Please select a path finding algorithm");
    }else{

        // Setting the run button as checkble and checked
        ui->runButton->setCheckable(true);
        ui->runButton->setChecked(true);
        ui->runButton->setText(QString("PAUSE"));

        // Blocking the interaction with the gridView
        gridView.setCurrentState(true);

        // Enabling the current QScatter series point as visible
        gridView.AlgorithmView(true);

        // Call path finding
        if (gridView.getCurrentAlgorithm() == BFS)
        {
            pathAlgorithm.runBFS(gridView.getGrid());
        }
    }

    if (gridView.getCurrentState() == true)
    {
        gridView.setCurrentState(false);
        ui->runButton->setChecked(false);

    }

    // Disabling the current QScatter series point as visible
    gridView.AlgorithmView(false);

}

void MainWindow::on_resetButton_clicked()
{
    // Calling populate grid with same previous arrangement
    gridView.populateGridMap(gridView.getCurrentArrangement());

}

void MainWindow::on_interactionBox_currentIndexChanged(int index)
{
    // Updating the current interaction chosen by the user
    gridView.setCurrentInteraction(index);
}

void MainWindow::on_algorithmsBox_currentIndexChanged(int index)
{
    // Changing the current Algorithm
    gridView.setCurrentAlgorithm(index);
}


