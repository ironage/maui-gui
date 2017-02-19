#define SHOW_FRAMERATE 1

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>

#include "mcamerathread.h"
#include <algorithm>
#include <memory>

// Windows includes defines for min,max which collide with std:: versions
#define NOMINMAX

// Disable warnings for unreferenced formal parameter with visual
// studio on the matlab generated header files.
#pragma warning(push)
#pragma warning( disable : 4100 )
#include "MAUIVelocityDllWithAutoInit.h"   // custom generated header (with lib) from matlab code
#pragma warning(pop)


CameraTask::CameraTask(MVideoCapture* camera, QVideoFrame* videoFrame,
                       unsigned char* cvImageBuf, int width, int height)
    : running(true), camera(camera), videoFrame(videoFrame),cvImageBuf(cvImageBuf),
    width(width), height(height), curPlayState(Paused), curFrame(-1), frameToSeekTo(-1),
    startFrame(0), endFrame(0), autoRecomputeROI(false), doneInit(false),
    cameraFrame(nullptr), cachedFrameIsDirty(true), doProcessOutputVideo(true)
{
    qRegisterMetaType<CameraTask::ProcessingState>();
    matlabArrays = new mwArray[ARRAY_COUNT];
}

CameraTask::~CameraTask()
{
    qDebug() << "CameraTask destructed";
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

std::vector<cv::Point> convertMWArray(mwArray &points, QPoint offset) {
    size_t numElements = points.NumberOfElements();
    std::vector<cv::Point> result;
    if (numElements > 0 && numElements % 2 == 0) { // Assuming two dimensions
        size_t numPoints = numElements / 2;
        mxInt32 *data = new mxInt32[numElements];
        points.GetData(data, numElements);
        for (int i = 0; i < numPoints; ++i) {
            result.emplace_back(cv::Point(data[i] + offset.x(), data[numPoints + i] + offset.y()));
        }
        delete [] data;
    }
    return result;
}

void CameraTask::convertUVsp2UVp(unsigned char* __restrict srcptr, unsigned char* __restrict dstptr, int stride)
{
    for(int i=0;i<stride;i++){
        dstptr[i]           = srcptr[i*2];
        dstptr[i + stride]  = srcptr[i*2 + 1];
    }
}

void CameraTask::drawOverlay(int frame, cv::Mat& mat) {
    const MDataEntry *existing = log.get(frame);
    if (existing) {
        cv::Scalar strongColor(0, 0, 250);  // b,g,r
        cv::Scalar weakColor(0, 250, 0);    // b,g,r
        drawLine(mat, existing->getTopStrongLine(), strongColor);
        drawLine(mat, existing->getTopWeakLine(), weakColor);
        drawLine(mat, existing->getBottomStrongLine(), strongColor);
        drawLine(mat, existing->getBottomWeakLine(), weakColor);
        cachedFrameIsDirty = true; // now that we have drawn on this frame, force re-load cache next time
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

    double frameRate = 1;
    if (camera) {
        frameRate = camera->getProperty(CV_CAP_PROP_FPS);
        if (frameRate == 0) frameRate = 1;
    }
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
            break;
        case PlayState::Seeking:
            camera->setProperty(CV_CAP_PROP_POS_FRAMES, frameToSeekTo);
            // fall through
        case PlayState::Playing:
        default:
            break; // switch
        }

        curFrame = camera->getProperty(CV_CAP_PROP_POS_FRAMES); // opencv frame is 0 indexed
        if (curPlayState == PlayState::AutoInitCurFrame) {
            if (!cameraFrame || cachedFrameIsDirty) {
                if (!getNextFrameData()) continue;
                camera->setProperty(CV_CAP_PROP_POS_FRAMES, curFrame); // AutoInit does not advance frames
            }
        } else {
            if (!getNextFrameData()) continue;
            if (curPlayState == PlayState::Seeking) {
                curPlayState = PlayState::Paused;
                camera->setProperty(CV_CAP_PROP_POS_FRAMES, curFrame); // Seek does not advance frames
            }
        }

        //Get camera image into screen frame buffer
        //Assuming desktop, RGB camera image and RGBA QVideoFrame
        if(videoFrame) {
            cv::Mat tempMat(height, width, CV_8UC3, cameraFrame);
            cv::Mat roiSection = tempMat(getCVROI());
            cv::cvtColor(roiSection, roiSection, CV_BGR2GRAY);
            std::unique_ptr<mwArray> matlabROI(opencvConvertToMX(roiSection));

            if (curPlayState == PlayState::Paused) {
                if (autoRecomputeROI) {
                    autoInitializeOnROI(matlabROI.get());
                }
            } else if (curPlayState == PlayState::AutoInitCurFrame) {
                curPlayState = PlayState::Paused;
                autoInitializeOnROI(matlabROI.get());
            } else if (curPlayState == PlayState::Playing) {
                if (!doneInit) {
                    bool autoInitSuccess = autoInitializeOnROI(matlabROI.get());
                    if (!autoInitSuccess || topPoints.empty() || bottomPoints.empty()) {
                        emit videoFinished(CameraTask::ProcessingState::AUTO_INIT_FAILED);
                        curPlayState = PlayState::Paused;
                        camera->setProperty(CV_CAP_PROP_POS_FRAMES, curFrame); // this frame needs reprocessing
                        cachedFrameIsDirty = true;
                        continue;
                    }
                    const int numReturnValues = 6;
                    setup(numReturnValues,
                          matlabArrays[SMOOTH_KERNEL], matlabArrays[DERIVATE_KERNEL],
                          matlabArrays[TOP_STRONG_LINE], matlabArrays[BOTTOM_STRONG_LINE],
                          matlabArrays[TOP_REF_WALL], matlabArrays[BOTTOM_REF_WALL],
                          matlabArrays[TOP_STRONG_POINTS], matlabArrays[BOTTOM_STRONG_POINTS]);
                    doneInit = true;
                }

                try {
                    mwArray outerLumenDiameter, topIMT, bottomIMT;
                    const int numReturnValues = 9;

                    update(numReturnValues,
                           matlabArrays[TOP_STRONG_LINE], matlabArrays[BOTTOM_STRONG_LINE],
                           outerLumenDiameter,  matlabArrays[TOP_WEAK_LINE],
                           topIMT, matlabArrays[BOTTOM_WEAK_LINE],
                           bottomIMT, matlabArrays[TOP_REF_WALL], matlabArrays[BOTTOM_REF_WALL],
                           *matlabROI, matlabArrays[SMOOTH_KERNEL], matlabArrays[DERIVATE_KERNEL],
                           matlabArrays[TOP_STRONG_LINE], matlabArrays[BOTTOM_STRONG_LINE],
                           matlabArrays[TOP_STRONG_POINTS], matlabArrays[BOTTOM_STRONG_POINTS],
                           matlabArrays[TOP_REF_WALL], matlabArrays[BOTTOM_REF_WALL]);

                    QPoint offset(std::max(0, roi.x()), std::max(0, roi.y()));
                    log.add(MDataEntry(
                                curFrame + 1,
                                getFirst(outerLumenDiameter, 0),
                                getFirst(topIMT, 0),
                                getFirst(bottomIMT, 0),
                                (1/frameRate) * curFrame,
                                convertMWArray(matlabArrays[TOP_STRONG_LINE], offset),
                                convertMWArray(matlabArrays[TOP_WEAK_LINE], offset),
                                convertMWArray(matlabArrays[BOTTOM_STRONG_LINE], offset),
                                convertMWArray(matlabArrays[BOTTOM_WEAK_LINE], offset)));
                } catch (const mwException& e) {
                    std::cerr << "exception caught: " << e.what() << std::endl;
                }
            }

            drawOverlay(curFrame + 1, tempMat);
            cv::cvtColor(tempMat,screenImage,cv::COLOR_RGB2RGBA);
        }

        //Export camera image
        if(cvImageBuf){
            memcpy(cvImageBuf,cameraFrame,height*width*3);
        }

        emit imageReady(curFrame);

        if (curPlayState == PlayState::Playing && curFrame >= endFrame) {
            curPlayState = PlayState::Paused;
            camera->setProperty(CV_CAP_PROP_POS_FRAMES, endFrame);
            writeResults();
        }

#if defined(SHOW_FRAMERATE) && !defined(ANDROID)
        millis = (int)timer.restart();
        millisElapsed += millis;
        fps = CAM_FPS_RATE*fps + (1.0f - CAM_FPS_RATE)*(1000.0f/millis);
        if (millisElapsed >= CAM_FPS_PRINT_PERIOD) {
            qDebug("Camera is running at %f FPS",fps);
            millisElapsed = 0;
        }
#endif
    }
}

