#define SHOW_FRAMERATE 1

#include <QCoreApplication>

#include "mcamerathread.h"
#include <algorithm>

// Windows includes defines for min,max which collide with std:: versions
#define NOMINMAX

// Disable warnings for unreferenced formal parameter with visual
// studio on the matlab generated header files.
#pragma warning(push)
#pragma warning( disable : 4100 )
#include "libAutoInit.h"   // custom generated header (with lib) from matlab code
#include "libMAUI.h"       // matlab generated header
#pragma warning(pop)


CameraTask::CameraTask(MVideoCapture* camera, QVideoFrame* videoFrame,
                       unsigned char* cvImageBuf, int width, int height)
    : running(true), camera(camera), videoFrame(videoFrame),cvImageBuf(cvImageBuf),
    width(width), height(height), curPlayState(Paused), curFrame(-1), frameToSeekTo(-1),
    startFrame(0), endFrame(0), doneInit(false)
{
    qDebug() << "Starting initialization of matlab.";

    const char *pStrings[]={"-nojvm","-nojit"};
    // Initialize the MATLAB Compiler Runtime global state
    if (!mclInitializeApplication(pStrings,2))
    {
        qDebug() << "Could not initialize the application properly.";
    }

    qDebug() << "Initializing autoInit matlab library";
    if (!libAutoInitInitialize()) {
        qDebug() << "Could not initialize the autoInit library.";
    }
    qDebug() << "Initializing MAUI matlab library";
    if (!libMAUIInitialize()) {
        qDebug() << "Could not initialize the maui library";
    }
    matlabArrays = new mwArray[ARRAY_COUNT];
    qDebug() << "Done initialization stage.";
}

CameraTask::~CameraTask()
{
    qDebug() << "CameraTask destructed";
    libMAUITerminate();
    libAutoInitTerminate();
    mclTerminateApplication();  // can only be called once per application
    delete [] matlabArrays;
    //Leave camera and videoFrame alone, they will be destroyed elsewhere
}

void CameraTask::stop()
{
    running = false;
}

mwArray* opencvConvertToMX(cv::Mat& m) {
    int rows=m.rows;
    int cols=m.cols;
    mwArray *T = new mwArray(rows, cols, mxDOUBLE_CLASS);
    mxDouble *dataBuffer = new mxDouble[cols*rows]; // TODO: allocation on size change only
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            //        [i*cols+j]  row vs col order is reversed from cv::Mat to mwArray
            dataBuffer[j*rows+i] = m.at<uchar>(i,j);
        }
    }
    T->SetData(dataBuffer, cols*rows);
    delete [] dataBuffer;
    return T;
}

void CameraTask::convertUVsp2UVp(unsigned char* __restrict srcptr, unsigned char* __restrict dstptr, int stride)
{
    for(int i=0;i<stride;i++){
        dstptr[i]           = srcptr[i*2];
        dstptr[i + stride]  = srcptr[i*2 + 1];
    }
}

