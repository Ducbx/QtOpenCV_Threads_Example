#ifndef VIDEOPROCESSORTHREAD_H
#define VIDEOPROCESSORTHREAD_H

#include <QThread>
#include <QPixmap>

#include <cvtoqt.h> // my class :)
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>

/*
 * Zakaria Chahboun 04/04 1:00
 * MultiThread Video Processing :)
*/

class VideoProcessorThread : public QThread
{
    Q_OBJECT
public:
    explicit VideoProcessorThread(QObject *parent = 0);

    typedef enum {StartVideo, PauseVideo, StopVideo} state;
    typedef enum {NoFilter, Gray, Invert} filter;

    void setVideoPath(const QString & videopath);
    void setReadVideoState(state videostate);
    void setFrameSize(const QSize & inSize,const QSize & outSize);
    void setFilter(filter Filter);
    double getFramesCount();
    double getCurrentFramePosision();

signals:
    void inDisplay(QPixmap pixmap); // input display normal video
    void outDisplay(QPixmap pixmap); // output display process video :)
    void progressBarSetValue(int value);
    void progressBarSetMax(int max);

public slots:

private:
    void run() override;    
    QPixmap processImage(filter Filter, cv::Mat mat);
    cv::VideoCapture video;

    QString videopath;
    bool breakTheloop = false;
    QSize inFrameSize;
    QSize outFrameSize;
    filter GlobalFilter = NoFilter;
};

#endif // VIDEOPROCESSORTHREAD_H
