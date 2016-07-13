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

    void convertUVsp2UVp(unsigned char* __restrict srcptr, unsigned char* __restrict dstptr, int stride);

public slots:
    void doWork();
    void play();
    void pause();
    void seek(int frameNumber);
    void setStartFrame(int frameNumber);
    void setEndFrame(int frameNumber);
    void setROI(QRect newROI);    
signals:
    void imageReady(int);
    void initPointsDetected(QList<MPoint>, QList<MPoint>);
protected:
    void notifyInitPoints(mwArray topWall, mwArray bottomWall, QPoint offset);
};

class MCameraThread : public QObject{
Q_OBJECT

public:
    MCameraThread(MVideoCapture* camera, QVideoFrame* videoFrame, unsigned char* cvImageBuf, int width, int height);
    virtual ~MCameraThread();
    void start();
    void stop();
    void doPlay();
    void doPause();
    void doSeek(int frameNumber);
    void doSetStartFrame(int frameNumber);
    void doSetEndFrame(int frameNumber);
    void doSetROI(QRect roi);
private:
    QThread workerThread;
    CameraTask* task = NULL;
signals:
    void imageReady(int frameNumber);
    void play();
    void pause();
    void seek(int frameNumber);
    void setStartFrame(int frameNumber);
    void setEndFrame(int frameNumber);
    void setROI(QRect roi);
    void initPointsDetected(QList<MPoint>, QList<MPoint>);
};

#endif /* CAMERATHREAD_H */