void CameraTask::doWork()
{

#if defined(SHOW_FRAMERATE) && !defined(ANDROID)
    QElapsedTimer timer;
    float fps = 0.0f;
    int millisElapsed = 0;
    int millis;
    timer.start();
#endif

    if(videoFrame)
        videoFrame->map(QAbstractVideoBuffer::ReadOnly);

    //Assuming desktop, RGB camera image and RGBA QVideoFrame
    cv::Mat screenImage;
    if(videoFrame)
        screenImage = cv::Mat(height,width,CV_8UC4,videoFrame->bits());

    while(running && videoFrame != NULL && camera != NULL) {
         QCoreApplication::processEvents();

        switch (curPlayState) {
        case PlayState::Paused:
            QThread::msleep(10);
            continue; // loop
        case PlayState::AutoInitCurFrame:
            camera->setProperty(CV_CAP_PROP_POS_FRAMES, curFrame); // FIXME: cache current frame
            break;
        case PlayState::Seeking:
            camera->setProperty(CV_CAP_PROP_POS_FRAMES, frameToSeekTo);
            curPlayState = PlayState::Paused;
            // fall through
        case PlayState::Playing:
        default:
            break; // switch
        }

        curFrame = camera->getProperty(CV_CAP_PROP_POS_FRAMES);
        if (curPlayState == PlayState::Playing && curFrame > endFrame) {
            curPlayState = PlayState::Paused;
            continue;
        }

        if(!camera->grabFrame())
            continue;
        unsigned char* cameraFrame = camera->retrieveFrame();

        //Get camera image into screen frame buffer
        //Assuming desktop, RGB camera image and RGBA QVideoFrame
        if(videoFrame) {

            cv::Mat tempMat(height, width, CV_8UC3, cameraFrame);
            if (curPlayState == PlayState::AutoInitCurFrame) {
                curPlayState = PlayState::Paused;
                // Trying to crop an image outside of bounds will crash, bound check here
                int roiX = std::max(0, roi.x());
                int roiY = std::max(0, roi.y());
                int roiW = std::min(width - roiX - 1, roi.width());
                int roiH = std::min(height - roiY - 1, roi.height());
                cv::Rect roiRect(roiX, roiY, roiW, roiH);
                cv::Mat roiSection = tempMat(roiRect);  // TODO: check if leaked
                cv::cvtColor(roiSection, roiSection, CV_BGR2GRAY);
                //imwrite( "cropped.jpg", roiSection);
                try {
                    mwArray* mwROI = opencvConvertToMX(roiSection);
                    mwArray numPoints(1, 1, mxINT32_CLASS);
                    int numPointsData [] = { 3 };
                    numPoints.SetData(numPointsData, 1);

                    const int numReturnValues = 2;
                    autoInitializer(numReturnValues, matlabArrays[TOP_STRONG_POINTS],
                                    matlabArrays[BOTTOM_STRONG_POINTS], *mwROI, numPoints);

                    notifyInitPoints(matlabArrays[TOP_STRONG_POINTS], matlabArrays[BOTTOM_STRONG_POINTS],
                                     QPoint(roiX, roiY));
                    delete mwROI;
                } catch (const mwException& e) {
                    std::cerr << "exception caught: " << e.what() << std::endl;
                }
            } else if (curPlayState == PlayState::Playing) {
                if (!doneInit) {
                    const int numReturnValues = 6;
                    setup(numReturnValues, matlabArrays[SMOOTH_KERNEL], matlabArrays[DERIVATE_KERNEL],
                          matlabArrays[TOP_STRONG_LINE], matlabArrays[BOTTOM_STRONG_LINE],
                          matlabArrays[TOP_REF_WALL], matlabArrays[BOTTOM_REF_WALL],
                          matlabArrays[TOP_STRONG_POINTS], matlabArrays[BOTTOM_STRONG_POINTS]);
                    doneInit = true;
                    qDebug() << "smooth: " << matlabArrays[SMOOTH_KERNEL].ToString()
                             << "\nderivate: " << matlabArrays[DERIVATE_KERNEL].ToString()
                             << "\ntopStrongLine: " << matlabArrays[TOP_STRONG_POINTS].ToString()
                             << "\nbottomStrongLine: " << matlabArrays[BOTTOM_STRONG_LINE].ToString()
                             << "\ntopRefWall: " << matlabArrays[TOP_REF_WALL].ToString()
                             << "\nbottomRefWall: " << matlabArrays[BOTTOM_REF_WALL].ToString();
                }
            }
            cv::cvtColor(tempMat,screenImage,cv::COLOR_RGB2RGBA);
        }

        //Export camera image
        if(cvImageBuf){
            memcpy(cvImageBuf,cameraFrame,height*width*3);
        }

        emit imageReady(curFrame);

#if defined(SHOW_FRAMERATE) && !defined(ANDROID)
        millis = (int)timer.restart();
        millisElapsed += millis;
        fps = CAM_FPS_RATE*fps + (1.0f - CAM_FPS_RATE)*(1000.0f/millis);
        if(millisElapsed >= CAM_FPS_PRINT_PERIOD){
            qDebug("Camera is running at %f FPS",fps);
            millisElapsed = 0;
        }
#endif
    }
}

void CameraTask::play()
{
    curPlayState = PlayState::Playing;
}

void CameraTask::pause()
{
    curPlayState = PlayState::Paused;
}

