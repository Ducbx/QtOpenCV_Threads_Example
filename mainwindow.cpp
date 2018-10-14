#include "mainwindow.h"
#include "ui_mainwindow.h"

/* Zakaria Chahboun 04/04 1:00 */
#include <QDebug>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#define MyOrgName "ZAKI PRO"
#define MyAppName "OpenCvTest2_with_QThread"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //1- Load app Settings :)
    this->setWindowIcon(QIcon(":/res/ZAKI PRO - PNG.PNG"));
    QSettings stt(MyOrgName,MyAppName,this);
    this->path = stt.value("PathName","").toString();
    this->resize(stt.value("WinSize",this->size()).toSize());

    //2- Instance : Video Preccessor Class
    this->processor = new VideoProcessorThread(this);

    //3- Init the video path :)
    processor->setVideoPath(this->path);

    // Signals/Slots (Video Processor) : Show Video
    connect(processor,&VideoProcessorThread::inDisplay,
            ui->inputVideo,&QLabel::setPixmap);
    connect(processor,&VideoProcessorThread::outDisplay,
            ui->outputVideo,&QLabel::setPixmap);

    // Signals/Slots (Video Processor) : Progressbar
    ui->progressBar->setMinimum(0);
    connect(processor,&VideoProcessorThread::progressBarSetValue,
            ui->progressBar,&QProgressBar::setValue);
    connect(processor,&VideoProcessorThread::progressBarSetMax,
            ui->progressBar,&QProgressBar::setMaximum);

    // Do somthing when (VideoProcessor) Thread is fineshed :)
    connect(processor,&VideoProcessorThread::finished,[&](){
        qDebug() << "Thread is fineshed!";

        // Decorations :)
        ui->startVideo->setEnabled(true);
        ui->pauseVideo->setEnabled(false);
        ui->stopVideo->setEnabled(false);
    });

    // Signals/Slots (Video Processor) : Set Filter
    connect(ui->buttonGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
    [=](int id){
            auto NoFilter = ui->buttonGroup->id(ui->RadioFilter_No);
            auto Gray = ui->buttonGroup->id(ui->RadioFilter_Gray);
            auto Invert = ui->buttonGroup->id(ui->RadioFilter_Invert);

            if(id == NoFilter)
                processor->setFilter(VideoProcessorThread::NoFilter);

            if(id == Gray)
                processor->setFilter(VideoProcessorThread::Gray);

            if(id == Invert)
                processor->setFilter(VideoProcessorThread::Invert);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
    processor->requestInterruption();
    processor->wait();
    processor->deleteLater();
}

// On Start Video : Click
void MainWindow::on_startVideo_clicked()
{
    // init Size of frames (Decorations) :)
    auto is = ui->inputVideo->size();
    auto os = ui->inputVideo->size();
    is.scale(is.width()-5,is.height()-5,Qt::IgnoreAspectRatio);
    os.scale(os.width()-5,os.height()-5,Qt::IgnoreAspectRatio);
    processor->setFrameSize(is,os);

    // Set State of VidePlayer (VideoProcess)
    this->processor->setReadVideoState(VideoProcessorThread::StartVideo);

    // Start the Video Processor Thread :)
    processor->start();

    //* **************** Decorations :) **************** */
    ui->startVideo->setEnabled(false);
    ui->pauseVideo->setEnabled(true);
    ui->stopVideo->setEnabled(true);
}
// On Pause Video : Click
void MainWindow::on_pauseVideo_clicked()
{
    // Pause playing video
    // and exit from Video Processor Thread (my loop) : emit (finished signal) :)
    this->processor->setReadVideoState(VideoProcessorThread::PauseVideo);

    // Decorations :)
    ui->startVideo->setEnabled(true);
    ui->pauseVideo->setEnabled(false);
    ui->stopVideo->setEnabled(true);
}

// On Close Event : make sure (exit from the thread) when app is close :)
void MainWindow::closeEvent(QCloseEvent *event){
    event->accept();

    this->processor->setReadVideoState(VideoProcessorThread::StopVideo);

    // Save app setting :)
    QSettings stt(MyOrgName,MyAppName,this);
    stt.setValue("WinSize",this->size());
    stt.setValue("PathName",this->path);
}

// On Resize Event (Scale Image)
void MainWindow::resizeEvent(QResizeEvent *ev)
{
    Q_UNUSED(ev);
    // Send Sizes to Thread (Scale Image : Video) :)
    int inW = ui->inputVideo->size().width() - 5;
    int inH = ui->inputVideo->size().height() - 5;
    int outW = ui->outputVideo->size().width() - 5;
    int outH = ui->outputVideo->size().height() - 5;

    QSize inSize(inW,inH);
    QSize outSize(outW,outH);

    processor->setFrameSize(inSize,outSize);
}

// Open FileDialog : Get Video Path
void MainWindow::on_fileNameVideo_clicked()
{
    // Pause playing video :)
    this->processor->setReadVideoState(VideoProcessorThread::PauseVideo);

    // Get video path
    auto filename = QFileDialog::getOpenFileName(this,
          tr("Open Video"), this->path,
          tr("Video Files (*.mp4 *.mkv *.avi *.webm *.wmv *.mov *.mpg *.flv *.vob *.swf *.3gp *.jpg *.png)"));

    // Check it!
    if(filename.isEmpty())
        return;

    // Save it :)
    this->path = filename;

    // Change the video path
    processor->setVideoPath(this->path);

    // And Start! :)
    this->on_startVideo_clicked();
}

// On Stop Video
void MainWindow::on_stopVideo_clicked()
{
    this->processor->setReadVideoState(VideoProcessorThread::StopVideo);

    // Decorations
    ui->startVideo->setEnabled(true);
    ui->pauseVideo->setEnabled(false);
    ui->stopVideo->setEnabled(false);
}

/************** QAction *************/
// Open Video / Image
void MainWindow::on_actionOpen_Image_Video_triggered()
{
    this->on_fileNameVideo_clicked();
}
// About
void MainWindow::on_actionCreator_triggered()
{
    QMessageBox about(this);
    about.setWindowIcon(QIcon(":/res/ZAKI PRO - PNG.PNG"));
    about.setWindowTitle("About this application");
    about.setText(
                   "Creator: <b>Zakaria Chahboun</b><br>"
                   "Version: <b>1.0</b><br>"
                   "Date: <b>10/04/2018</b><br>"
                   "Corporation: <b>ZakiPRO Corporation</b><br>"
                   "<br>"
                   "Gmail: <b>Zakaria.Chahboun.1996@Gmail.com</b><br>"
                   "Twitter: <b>@Z_Chahboun</b><br>"
                   "Phone Number 1: <b>+212 6 78 80 72 80</b><br>"
                   "Phone Number 2: <b>+212 6 80 98 21 06</b><br>"
                       );
    about.setIconPixmap(QPixmap(":/res/ZAKI PRO - PNG.PNG")
                        .scaledToWidth(200,Qt::SmoothTransformation));
    about.exec();
}
