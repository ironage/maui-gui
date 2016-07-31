#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include<QDebug>
#include<QThread>
#include<QObject>
#include<QElapsedTimer>
#include<QVideoFrame>

#include<opencv2/highgui/highgui.hpp>
#include<opencv2/video/video.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/imgproc/types_c.h>

#include<vector>

#include "mdatalog.h"
#include "mpoint.h"
#include "mvideocapture.h"

class mwArray;

class CameraTask : public QObject{
Q_OBJECT

public:
    CameraTask(MVideoCapture* camera, QVideoFrame* videoFrame, unsigned char* cvImageBuf, int width, int height);
    virtual ~CameraTask();
    void stop();

private:
    enum PlayState {
        Playing,
        Paused,
        Seeking,
        AutoInitCurFrame
    };

#if defined(SHOW_FRAMERATE) && !defined(ANDROID) //Android camera has its own FPS debug info
    const float CAM_FPS_RATE = 0.9f;            ///< Rate of using the older FPS estimates
    const int CAM_FPS_PRINT_PERIOD = 500;       ///< Period of printing the FPS estimate, in milliseconds
#endif
    PlayState curPlayState;
    int width;                                  ///< Width of the camera image
    int height;                                 ///< Height of the camera image
    MVideoCapture* camera;                      ///< The camera to get data from
    bool running = false;                       ///< Whether the worker thread is running
    QVideoFrame* videoFrame;                    ///< Place to draw camera image to
    unsigned char* cvImageBuf;                  ///< Place to export camera image to
    int curFrame;
    int frameToSeekTo;
    int startFrame;
    int endFrame;
    QRect roi;
    bool doneInit;
    QList<MPoint> topPoints, bottomPoints;
    mwArray* matlabArrays;
    cv::VideoWriter outputVideo;
    MDataLog log;
    QString outputFileName;
    unsigned char* cameraFrame;
    void convertUVsp2UVp(unsigned char* __restrict srcptr, unsigned char* __restrict dstptr, int stride);

    enum MatlabArrays {
        TOP_POINTS,
        BOTTOM_POINTS,
        TOP_STRONG_POINTS,
        BOTTOM_STRONG_POINTS,
        TOP_STRONG_LINE,
        BOTTOM_STRONG_LINE,
        TOP_REF_WALL,
        BOTTOM_REF_WALL,
        SMOOTH_KERNEL,
        DERIVATE_KERNEL,
        TOP_WEAK_LINE,
        BOTTOM_WEAK_LINE,

        ARRAY_COUNT
    };

public slots:
    void doWork();
    void play();
    void continueProcessing();
    void pause();
    void seek(int frameNumber);
    void setStartFrame(int frameNumber);
    void setEndFrame(int frameNumber);
    void setROI(QRect newROI);    
    void setLogMetaData(MLogMetaData data);
signals:
    void imageReady(int);
    void initPointsDetected(QList<MPoint>, QList<MPoint>);
    void videoFinished();
protected:
    void notifyInitPoints(mwArray topWall, mwArray bottomWall, QPoint offset);
    cv::Rect getCVROI();
    void drawLine(cv::Mat& dest, mwArray& points, cv::Scalar color, QPoint offset);
    void initializeOutputVideo();
    double getFirst(mwArray& data, double defaultValue);
    void writeResults();
};

class MCameraThread : public QObject{
Q_OBJECT

public:
    MCameraThread(MVideoCapture* camera, QVideoFrame* videoFrame, unsigned char* cvImageBuf, int width, int height);
    virtual ~MCameraThread();
    void start();
    void stop();
    void doPlay();
    void doContinue();
    void doPause();
    void doSeek(int frameNumber);
    void doSetStartFrame(int frameNumber);
    void doSetEndFrame(int frameNumber);
    void doSetROI(QRect roi);
    void doSetLogMetaData(MLogMetaData m);
private:
    QThread workerThread;
    CameraTask* task = NULL;
signals:
    void imageReady(int frameNumber);
    void videoFinished();
    void play();
    void continueProcessing();
    void pause();
    void seek(int frameNumber);
    void setStartFrame(int frameNumber);
    void setEndFrame(int frameNumber);
    void setROI(QRect roi);
    void setLogMetaData(MLogMetaData d);
    void initPointsDetected(QList<MPoint>, QList<MPoint>);
};

#endif /* CAMERATHREAD_H */

