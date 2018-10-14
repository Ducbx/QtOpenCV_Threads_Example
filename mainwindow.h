#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QResizeEvent>

// VideoProcessor (myclass with Thread) :)
#include "videoprocessorthread.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent * ev);
    ~MainWindow();

private slots:
    void on_startVideo_clicked();

    void on_fileNameVideo_clicked();

    void on_pauseVideo_clicked();

    void on_stopVideo_clicked();

    void on_actionOpen_Image_Video_triggered();

    void on_actionCreator_triggered();

private:
    Ui::MainWindow *ui;

    VideoProcessorThread *processor;
    QString path;
};

#endif // MAINWINDOW_H
