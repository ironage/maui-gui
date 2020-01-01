#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include <QDebug>
#include <QList>
#include <QThread>
#include <QObject>
#include <QElapsedTimer>
#include <QVideoFrame>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>

#include <vector>

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

    enum ProcessingState {
        SUCCESS,
        AUTO_INIT_FAILED,
        VELOCITY_INIT_FAILED
    };
    enum SetupState { // mutually exclusive bits for & checks
        NONE,
        NORMAL_ROI,
        VELOCITY_ROI,
        ALL
    };

private:
    enum PlayState {
        Playing,
        Paused,
        Seeking,
        AutoInitCurFrame,
        AutoInitVelocityInFrame,
        RedrawCurFrame
    };
    struct VelocityState {
        double videoType = -1;
        double firstMovingFrame = -1;
        double xAxisLocation = -1;
        double previousMaxXTrackingLoc = 1;
    };

#if defined(SHOW_FRAMERATE) && !defined(ANDROID) //Android camera has its own FPS debug info
    const float CAM_FPS_RATE = 0.9f;            ///< Rate of using the older FPS estimates
    const int CAM_FPS_PRINT_PERIOD = 500;       ///< Period of printing the FPS estimate, in milliseconds
#endif
    PlayState curPlayState;
    SetupState curSetupState;
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
    QRect velocityROI;
    VelocityState curVelocityState;
    bool autoRecomputeROI;
    bool doneInit;
    QList<MPoint> topPoints, bottomPoints;
    mwArray* matlabArrays;
    MDataLog log;
    QString outputFileName;
    unsigned char* cameraFrame;
    bool cachedFrameIsDirty;
    bool doProcessOutputVideo;
    int processingMillisecondsSinceStart;
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
    void setVelocityROI(QRect newROI);
    void refreshROIOnCurFrame();
    void setRecomputeROIMode(bool mode);
    void setLogMetaData(MLogMetaData data);
    void setProcessOutputVideo(bool doProcess);
    bool getDoProcessOutputVideo() { return doProcessOutputVideo; }
    void setSetupState(CameraTask::SetupState state);
    int getSetupState() { return curSetupState; }
    void setNewTopPoints(QList<MPoint> points);
    void setNewBottomPoints(QList<MPoint> points);
    int getprocessingMillisecondsSinceStart();
signals:
    void imageReady(int);
    void initPointsDetected(QList<MPoint>, QList<MPoint>);
    void videoFinished(CameraTask::ProcessingState state);
    void outputProgress(int);
protected:
    void notifyInitPoints(mwArray topWall, mwArray bottomWall, QPoint offset);
    cv::Rect getCVROI(QRect rect);
    void drawLine(cv::Mat &dest, const std::vector<cv::Point>& points, cv::Scalar color, int thickness = 1);
    void initializeOutput();
    double getFirst(mwArray& data, double defaultValue);
    void writeResults();
    bool autoInitializeOnROI(mwArray* matlabROI);
    bool initializeVelocityROI(mwArray* velocityCurrentROI, mwArray* velocityPreviousROI, bool findFirstFrame);
    VelocityResults getVelocityFromFrame(mwArray* velocityCurrentROI, int frame, VelocityState velocityState);
    int getIndexOfFirstMovingFrame();
    bool getNextFrameData();
    void drawOverlay(int frame, cv::Mat &mat);
    cv::Point makeSafePoint(int x, int y, const cv::Point& offset = cv::Point(0,0));
    void processOutputVideo();
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
    void doSetVelocityROI(QRect roi);
    void doForceROIRefresh();
    void doSetRecomputeROIMode(bool mode);
    void doSetLogMetaData(MLogMetaData m);
    void doSetProcessOutputVideo(bool process);
    bool doGetProcessOutputVideo();
    void doSetSetupState(CameraTask::SetupState state);
    int doGetSetupState();
    int doGetprocessingMillisecondsSinceStart();
    void doSetNewTopPoints(QList<MPoint> points);
    void doSetNewBottomPoints(QList<MPoint> points);
private:
    QThread workerThread;
    CameraTask* task = NULL;
signals:
    void imageReady(int frameNumber);
    void videoFinished(CameraTask::ProcessingState state);
    void play();
    void continueProcessing();
    void pause();
    void seek(int frameNumber);
    void setStartFrame(int frameNumber);
    void setEndFrame(int frameNumber);
    void setROI(QRect roi);
    void setVelocityROI(QRect roi);
    void forceROIRefresh();
    void setNewTopPoints(QList<MPoint> points);
    void setNewBottomPoints(QList<MPoint> points);
    void setRecomputeROIMode(bool mode);
    void setLogMetaData(MLogMetaData d);
    void setProcessOutputVideo(bool process);
    void initPointsDetected(QList<MPoint>, QList<MPoint>);
    void outputProgress(int);
    void setSetupState(CameraTask::SetupState state);
};

Q_DECLARE_METATYPE(CameraTask::ProcessingState)
Q_DECLARE_METATYPE(CameraTask::SetupState)


#endif /* CAMERATHREAD_H */

