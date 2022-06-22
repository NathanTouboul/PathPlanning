#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "headers/GridView.h"
#include "headers/PathAlgorithm.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class MainWindow;
}

QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:

        // Constructor
        MainWindow(QWidget *parent = nullptr);

        // Destructor
        virtual ~MainWindow();

        // Setting up objects
        void setupInteractionComboBox();
        void setupAlgorithmsComboBox();
        void setupGridView(QString gridViewName);

        // Getters
        GridView& getGridView();


public: Q_SIGNALS:
    void launchedBFS();

    public slots:

        // Run the simulation and pause it
        void on_runButton_clicked();

        // Reset the ChartView
        void on_resetButton_clicked();

        // Handles the different interactions changes
        void on_interactionBox_currentIndexChanged(int index);

        // Handles the different algorithm changes
        void on_algorithmsBox_currentIndexChanged(int index);

        // Action to do when the path algorithm is finished
        void onAlgorithmCompleted();

private slots:
        void on_dialWidth_valueChanged(int value);

        void on_dialHeight_valueChanged(int value);

private:

        Ui::MainWindow* ui;
        GridView gridView;
        PathAlgorithm pathAlgorithm;

};
#endif // MAINWINDOW_H
