#include "videoprocessorthread.h"

#include <QDebug>

VideoProcessorThread::VideoProcessorThread(QObject *parent) : QThread(parent)
{
    this->video = cv::VideoCapture(this->videopath.toStdString());
}

// Set Video Path
void VideoProcessorThread::setVideoPath(const QString &videopath)
{
    this->videopath = videopath;
    // init
    this->video = cv::VideoCapture(this->videopath.toStdString());
}

// Set Video State : Start/Pause/Stop :)
void VideoProcessorThread::setReadVideoState(state videostate)
{

    /* Start State */
    if(videostate ==  StartVideo){
        this->breakTheloop = false;

        // If video end : go to the begin :)
        if(getCurrentFramePosision() == getFramesCount())
            video.set(cv::CAP_PROP_POS_FRAMES,0);
    }

    /* Pause State */
    else if(videostate ==  PauseVideo){
        this->breakTheloop = true; // just pause :)
    }

    /* Stop State */
    else if(videostate ==  StopVideo){
        // Set Stop (break point)
        this->breakTheloop = true;
        // Init the video :)
        this->video = cv::VideoCapture(this->videopath.toStdString());
    }
}

// Set Frame Size
void VideoProcessorThread::setFrameSize(const QSize &inSize, const QSize &outSize)
{
    this->inFrameSize = inSize;
    this->outFrameSize = outSize;
}

// Image Processing : Filtring, GrayScale, Invert ....
QPixmap VideoProcessorThread::processImage(filter Filter, cv::Mat mat)
{
    if(Filter == NoFilter){
        return QPixmap::fromImage(CvToQt::MatToQImage(mat))
                .scaled(outFrameSize,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    }

    else if(Filter == Gray){
        cv::cvtColor(mat, mat, CV_BGR2GRAY);
        return QPixmap::fromImage(CvToQt::MatToQImage(mat,true))
                .scaled(outFrameSize,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    }

    else if(Filter == Invert){
        cv::bitwise_not(mat, mat);
        return QPixmap::fromImage(CvToQt::MatToQImage(mat))
                .scaled(outFrameSize,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    }
}

// Set filter type
void VideoProcessorThread::setFilter(filter Filter)
{
    this->GlobalFilter = Filter;
}

// Get Frames Count
double VideoProcessorThread::getFramesCount()
{
    return this->video.get(cv::CAP_PROP_FRAME_COUNT);
}

// Get Current Frame Position
double VideoProcessorThread::getCurrentFramePosision()
{
    return this->video.get(cv::CAP_PROP_POS_FRAMES);
}

void VideoProcessorThread::run()
{
    cv::Mat frame;

    // Set Max values of Progressbar
    emit progressBarSetMax((int)getFramesCount());

    while(video.isOpened() && !isInterruptionRequested())
    {
        // set break point :)
        if(this->breakTheloop)
            break;

        // so .. Go on !
        video >> frame;
        if(frame.empty())
            break;

        // Process (Invert Color) :)
        //cv::bitwise_not(inFrame, outFrame);
        //cv::cvtColor(inFrame, outFrame, CV_BGR2GRAY);

        // Show Video (frames)
        emit inDisplay(this->processImage(NoFilter,frame));

        emit outDisplay(this->processImage(this->GlobalFilter,frame));

        // Send current position to Progressbar
        emit progressBarSetValue((int)getCurrentFramePosision());
    }

    // start loop (to stop loop call "quit() or exit()"
   // exec();
}

