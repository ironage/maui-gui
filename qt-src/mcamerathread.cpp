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
                       int width, int height)
    : curPlayState(Paused)
    , curSetupState(ALL)
    , width(width)
    , height(height)
    , camera(camera)
    , running(true)
    , videoFrame(videoFrame)
    , curFrame(0)
    , startFrame(0)
    , endFrame(0)
    , autoRecomputeROI(false)
    , doneInit(false)
    , cameraFrame(nullptr)
    , doProcessOutputVideo(true)
    , processingMillisecondsSinceStart(0)
{
    qRegisterMetaType<CameraTask::ProcessingState>();
    qRegisterMetaType<CameraTask::SetupState>();
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

void printMatVector(mwArray &data)
{
    qDebug() << "mat(" << data.NumberOfDimensions() << " x " << data.NumberOfFields() << ") : ";
    for (int i = 0; i < data.NumberOfFields(); i++) {
        qDebug() << "field " << i << " is: " << data.GetFieldName(i);
    }
    size_t numElements = data.NumberOfElements();
    mxDouble *raw = new mxDouble[numElements];
    data.GetData(raw, numElements);
    for (size_t i = 0; i < numElements; i++) {
        qDebug() << "[" << i << "]=" << raw[i];
    }
}

mwArray* opencvConvertToMX(cv::Mat& m) {
    mwSize rows = mwSize(m.rows);
    mwSize cols = mwSize(m.cols);
    mwArray *T = new mwArray(rows, cols, mxDOUBLE_CLASS);
    mxDouble *dataBuffer = new mxDouble[cols * rows]; // TODO: allocation on size change only
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            //        [i*cols+j]  row vs col order is reversed from cv::Mat to mwArray
            dataBuffer[j * rows + i] = double(m.at<unsigned char>(int(i), int(j))) / 255.0;
        }
    }
    T->SetData(dataBuffer, cols * rows);
    delete [] dataBuffer;
    return T;
}

void setPoint(const MPoint &p, mwArray &points, size_t index) {
    size_t numElements = points.NumberOfElements();
    if (numElements > 0  && index + (numElements/2) < numElements) {
        points.Get(1, (index + 1)) = p.x();
        points.Get(1, (index + 1) + (numElements/2)) = p.y();
    } else {
        qDebug() << "Attempting to write beyond array dimensions:" << index << " size: " << numElements;
    }
}

std::vector<cv::Point> convertMWArray(mwArray &points, QPoint offset) {
    size_t numElements = points.NumberOfElements();
    std::vector<cv::Point> result;
    if (numElements > 0 && numElements % 2 == 0) { // Assuming two dimensions
        size_t numPoints = numElements / 2;
        mxInt32 *data = new mxInt32[numElements];
        points.GetData(data, numElements);
        for (size_t i = 0; i < numPoints; ++i) {
            result.emplace_back(cv::Point(data[i] + offset.x(), data[numPoints + i] + offset.y()));
        }
        delete [] data;
    }
    return result;
}

