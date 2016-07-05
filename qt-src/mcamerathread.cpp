#define SHOW_FRAMERATE 1

#include <QCoreApplication>

#include "mcamerathread.h"
#include <locale>

#include "libAutoInit.h"   // custom generated header (with lib) from matlab code
//#include <opencvmex.hpp>

//#include "util/MxArray.hpp"

CameraTask::CameraTask(MVideoCapture* camera, QVideoFrame* videoFrame,
                       unsigned char* cvImageBuf, int width, int height)
    : running(true), camera(camera), videoFrame(videoFrame),cvImageBuf(cvImageBuf),
    width(width), height(height), curPlayState(Paused), curFrame(-1), frameToSeekTo(-1),
    startFrame(0), endFrame(0)
{
    qDebug() << "Starting initialization of matlab.";

//    qDebug() << "User-preferred locale setting is " << std::locale("").name().c_str() << '\n';
//    // on startup, the global locale is the "C" locale
//    // replace the C++ global locale as well as the C locale with the user-preferred locale
//    std::locale::global(std::locale("English_United States.1252"));
//    qDebug() << "New locale setting is " << std::locale("English_United States.1252").name().c_str() << '\n';
    const char *pStrings[]={"-nojvm","-nojit"};
    // Initialize the MATLAB Compiler Runtime global state
    if (!mclInitializeApplication(NULL,0))
    {
        qDebug() << "Could not initialize the application properly.";
    }

    qDebug() << "Initializing custom matlab library";
    if (!libAutoInitInitialize()) {
        qDebug() << "Could not initialize the autoInit library.";
    }
    qDebug() << "Done initialization stage.";
}

CameraTask::~CameraTask()
{
    qDebug() << "CameraTask destructed";
    libAutoInitTerminate();
    mclTerminateApplication();  // can only be called once per application
    //Leave camera and videoFrame alone, they will be destroyed elsewhere
}

void CameraTask::stop()
{
    running = false;
}

mwArray opencvConvertToMX(cv::Mat& m) {
    int rows=m.rows;
    int cols=m.cols;
    //Mat data is float, and mxArray uses double, so we need to convert.
    //mxArray *mxT=mxCreateDoubleMatrix(rows, cols, mxREAL);
    mwArray T(rows, cols, mxDOUBLE_CLASS);
    mxDouble *dataBuffer = new mxDouble[cols*rows]; // TODO: LEAK
    qDebug() << "rows: " << rows << " cols: " << cols << "(" << rows*cols << ")";
    //double *buffer=(double*)mxGetPr(T);
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            dataBuffer[j*(cols)+i] = m.at<uchar>(i,j);
        }
    }
    qDebug() << "SetData...";
    T.SetData(dataBuffer, cols*rows);
    qDebug() << "Done.";
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
            camera->setProperty(CV_CAP_PROP_POS_FRAMES, curFrame);
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
                cv::Rect roiRect(roi.x(), roi.y(), roi.width(), roi.height());
                cv::Mat roiSection = tempMat(roiRect).clone();  // TODO: check if leaked
                cv::cvtColor(roiSection, roiSection, CV_BGR2GRAY);
                imwrite( "cropped.jpg", roiSection);
                try {
                    //MxArray converter(roiSection);
                    qDebug() << "using libs from matlab";
                    mwArray topWall;
                    mwArray botWall;
                    mwArray mwROI = opencvConvertToMX(roiSection);
                    double bdata [] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
                    //mwArray mwROI(2,5, mxDOUBLE_CLASS);
                    //mwROI.SetData(bdata, 10);

                    mwArray numPoints(1, 1, mxINT32_CLASS);
                    int numPointsData [] = { 3 };
                    numPoints.SetData(numPointsData, 1);

                    qDebug() << "numPoints: " << numPoints.ToString();
                    autoInitializer(1, topWall, botWall, mwROI, numPoints);

                    qDebug() << "top wall: " << topWall.ToString();
                    qDebug() << "bottom wall: " << botWall.ToString();
                    //delete mwROI;

                } catch (const mwException& e) {
                    std::cerr << e.what() << std::endl;
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






MCameraThread::MCameraThread(MVideoCapture* camera, QVideoFrame* videoFrame, unsigned char* cvImageBuf, int width, int height)
{
    task = new CameraTask(camera,videoFrame,cvImageBuf,width,height);
    task->moveToThread(&workerThread);
    connect(&workerThread, SIGNAL(started()), task, SLOT(doWork()));
    connect(task, SIGNAL(imageReady(int)), this, SIGNAL(imageReady(int)));
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
