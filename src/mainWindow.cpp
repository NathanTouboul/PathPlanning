#include <iostream>
#include <QChartView>
#include <QMessageBox>

#include "headers/mainWindow.h"
#include "ui_mainWindow.h"
#include "headers/GridView.h"


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow), gridView(30, 30, 19), pathAlgorithm()
{
    // Setup of the window
    ui->setupUi(this);


    // Customize chart background
    QLinearGradient backgroundGradient;
    backgroundGradient.setStart(QPointF(0, 0));
    backgroundGradient.setFinalStop(QPointF(0, 1));
    backgroundGradient.setColorAt(0.0, QRgb(0xd2d0d1));
    backgroundGradient.setColorAt(1.0, QRgb(0x4c4547));
    backgroundGradient.setCoordinateMode(QGradient::ObjectBoundingMode);

    QBrush brush(backgroundGradient);

    QPalette palette;
    palette.setBrush(QPalette::Window, brush);
    setPalette(palette);

    // Setup
    ui->dialWidth   ->setValue  (gridView.widthGrid);
    ui->dialWidth   ->setMinimum(5);
    ui->dialWidth   ->setMaximum(35);
    ui->lcdWidth    ->display   (gridView.widthGrid);    

    ui->dialHeight  ->setValue  (gridView.heightGrid);
    ui->dialHeight   ->setMinimum(5);
    ui->dialHeight   ->setMaximum(35);
    ui->lcdHeight   ->display   (gridView.heightGrid);

    ui->sliderMarker->setValue  (gridView.markerSize);
    ui->lcdMarker   ->display   (gridView.markerSize);

    // Initial Simulation speed
    ui->speedSpinBox->setMaximum(100);
    int speed = ui->speedSpinBox->maximum() / 5;
    ui->speedSpinBox->setValue  (speed);

    // Setting up the chart view
    setupGridView("gridView");

    // Setting up the Interaction Combo Box
    setupInteractionComboBox();

    // Setting up the Algorithms Combo Box
    setupAlgorithmsComboBox();

    // A change in the grid view create a change in the chartview
    connect(&pathAlgorithm, &PathAlgorithm::updatedScatterGridView, &gridView, &GridView::handleUpdatedScatterGridView);
    connect(&pathAlgorithm, &PathAlgorithm::updatedLineGridView,    &gridView, &GridView::handleUpdatedLineGridView);

    // Connecting the end signal of path planning to the window
    connect(&pathAlgorithm, &PathAlgorithm::algorithmCompleted, this, &MainWindow::onAlgorithmCompleted);
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
    ui->algorithmsBox->addItem("BFS Algorithm");
    ui->algorithmsBox->addItem("DFS Algorithm");
    ui->algorithmsBox->addItem("Dijkstra's Algorithm");
    ui->algorithmsBox->addItem("A* Algorithm");

}

void MainWindow::setupGridView(QString gridViewName)
{

    // Setting up chartview
    ui->gridView->setObjectName(gridViewName);
    ui->gridView->setMinimumWidth(qreal(700));
    ui->gridView->setMinimumHeight(qreal(700));

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

    }else if (pathAlgorithm.simulationOnGoing){

        if (pathAlgorithm.running){
            pathAlgorithm.pauseAlgorithm();
            gridView.setSimulationRunning(false);
            ui->runButton->setChecked(false);
            ui->runButton->setText(QString("RUN"));

        }else{
            pathAlgorithm.resumeAlgorithm();
            gridView.setSimulationRunning(true);

            ui->runButton->setChecked(true);
            //ui->runButton->setText(QString("PAUSE"));
            ui->runButton->setText(QString("RUNNING"));
        }

    }else{

        pathAlgorithm.running = true;

        // set the grid node of the path algorithm object;
        pathAlgorithm.gridNodes = gridView.gridNodes;
        pathAlgorithm.heightGrid = gridView.heightGrid;
        pathAlgorithm.widthGrid = gridView.widthGrid;

        // Setting the run button as checkble and checked
        ui->runButton->setCheckable(true);
        ui->runButton->setChecked(true);
        //ui->runButton->setText(QString("PAUSE"));
        ui->runButton->setText(QString("RUNNING"));

        // Blocking the interaction with the gridView
        gridView.setSimulationRunning(true);

        // Enabling the current QScatter series point as visible
        gridView.AlgorithmView(true);

        // Call path finding
        pathAlgorithm.runAlgorithm(gridView.getCurrentAlgorithm());

    }

}

void MainWindow::on_mazeButton_clicked()
{
    gridView.setCurrentAlgorithm(BACKTRACK);
    pathAlgorithm.running = true;

    // set the grid node of the path algorithm object;
    pathAlgorithm.gridNodes = gridView.gridNodes;
    pathAlgorithm.heightGrid = gridView.heightGrid;
    pathAlgorithm.widthGrid = gridView.widthGrid;

    // Blocking the interaction with the gridView
    gridView.setSimulationRunning(true);

    // Enabling the current QScatter series point as visible
    gridView.AlgorithmView(true);

    // Call path finding
    pathAlgorithm.runAlgorithm(gridView.getCurrentAlgorithm());

}

void MainWindow::on_resetButton_clicked()
{
    // Calling populate grid with same previous arrangement
    gridView.populateGridMap(gridView.getCurrentArrangement(), true);

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


void MainWindow::onAlgorithmCompleted()
{
    gridView.setSimulationRunning(false);
    pathAlgorithm.setSimulationOnGoing(false);
    ui->runButton->setChecked(false);
    ui->runButton->setText(QString("RUN"));

    gridView.setCurrentAlgorithm(ui->algorithmsBox->currentIndex());
}

void MainWindow::on_dialWidth_valueChanged(int value)
{
    ui->lcdWidth->display(value);
}


void MainWindow::on_dialHeight_valueChanged(int value)
{
    ui->lcdHeight->display(value);

}


void MainWindow::on_sliderMarker_valueChanged(int value)
{
    ui->lcdMarker->display(value);
}


void MainWindow::on_sliderMarker_sliderReleased()
{
    // Set the new marker size
    gridView.markerSize = ui->lcdMarker->value();

    // Set the marker size of elements
    gridView.setElementsMarkerSize();
}


void MainWindow::on_dialWidth_sliderReleased()
{
    // Set the new width of the grid
    gridView.widthGrid = ui->lcdWidth->value();

    // Resetting the gridview
    gridView.populateGridMap(gridView.getCurrentArrangement(), true);
}


void MainWindow::on_dialHeight_sliderReleased()
{
    // Set the new height of the grid
    gridView.heightGrid = ui->lcdHeight->value();

    // Resetting the gridview
    gridView.populateGridMap(gridView.getCurrentArrangement(), true);

}


void MainWindow::on_mazeButton_released()
{

}


void MainWindow::on_speedSpinBox_valueChanged(int arg1)
{
    pathAlgorithm.setSpeedVizualization(ui->speedSpinBox->maximum() / arg1);
}