std::vector<double> covertMWArrayToList(mwArray &array) {
    std::vector<double> result;
    size_t numElements = array.NumberOfElements();
    if (numElements > 0) {
        result.resize(numElements);
        array.GetData(&result[0], numElements);
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

    // allow drawing a velocity baseline even without any log data entries on this frame
    if (curSetupState & VELOCITY_ROI) {
        const cv::Point velocityBase(velocityROI.x(), velocityROI.y());
        if (curVelocityState.xAxisLocation > 0) {
            cv::Scalar xAxisColor(0, 84, 211); // bgr of #d35400 pumpkin
            cv::Point leftAxisPoint = makeSafePoint(0, curVelocityState.xAxisLocation, velocityBase);
            cv::Point rightAxisPoint = makeSafePoint(velocityROI.width(), curVelocityState.xAxisLocation, velocityBase);
            drawLine(mat, {leftAxisPoint, rightAxisPoint}, xAxisColor, 2);
        }
    }

    const MDataEntry *existing = log.get(frame);
    if (!existing) return;
    const cv::Scalar strongColor(0, 0, 250);  // b,g,r
    const cv::Scalar weakColor(0, 250, 0);    // b,g,r
    if (curSetupState & NORMAL_ROI) {
        drawLine(mat, existing->getTopStrongLine(), strongColor);
        drawLine(mat, existing->getTopWeakLine(), weakColor);
        drawLine(mat, existing->getBottomStrongLine(), strongColor);
        drawLine(mat, existing->getBottomWeakLine(), weakColor);
    }

    if (curSetupState & VELOCITY_ROI) {
        const int radius = 1;
        const int thickness = 1;
        const VelocityResults vr = existing->getVelocity();
        const cv::Point velocityOffset(vr.drawOffsetX, vr.drawOffsetY);
        const size_t num_results = vr.xTrackingLocationIndividual.size();
        if (vr.avgPositive.size() == num_results
                && vr.avgNegative.size() == num_results
                && vr.maxNegative.size() == num_results
                && vr.maxPositive.size() == num_results) {
            for (size_t i = 0; i < num_results; i++) {
                cv::Point avgPositive = makeSafePoint(vr.xTrackingLocationIndividual[i], vr.avgPositive[i], velocityOffset);
                cv::circle(mat, avgPositive, radius, weakColor, thickness);
                cv::Point maxPositive = makeSafePoint(vr.xTrackingLocationIndividual[i], vr.maxPositive[i], velocityOffset);
                cv::circle(mat, maxPositive, radius, strongColor, thickness);
                cv::Point avgNegative = makeSafePoint(vr.xTrackingLocationIndividual[i], vr.avgNegative[i], velocityOffset);
                cv::circle(mat, avgNegative, radius, weakColor, thickness);
                cv::Point maxNegative = makeSafePoint(vr.xTrackingLocationIndividual[i], vr.maxNegative[i], velocityOffset);
                cv::circle(mat, maxNegative, radius, strongColor, thickness);
            }
        } else {
            qDebug() << "Warning: refusing to draw inconsistent velocity results.";
        }
    }
}

// drawing a pixel located outside of a matrix will cause a crash
cv::Point CameraTask::makeSafePoint(int x, int y, const cv::Point& offset)
{
    int xTest = x + offset.x;
    int yTest = y + offset.y;
    if (xTest >= width) {
        xTest = width -1;
    }
    if (xTest < 0) {
        xTest = 0;
    }
    if (yTest >= height) {
        yTest = height -1;
    }
    if (yTest < 0) {
        yTest = 0;
    }
    return cv::Point(xTest, yTest);
}

void CameraTask::doWork()
{

#if defined(SHOW_FRAMERATE) && !defined(ANDROID)
    float fps = 0.0f;
#endif
    QElapsedTimer timer;
    int millisElapsed = 0;
    int millis;
    timer.start();
    bool accumulateProcessingTime = false;

    if(videoFrame)
        videoFrame->map(QAbstractVideoBuffer::ReadOnly);

    double frameRate = 1;
    if (camera) {
        frameRate = camera->getFrameRate();
        if (frameRate == 0) frameRate = 1;
    }
    // Assuming desktop, RGB camera image and RGBA QVideoFrame
    cv::Mat screenImage;
    if(videoFrame)
        screenImage = cv::Mat(height, width, CV_8UC4, videoFrame->bits());

    while(running && videoFrame != nullptr && camera != nullptr) {
        QCoreApplication::processEvents();

        if (curPlayState == PlayState::Paused) {
            QThread::msleep(10);
            continue;
        }
        getFrameData(curFrame);
        if (curPlayState == PlayState::Seeking) {
            curPlayState = PlayState::Paused;
        }

        // Get camera image into screen frame buffer
        // Assuming desktop, RGB camera image and RGBA QVideoFrame
        if(videoFrame) {
            cv::Mat tempMat(height, width, CV_8UC3, cameraFrame);
            cv::Mat roiSection = tempMat(getCVROI(roi));
            cv::cvtColor(roiSection, roiSection, CV_BGR2GRAY);
            std::unique_ptr<mwArray> matlabROI(opencvConvertToMX(roiSection));

            if (curPlayState == PlayState::Paused) {
                if (autoRecomputeROI) {
                    autoInitializeOnROI(matlabROI.get());
                }
            } else if (curPlayState == PlayState::AutoInitCurFrame) {
                curPlayState = PlayState::Paused;
                autoInitializeOnROI(matlabROI.get());
            } else if (curPlayState == PlayState::RedrawCurFrame) {
                curPlayState = PlayState::Paused;
            } else if (curPlayState == PlayState::AutoInitVelocityInFrame) {
                curPlayState = PlayState::Paused;

                if (curSetupState & SetupState::VELOCITY_ROI) {
                    cv::Mat velocityROISection = tempMat(getCVROI(velocityROI));
                    cv::cvtColor(velocityROISection, velocityROISection, CV_BGR2GRAY);
                    std::unique_ptr<mwArray> matlabVelocityROI(opencvConvertToMX(velocityROISection));
                    qDebug() << "starting velocity init";
                    if (!getFrameData(curFrame + 1)) {
                        qDebug() << "failed to get velocity frame data";
                        emit videoFinished(CameraTask::ProcessingState::VELOCITY_INIT_FAILED);
                        continue;
                    }
                    cv::Mat velocityROISection2 = tempMat(getCVROI(velocityROI));
                    cv::cvtColor(velocityROISection2, velocityROISection2, CV_BGR2GRAY);
                    std::unique_ptr<mwArray> matlabVelocityROI2(opencvConvertToMX(velocityROISection2));

                    initializeVelocityROI(matlabVelocityROI2.get(), matlabVelocityROI.get(), false);
                    getFrameData(curFrame); // reload initial frame for display
                }
            } else if (curPlayState == PlayState::Playing) {
                accumulateProcessingTime = true;
                if (!doneInit) {
                    processingMillisecondsSinceStart = 0;
                    timer.restart();
                    if (curSetupState & SetupState::NORMAL_ROI) {
                        if (topPoints.empty() || bottomPoints.empty()) {
                            emit videoFinished(CameraTask::ProcessingState::AUTO_INIT_FAILED);
                            curPlayState = PlayState::Paused;
                            continue;
                        }
                        const int numReturnValues = 6;
                        setup(numReturnValues,
                              matlabArrays[SMOOTH_KERNEL], matlabArrays[DERIVATE_KERNEL],
                              matlabArrays[TOP_STRONG_LINE], matlabArrays[BOTTOM_STRONG_LINE],
                              matlabArrays[TOP_REF_WALL], matlabArrays[BOTTOM_REF_WALL],
                              matlabArrays[TOP_STRONG_POINTS], matlabArrays[BOTTOM_STRONG_POINTS]);
                    }
                    if (curSetupState & SetupState::VELOCITY_ROI) {
                        cv::Mat velocityROISection = tempMat(getCVROI(velocityROI));
                        cv::cvtColor(velocityROISection, velocityROISection, CV_BGR2GRAY);
                        std::unique_ptr<mwArray> matlabVelocityROI(opencvConvertToMX(velocityROISection));

                        if (!getFrameData(curFrame + 1)) {
                            emit videoFinished(CameraTask::ProcessingState::VELOCITY_INIT_FAILED);
                            curPlayState = PlayState::Paused;
                            continue;
                        }

                        cv::Mat velocityROISection2 = tempMat(getCVROI(velocityROI));
                        cv::cvtColor(velocityROISection2, velocityROISection2, CV_BGR2GRAY);
                        std::unique_ptr<mwArray> matlabVelocityROI2(opencvConvertToMX(velocityROISection2));

                        if (!initializeVelocityROI(matlabVelocityROI2.get(), matlabVelocityROI.get(), true)) {
                            emit videoFinished(CameraTask::ProcessingState::VELOCITY_INIT_FAILED);
                            curPlayState = PlayState::Paused;
                            continue;
                        }
                        getFrameData(curFrame); // reload initial frame for processing below
                    }
                    doneInit = true;
                }
                MDataEntry frameResults(curFrame + 1, (1/frameRate) * curFrame);
                try {
                    if (curSetupState & SetupState::NORMAL_ROI) {
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
                        frameResults.addWallPart(
                                    getFirst(outerLumenDiameter, 0),
                                    getFirst(topIMT, 0),
                                    getFirst(bottomIMT, 0),
                                    convertMWArray(matlabArrays[TOP_STRONG_LINE], offset),
                                    convertMWArray(matlabArrays[TOP_WEAK_LINE], offset),
                                    convertMWArray(matlabArrays[BOTTOM_STRONG_LINE], offset),
                                    convertMWArray(matlabArrays[BOTTOM_WEAK_LINE], offset));
                    }
                    if (curSetupState & SetupState::VELOCITY_ROI) {
                        cv::Mat velocityROISection = tempMat(getCVROI(velocityROI));
                        cv::cvtColor(velocityROISection, velocityROISection, CV_BGR2GRAY);
                        std::unique_ptr<mwArray> matlabVelocityROI(opencvConvertToMX(velocityROISection));
                        VelocityResults vr = getVelocityFromFrame(matlabVelocityROI.get(), curFrame + 1, curVelocityState);
                        vr.drawOffsetX = velocityROI.x();
                        vr.drawOffsetY = velocityROI.y();
                        if (!vr.xTrackingLocationIndividual.empty()) {
                            curVelocityState.previousMaxXTrackingLoc = vr.xTrackingLocationIndividual[vr.xTrackingLocationIndividual.size() - 1];
                        }
                        frameResults.addVelocityPart(std::move(vr));
                    }
                } catch (const mwException& e) {
                    std::cerr << "exception caught: " << e.what() << std::endl;
                }
                log.add(std::move(frameResults));
            }

            // deep copy so we can write an overlay without affecting the cached copy
            cv::Mat imageCopy = tempMat.clone();
            drawOverlay(curFrame + 1, imageCopy);
            //Export camera image
            cv::cvtColor(imageCopy, screenImage, cv::COLOR_RGB2RGBA);
        }

        millis = int(timer.restart());
        millisElapsed += millis;
        if (accumulateProcessingTime) {
            processingMillisecondsSinceStart += millis;
        }
#if defined(SHOW_FRAMERATE) && !defined(ANDROID)
        fps = CAM_FPS_RATE*fps + (1.0f - CAM_FPS_RATE)*(1000.0f/millis);
        if (millisElapsed >= CAM_FPS_PRINT_PERIOD) {
            qDebug("Camera is running at %f FPS", double(fps));
            millisElapsed = 0;
        }
#endif

        emit imageReady(curFrame);

        if (curPlayState == PlayState::Playing) {
            if (curFrame + 1 > endFrame) {
                QCoreApplication::processEvents(); // send emitted frame progress for completion
                curPlayState = PlayState::Paused;
                writeResults();
            } else {
                ++curFrame;
            }
        }
    }
}

void CameraTask::play()
{
    if (curPlayState != PlayState::Playing) {
        log.clear();
        curPlayState = PlayState::Playing;
        doneInit = false;
        curFrame = startFrame;
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
}

void CameraTask::seek(int frameNumber)
{
    if (curPlayState == PlayState::Paused && frameNumber == curFrame) {
        return;
    }
    curFrame = frameNumber;
    curPlayState = PlayState::Seeking;
}

void CameraTask::setStartFrame(int frameNumber)
{
    startFrame = frameNumber;
    seek(frameNumber);
}

void CameraTask::setEndFrame(int frameNumber)
{
    if (camera && frameNumber >= camera->getNumTotalFrames()) {
        endFrame = camera->getNumTotalFrames() - 1;
    } else {
        endFrame = frameNumber;
    }
    seek(endFrame);
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

void CameraTask::setVelocityROI(QRect newROI)
{
    if (velocityROI != newROI) {
        velocityROI = newROI;
        if (curPlayState == PlayState::Paused) {
            curPlayState = PlayState::AutoInitVelocityInFrame;
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

void CameraTask::setSetupState(CameraTask::SetupState state)
{
    curSetupState = state;
    if (curPlayState == PlayState::Paused) {
        curPlayState = PlayState::RedrawCurFrame;
    }
}

void printPoints(QList<MPoint>& list) {
    for (MPoint p : list) {
        qDebug() << "(" << p.x() << ", " << p.y() << ")";
    }
}

void CameraTask::setNewTopPoints(QList<MPoint> points)
{
    // points come in raw, need to adjust to roi
    QPointF offset(roi.x(), roi.y());
    for (int i = 0; i < points.size(); ++i) {
        setPoint(points[i] - offset, matlabArrays[TOP_STRONG_POINTS], size_t(i));
        // topPoints is the accessor that has absolute coordinates
        topPoints[i].setX(points[i].x());
        topPoints[i].setY(points[i].y());
    }
}

void CameraTask::setNewBottomPoints(QList<MPoint> points)
{
    // points come in raw, need to adjust to roi
    QPointF offset(roi.x(), roi.y());
    for (int i = 0; i < points.size(); ++i) {
        setPoint(points[i] - offset, matlabArrays[BOTTOM_STRONG_POINTS], size_t(i));
        // bottomPoints is the accessor that has absolute coordinates
        bottomPoints[i].setX(points[i].x());
        bottomPoints[i].setY(points[i].y());
    }
//    qDebug() << "bottomPoints after: ";
//    printPoints(bottomPoints);
}

int CameraTask::getprocessingMillisecondsSinceStart()
{
    return processingMillisecondsSinceStart;
}

void CameraTask::notifyInitPoints(mwArray topWall, mwArray bottomWall, QPoint offset)
{
    topPoints.clear();
    bottomPoints.clear();
    size_t topSize = topWall.NumberOfElements();
    if (topSize % 2 == 0) {  // Assuming two dimensions
        mxInt32 *topData = new mxInt32[topSize];
        topWall.GetData(topData, topSize);
        for (size_t i = 0; i < topSize / 2; ++i) {
            MPoint p(topData[i] + offset.x(), topData[(topSize/2) + i] + offset.y());
            topPoints.push_back(p);
        }
        delete [] topData;
    }
    size_t bottomSize = bottomWall.NumberOfElements();
    if (bottomSize % 2 == 0) {  // Assuming two dimensions
        mxInt32 *bottomData = new mxInt32[bottomSize];
        bottomWall.GetData(bottomData, bottomSize);
        for (size_t i = 0; i < bottomSize / 2; ++i) {
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

cv::Rect CameraTask::getCVROI(QRect rect)
{
    // Trying to crop an image outside of bounds will crash, bound check here
    int roiX = std::max(0, rect.x());
    if (roiX >= width) roiX = width - 1;
    int roiY = std::max(0, rect.y());
    if (roiY >= height) roiY = height - 1;

    int roiW = std::min(width - roiX - 1, rect.width());
    if (roiW < 0) roiW = 0;
    int roiH = std::min(height - roiY - 1, rect.height());
    if (roiH < 0) roiH = 0;
    return cv::Rect(roiX, roiY, roiW, roiH);
}

void CameraTask::drawLine(cv::Mat &dest, const std::vector<cv::Point>& points, cv::Scalar color, int thickness)
{
    size_t numPoints = points.size();
    for (size_t i = 0; i < numPoints - 1; ++i) {
        cv::line(dest, points[i], points[i+1], color, thickness, CV_AA); // antialised line of thickness 1
    }
}

void CameraTask::initializeOutput()
{
    MLogMetaData metaData = log.getMetaData();
    QString outputDirName(metaData.getOutputDir().isEmpty() ? metaData.getFilePath() : metaData.getOutputDir());
    if (!QDir(outputDirName).exists()) {
        QDir().mkdir(outputDirName);
    }
    outputFileName = outputDirName + "/" + (QFileInfo(metaData.getFileName()).completeBaseName());
}

void CameraTask::processOutputVideo() {
    double frameRate = 1;
    if (camera) {
        frameRate = camera->getFrameRate();
        if (frameRate == 0) frameRate = 1;
    }
    cv::Size videoSize(width, height);
    // the following line didn't work correctly for some videos
    //int ex = static_cast<int>(camera->getProperty(CV_CAP_PROP_FOURCC));     // Get Codec Type- Int form
    int ex = CV_FOURCC('M', 'J', 'P', 'G');
    std::string outputName = outputFileName.toStdString() + "_tracking" + log.getMetaData().getWriteTime().toStdString() + ".avi";
    cv::VideoWriter outputVideo;
    bool success = outputVideo.open(outputName, ex, frameRate, videoSize, true);
    qDebug() << "opening output video: " << QString::fromStdString(outputName) << " success ? " << success << " (ex: " << ex << ")";

    curFrame = startFrame;
    //Assuming desktop, RGB camera image and RGBA QVideoFrame
    while(running && camera != nullptr && doProcessOutputVideo) {
        if (!getFrameData(curFrame)) {
            break;
        }

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
        curFrame++;
        QCoreApplication::processEvents(); // check for incoming signals about canceling output video write
    }
    if (camera) {
        curFrame = endFrame;
        camera->getFrameData(endFrame);
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
    log.write(outputFileName, curSetupState);
    if (doProcessOutputVideo) {
        processOutputVideo();
    }
    emit videoFinished(CameraTask::ProcessingState::SUCCESS);
}

bool CameraTask::autoInitializeOnROI(mwArray *matlabROI)
{
    qDebug() << "running auto init";
    try {
        mwArray numPoints(1, 1, mxINT32_CLASS);
        //int pointsInLine = roi.width() * 0.05;  // 5 percent of the width of the ROI
        int pointsInLine = 3; // hard coded for draggable dots
        //if (pointsInLine < 5) pointsInLine = 5; // lower limit is 5 points
        int numPointsData [] = { pointsInLine };
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

bool CameraTask::initializeVelocityROI(mwArray *velocityCurrentROI, mwArray *velocityPreviousROI, bool findFirstFrame)
{
    curVelocityState = VelocityState(); // reset to -1s
    try {
        mwArray numPoints(1, 1, mxINT32_CLASS);
        int numPointsData [] = { 5 };
        numPoints.SetData(numPointsData, 1);
        const int numReturnValues = 2;

        mwArray velocityXLocationMat, videoTypeMat;
        setup4Velocity(numReturnValues, velocityXLocationMat,
                        videoTypeMat, *velocityCurrentROI, *velocityPreviousROI);

        int velocityXLocation = getFirst(velocityXLocationMat, -1);
        int videoType = getFirst(videoTypeMat, -1);
        qDebug() << "initializeVelocityROI found video type: " << videoType << " findFirstFrame ? " << findFirstFrame;

        int indexOfFirstMovingFrame = 1; // for type 2 videos this is always 1
        if (videoType == 1 && findFirstFrame) {
            indexOfFirstMovingFrame = getIndexOfFirstMovingFrame();
            if (indexOfFirstMovingFrame < 0) {
                qDebug() << "could not find starting point of velocity movement for video type 1";
            }
        }
        curVelocityState.firstMovingFrame = indexOfFirstMovingFrame;
        curVelocityState.previousMaxXTrackingLoc = 1;
        curVelocityState.videoType = videoType;
        curVelocityState.xAxisLocation = velocityXLocation;
        log.setVelocityXAxisLocation(velocityXLocation);

        doneInit = false;
    } catch (const mwException& e) {
        std::cerr << "exception caught: " << e.what() << std::endl;
    }
    if (curVelocityState.xAxisLocation < 0
        || (curVelocityState.videoType != 1 && curVelocityState.videoType != 2)
        || (curVelocityState.videoType == 1 && curVelocityState.firstMovingFrame < 0)) {
        return false;
    }
    return true;
}

mwArray makeMWArrayFromVector(const std::vector<double>& data) {
    size_t length = data.size();
    mwArray mwData(1, length, mxDOUBLE_CLASS); // check row vs col order
    mxDouble *dataBuffer = new mxDouble[length];
    for (size_t i = 0; i < length; i++) {
        dataBuffer[i] = data[i];
    }
    if (length > 0) {
        mwData.SetData(dataBuffer, length);
    }
    delete [] dataBuffer;
    return mwData;
}

VelocityResults CameraTask::getVelocityFromFrame(mwArray *velocityCurrentROI,
                                                 int frame,
                                                 VelocityState velocityState)
{
    VelocityResults results;
    try {
        mwArray frameNumMat(1, 1, mxDOUBLE_CLASS);
        double frameNumberData [] = { static_cast<double>(frame) };
        frameNumMat.SetData(frameNumberData, 1);

        mwArray xAxisLocationMat(1, 1, mxDOUBLE_CLASS);
        double xAxisLocationData [] = { velocityState.xAxisLocation };
        xAxisLocationMat.SetData(xAxisLocationData, 1);

        mwArray videoTypeMat(1, 1, mxDOUBLE_CLASS);
        double videoTypeData [] = { velocityState.videoType };
        videoTypeMat.SetData(videoTypeData, 1);

        mwArray firstMovingFrameMat(1, 1, mxDOUBLE_CLASS);
        double firstMovingFrameData [] = { velocityState.firstMovingFrame };
        firstMovingFrameMat.SetData(firstMovingFrameData, 1);

        mwArray previousXTrackingLocMat(1, 1, mxDOUBLE_CLASS);
        double previousXTrackingLocData [] = { velocityState.previousMaxXTrackingLoc };
        previousXTrackingLocMat.SetData(previousXTrackingLocData, 1);

        const int numReturnValues = 5;

        mwArray maxPositiveMat, avgPositiveMat, maxNegativeMat, avgNegativeMat, xTrackingLocationIndividualMat;

        processVelocityIntervals(numReturnValues, maxPositiveMat,
                                 avgPositiveMat, maxNegativeMat, avgNegativeMat,
                                 xTrackingLocationIndividualMat,
                                 *velocityCurrentROI, frameNumMat,
                                 xAxisLocationMat, videoTypeMat,
                                 firstMovingFrameMat, previousXTrackingLocMat);

        results.maxPositive = covertMWArrayToList(maxPositiveMat);
        results.avgPositive = covertMWArrayToList(avgPositiveMat);
        results.maxNegative = covertMWArrayToList(maxNegativeMat);
        results.avgNegative = covertMWArrayToList(avgNegativeMat);
        results.xTrackingLocationIndividual = covertMWArrayToList(xTrackingLocationIndividualMat);
        //qDebug() << "velocity results: " << results;
    } catch (const mwException& e) {
        std::cerr << "exception caught: " << e.what() << std::endl;
    }
    return results;
}

int CameraTask::getIndexOfFirstMovingFrame()
{
    qDebug() << "getIndexOfFirstMovingFrame: start";
    if (!camera || camera->getNumTotalFrames() < 2) {
        qDebug() << "Error could not initialize type 1 video";
        return -1;
    }

    // advance to beginning
    if (!getFrameData(0)) return -1;
    cv::Mat tempMat(height, width, CV_8UC3, cameraFrame);

    cv::Mat velocityROISection = tempMat(getCVROI(velocityROI));
    cv::cvtColor(velocityROISection, velocityROISection, CV_BGR2GRAY);
    std::unique_ptr<mwArray> matlabVelocityROI(opencvConvertToMX(velocityROISection));

    int returnedIndex = -1;
    mwArray returnedIndexMat;
    mwArray curIndexMat(1, 1, mxINT32_CLASS);
    int curVelocityInitFrame = 2; // current frame starts at 2 since previous frame starts at 1
    int curIndexData [] = { curVelocityInitFrame };

    int velocitySeekFrame = 1;
    while (returnedIndex < 0) {
        if (!getFrameData(velocitySeekFrame)) break;
        cv::Mat velocityROISection2 = tempMat(getCVROI(velocityROI));
        cv::cvtColor(velocityROISection2, velocityROISection2, CV_BGR2GRAY);
        std::unique_ptr<mwArray> matlabVelocityROI2(opencvConvertToMX(velocityROISection2));

        curIndexData[0] = curVelocityInitFrame;
        curIndexMat.SetData(curIndexData, 1);

        check4FirstMovingFrame(1, returnedIndexMat, *(matlabVelocityROI2.get()), *(matlabVelocityROI.get()), curIndexMat);

        returnedIndex = getFirst(returnedIndexMat, -1);
        matlabVelocityROI.reset(matlabVelocityROI2.release());
        ++curVelocityInitFrame;
        ++velocitySeekFrame;
    }
    getFrameData(curFrame);
    qDebug() << "velocity movement returning " << returnedIndex;
    return returnedIndex;
}

bool CameraTask::getFrameData(int frame)
{
    unsigned char* frameData = camera->getFrameData(frame);
    if (!frameData) {
        qDebug() << "Something is fatally wrong if we couldn't refresh the frame! " << curFrame << " requested: " << frame;
        return false;
    }
    cameraFrame = frameData;
    return true;
}








MCameraThread::MCameraThread(MVideoCapture* camera, QVideoFrame* videoFrame, int width, int height)
{
    task = new CameraTask(camera, videoFrame, width, height);
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
    connect(this, SIGNAL(setVelocityROI(QRect)), task, SLOT(setVelocityROI(QRect)), Qt::QueuedConnection);
    connect(this, SIGNAL(forceROIRefresh()), task, SLOT(refreshROIOnCurFrame()), Qt::QueuedConnection);
    connect(this, SIGNAL(setRecomputeROIMode(bool)), task, SLOT(setRecomputeROIMode(bool)), Qt::QueuedConnection);
    connect(this, SIGNAL(setLogMetaData(MLogMetaData)), task, SLOT(setLogMetaData(MLogMetaData)), Qt::QueuedConnection);
    connect(this, SIGNAL(continueProcessing()), task, SLOT(continueProcessing()), Qt::QueuedConnection);
    connect(this, SIGNAL(setProcessOutputVideo(bool)), task, SLOT(setProcessOutputVideo(bool)), Qt::QueuedConnection);
    connect(this, SIGNAL(setSetupState(CameraTask::SetupState)), task, SLOT(setSetupState(CameraTask::SetupState)), Qt::QueuedConnection);
    connect(this, SIGNAL(setNewTopPoints(QList<MPoint>)), task, SLOT(setNewTopPoints(QList<MPoint>)), Qt::QueuedConnection);
    connect(this, SIGNAL(setNewBottomPoints(QList<MPoint>)), task, SLOT(setNewBottomPoints(QList<MPoint>)), Qt::QueuedConnection);
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
    if (task != nullptr) {
        task->stop();
    }
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

void MCameraThread::doSetVelocityROI(QRect roi)
{
    emit setVelocityROI(roi);
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
    if (task) {
        return task->getDoProcessOutputVideo();
    }
    return true;
}

void MCameraThread::doSetSetupState(CameraTask::SetupState state)
{
    emit setSetupState(state);
}

int MCameraThread::doGetSetupState()
{
    if (task) {
        return task->getSetupState();
    }
    return CameraTask::SetupState::NONE;
}

int MCameraThread::doGetprocessingMillisecondsSinceStart()
{
    if (task) {
        return task->getprocessingMillisecondsSinceStart();
    }
    return 0;
}

void MCameraThread::doSetNewTopPoints(QList<MPoint> points)
{
    emit setNewTopPoints(points);
}

void MCameraThread::doSetNewBottomPoints(QList<MPoint> points)
{
    emit setNewBottomPoints(points);
}