void CameraTask::play()
{
    if (curPlayState != PlayState::Playing) {
        log.clear();
        curPlayState = PlayState::Playing;
        doneInit = false;
        if (camera) {
            camera->setProperty(CV_CAP_PROP_POS_FRAMES, startFrame);
        }
    }
}

void CameraTask::continueProcessing() {
    if (curPlayState != PlayState::Playing) {
        curPlayState = PlayState::Playing;
    }
}

void CameraTask::pause()
{
    curPlayState = PlayState::Paused;
    if (camera) {
        camera->setProperty(CV_CAP_PROP_POS_FRAMES, curFrame); // review frame
    }
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
        if (curPlayState == PlayState::Paused) {
            curPlayState = PlayState::AutoInitCurFrame;
        }
    }
}

void CameraTask::refreshROIOnCurFrame()
{
    if (curPlayState == PlayState::Paused) {
        curPlayState = PlayState::AutoInitCurFrame;
    }
}

void CameraTask::setRecomputeROIMode(bool mode)
{
    autoRecomputeROI = mode;
}

void CameraTask::setLogMetaData(MLogMetaData data)
{
    log.initialize(data);
}

void CameraTask::setProcessOutputVideo(bool doProcess)
{
    doProcessOutputVideo = doProcess;
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

cv::Rect CameraTask::getCVROI()
{
    // Trying to crop an image outside of bounds will crash, bound check here
    int roiX = std::max(0, roi.x());
    if (roiX >= width) roiX = width - 1;
    int roiY = std::max(0, roi.y());
    if (roiY >= height) roiY = height - 1;

    int roiW = std::min(width - roiX - 1, roi.width());
    if (roiW < 0) roiW = 0;
    int roiH = std::min(height - roiY - 1, roi.height());
    if (roiH < 0) roiH = 0;
    return cv::Rect(roiX, roiY, roiW, roiH);
}

void CameraTask::drawLine(cv::Mat &dest, const std::vector<cv::Point>& points, cv::Scalar color)
{
    int numPoints = int(points.size());
    for (int i = 0; i < numPoints - 1; ++i) {
        cv::line(dest, points[i], points[i+1], color, 1, CV_AA); // antialised line of thickness 1
    }
}

void CameraTask::initializeOutput()
{
    MLogMetaData metaData = log.getMetaData();
    std::string outputDirName(metaData.getOutputDir().isEmpty() ? metaData.getFilePath().toStdString() : metaData.getOutputDir().toStdString());
    if (!QDir(outputDirName.c_str()).exists()) {
        QDir().mkdir(outputDirName.c_str());
    }
    outputFileName = QString::fromStdString(outputDirName) + "/" + (QFileInfo(metaData.getFileName()).completeBaseName());
}

void CameraTask::processOutputVideo() {
    double frameRate = 1;
    if (camera) {
        frameRate = camera->getProperty(CV_CAP_PROP_FPS);
        if (frameRate == 0) frameRate = 1;
        camera->setProperty(CV_CAP_PROP_POS_FRAMES, startFrame);
    }
    cv::Size videoSize(width, height);
    // the following line didn't work correctly for some videos
    //int ex = static_cast<int>(camera->getProperty(CV_CAP_PROP_FOURCC));     // Get Codec Type- Int form
    int ex = CV_FOURCC('M', 'J', 'P', 'G');
    std::string outputName = outputFileName.toStdString() + "_tracking" + log.getMetaData().getWriteTime().toStdString() + ".avi";
    cv::VideoWriter outputVideo;
    bool success = outputVideo.open(outputName, ex, frameRate, videoSize, true);
    qDebug() << "opening output video: " << QString::fromStdString(outputName) << " success ? " << success << " (ex: " << ex << ")";

    //Assuming desktop, RGB camera image and RGBA QVideoFrame
    while(running && camera != NULL && doProcessOutputVideo) {
        curFrame = camera->getProperty(CV_CAP_PROP_POS_FRAMES); // opencv frame is 0 indexed

        if (!camera->grabFrame()) {
            break;
        }
        cameraFrame = camera->retrieveFrame();

        cv::Mat tempMat(height, width, CV_8UC3, cameraFrame);

        drawOverlay(curFrame + 1, tempMat);
        if (outputVideo.isOpened()) {
            outputVideo << tempMat;
        }
        if (endFrame - startFrame > 0) {
            int progress = ((curFrame - startFrame) * 100) / (endFrame - startFrame);
            emit outputProgress(progress);
        } else {
            qDebug() << "End frame is zero, not showing progress.";
        }
        if (curFrame >= endFrame) {
            break;
        }
        QCoreApplication::processEvents(); // check for incoming signals about canceling output video write
    }
    if (camera) {
        camera->setProperty(CV_CAP_PROP_POS_FRAMES, endFrame);
    }
    if (outputVideo.isOpened()) {
        outputVideo.release(); // flush file and reset
    }
}

double CameraTask::getFirst(mwArray &data, double defaultValue)
{
    size_t numElements = data.NumberOfElements();
    if (numElements >= 1) {
        data.GetData(&defaultValue, 1);
    }
    return defaultValue;
}

void CameraTask::writeResults()
{
    initializeOutput();
    log.write(outputFileName + "_data");
    if (doProcessOutputVideo) {
        processOutputVideo();
    }
    emit videoFinished(CameraTask::ProcessingState::SUCCESS);
}

bool CameraTask::autoInitializeOnROI(mwArray *matlabROI)
{
    try {
        mwArray numPoints(1, 1, mxINT32_CLASS);
        int numPointsData [] = { 5 };
        numPoints.SetData(numPointsData, 1);
        mwArray kerUpHeight, kerBotHeight;


        const int numReturnValues = 4;
        autoInitializer(numReturnValues, matlabArrays[TOP_STRONG_POINTS],
                        matlabArrays[BOTTOM_STRONG_POINTS], kerUpHeight, kerBotHeight, *matlabROI, numPoints);

        notifyInitPoints(matlabArrays[TOP_STRONG_POINTS], matlabArrays[BOTTOM_STRONG_POINTS],
                         QPoint(roi.x(), roi.y()));
        doneInit = false;
    } catch (const mwException& e) {
        std::cerr << "exception caught: " << e.what() << std::endl;
        return false;
    }
    return true;
}

bool CameraTask::getNextFrameData()
{
    if (!camera->grabFrame()) {
        if (curPlayState == PlayState::Playing) {
            camera->setProperty(CV_CAP_PROP_POS_FRAMES, endFrame);
            writeResults();
            qDebug() << "Could not grab next video frame";
        } else {
            qDebug() << "Something is fatally wrong if we couldn't refresh the current frame! " << curFrame;
        }
        curPlayState = PlayState::Paused;
        return false;
    }
    cameraFrame = camera->retrieveFrame();
    cachedFrameIsDirty = false;
    return true;
}








MCameraThread::MCameraThread(MVideoCapture* camera, QVideoFrame* videoFrame, unsigned char* cvImageBuf, int width, int height)
{
    task = new CameraTask(camera,videoFrame,cvImageBuf,width,height);
    task->moveToThread(&workerThread);
    connect(&workerThread, SIGNAL(started()), task, SLOT(doWork()));
    connect(task, SIGNAL(imageReady(int)), this, SIGNAL(imageReady(int)));
    connect(task, SIGNAL(initPointsDetected(QList<MPoint>,QList<MPoint>)), this, SIGNAL(initPointsDetected(QList<MPoint>,QList<MPoint>)));
    connect(task, SIGNAL(videoFinished(CameraTask::ProcessingState)), this, SIGNAL(videoFinished(CameraTask::ProcessingState)));
    connect(task, SIGNAL(outputProgress(int)), this, SIGNAL(outputProgress(int)));
    connect(this, SIGNAL(play()), task, SLOT(play()), Qt::QueuedConnection);
    connect(this, SIGNAL(pause()), task, SLOT(pause()), Qt::QueuedConnection);
    connect(this, SIGNAL(seek(int)), task, SLOT(seek(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setStartFrame(int)), task, SLOT(setStartFrame(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setEndFrame(int)), task, SLOT(setEndFrame(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setROI(QRect)), task, SLOT(setROI(QRect)), Qt::QueuedConnection);
    connect(this, SIGNAL(forceROIRefresh()), task, SLOT(refreshROIOnCurFrame()), Qt::QueuedConnection);
    connect(this, SIGNAL(setRecomputeROIMode(bool)), task, SLOT(setRecomputeROIMode(bool)), Qt::QueuedConnection);
    connect(this, SIGNAL(setLogMetaData(MLogMetaData)), task, SLOT(setLogMetaData(MLogMetaData)), Qt::QueuedConnection);
    connect(this, SIGNAL(continueProcessing()), task, SLOT(continueProcessing()), Qt::QueuedConnection);
    connect(this, SIGNAL(setProcessOutputVideo(bool)), task, SLOT(setProcessOutputVideo(bool)), Qt::QueuedConnection);
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

void MCameraThread::doContinue()
{
    emit continueProcessing();
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

void MCameraThread::doForceROIRefresh()
{
    emit forceROIRefresh();
}

void MCameraThread::doSetRecomputeROIMode(bool mode)
{
    emit setRecomputeROIMode(mode);
}

void MCameraThread::doSetLogMetaData(MLogMetaData m)
{
    emit setLogMetaData(m);
}

void MCameraThread::doSetProcessOutputVideo(bool process)
{
    emit setProcessOutputVideo(process);
}

bool MCameraThread::doGetProcessOutputVideo()
{
    return task->getDoProcessOutputVideo();
}