void CameraTask::seek(int frameNumber)
{
    if (frameToSeekTo != frameNumber) {
        frameToSeekTo = frameNumber;
        curPlayState = PlayState::Seeking;
    } else {
        //curPlayState = PlayState::Paused;
    }
}

void CameraTask::setStartFrame(int frameNumber)
{
    startFrame = frameNumber;
    seek(frameNumber);
}

void CameraTask::setEndFrame(int frameNumber)
{
    endFrame = frameNumber;
    seek(frameNumber);
}

void CameraTask::setROI(QRect newROI)
{
    if (roi != newROI) {
        roi = newROI;
        curPlayState = PlayState::AutoInitCurFrame;
    }
}

void CameraTask::notifyInitPoints(mwArray topWall, mwArray bottomWall, QPoint offset)
{
    topPoints.clear();
    bottomPoints.clear();
    size_t topSize = topWall.NumberOfElements();
    if (topSize % 2 == 0) {  // Assuming two dimensions
        mxInt32 *topData = new mxInt32[topSize];
        topWall.GetData(topData, topSize);
        for (int i = 0; i < topSize / 2; ++i) {
            MPoint p(topData[i] + offset.x(), topData[(topSize/2) + i] + offset.y());
            topPoints.push_back(p);
        }
        delete [] topData;
    }
    size_t bottomSize = bottomWall.NumberOfElements();
    if (bottomSize % 2 == 0) {  // Assuming two dimensions
        mxInt32 *bottomData = new mxInt32[bottomSize];
        bottomWall.GetData(bottomData, bottomSize);
        for (int i = 0; i < bottomSize / 2; ++i) {
            MPoint p(bottomData[i] + offset.x(), bottomData[(bottomSize/2) + i] + offset.y());
            bottomPoints.push_back(p);
        }
        delete [] bottomData;
    }

//    qDebug() << "top wall: " << topWall.ToString();
//    qDebug() << "bottom wall: " << bottomWall.ToString();
//    qDebug() << "converted top: " << topPoints;
//    qDebug() << "converted bottom: " << bottomPoints;
    emit initPointsDetected(topPoints, bottomPoints);
}






MCameraThread::MCameraThread(MVideoCapture* camera, QVideoFrame* videoFrame, unsigned char* cvImageBuf, int width, int height)
{
    task = new CameraTask(camera,videoFrame,cvImageBuf,width,height);
    task->moveToThread(&workerThread);
    connect(&workerThread, SIGNAL(started()), task, SLOT(doWork()));
    connect(task, SIGNAL(imageReady(int)), this, SIGNAL(imageReady(int)));
    connect(task, SIGNAL(initPointsDetected(QList<MPoint>,QList<MPoint>)), this, SIGNAL(initPointsDetected(QList<MPoint>,QList<MPoint>)));
    connect(this, SIGNAL(play()), task, SLOT(play()), Qt::QueuedConnection);
    connect(this, SIGNAL(pause()), task, SLOT(pause()), Qt::QueuedConnection);
    connect(this, SIGNAL(seek(int)), task, SLOT(seek(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setStartFrame(int)), task, SLOT(setStartFrame(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setEndFrame(int)), task, SLOT(setEndFrame(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setROI(QRect)), task, SLOT(setROI(QRect)), Qt::QueuedConnection);
}

MCameraThread::~MCameraThread()
{
    stop();
    delete task;
}

void MCameraThread::start()
{
    workerThread.start();
}

void MCameraThread::stop()
{
    if(task != NULL)
        task->stop();
    workerThread.wait(1000);
    workerThread.quit();
    workerThread.wait();
}

void MCameraThread::doPlay()
{
    emit play();
}
void MCameraThread::doPause()
{
    emit pause();
}

void MCameraThread::doSeek(int frameNumber)
{
    emit seek(frameNumber);
}

void MCameraThread::doSetStartFrame(int frameNumber)
{
    emit setStartFrame(frameNumber);
}

void MCameraThread::doSetEndFrame(int frameNumber)
{
    emit setEndFrame(frameNumber);
}

void MCameraThread::doSetROI(QRect roi)
{
    emit setROI(roi);
}
