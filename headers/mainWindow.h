#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <headers/GridView.h>
#include <headers/PathAlgorithm.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
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

private slots:
        void on_runButton_clicked();
        void on_resetButton_clicked();
        void on_interactionBox_currentIndexChanged(int index);
        void on_algorithmsBox_currentIndexChanged(int index);

private:

        Ui::MainWindow* ui;
        GridView gridView;
        PathAlgorithm pathAlgorithm;
};
#endif // MAINWINDOW_H
