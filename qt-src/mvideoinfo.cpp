#include "mvideoinfo.h"

#include <QMutexLocker>
#include <QMediaPlayer>

MVideoInfo::MVideoInfo(QUrl path)
    : QObject(nullptr),
      numFrames(0),
      sourceUrl(path),
      recomputeROIMode(false),
      stopped(true)
{
    qDebug() << "source file set: " << sourceUrl.toLocalFile();
    sourceFile = sourceUrl.toLocalFile();
}

MVideoInfo::~MVideoInfo()
{
    { // braces for locker scope
        QMutexLocker locker(&lock);
        if(thread)
            thread->stop();
        delete thread;
        delete camera;
    }

    while (topPoints.size() > 0) {
        delete topPoints.takeAt(0);
    }
    while (bottomPoints.size() > 0) {
        delete bottomPoints.takeAt(0);
    }
}

void MVideoInfo::setVelocityROI(const QRect &newROI)
{
    if (velocityROI != newROI) {
        velocityROI = newROI;
        QMutexLocker locker(&lock);
        if (thread) {
            thread->doSetVelocityROI(velocityROI);
        }
        emit velocityROIChanged();
    }
}

void MVideoInfo::forceROIRefresh()
{
    QMutexLocker locker(&lock);
    if (thread) {
        thread->doForceROIRefresh();
    }
}

void MVideoInfo::setRecomputeROIMode(bool mode)
{
    if (recomputeROIMode != mode) {
        recomputeROIMode = mode;
        QMutexLocker locker(&lock);
        if (thread) {
            thread->doSetRecomputeROIMode(recomputeROIMode);
        }
    }
}

void MVideoInfo::setROI(const QRect &newROI)
{
    if (roi != newROI) {
        roi = newROI;
        QMutexLocker locker(&lock);
        if (thread) {
            thread->doSetROI(roi);
        }
        emit roiChanged();
    }
}

void MVideoInfo::updateVideoSettings() {
    if (camera) {
        int videoWidth = camera->getProperty(CV_CAP_PROP_FRAME_WIDTH);
        int videoHeight = camera->getProperty(CV_CAP_PROP_FRAME_HEIGHT);
        size = QSize(videoWidth, videoHeight);
        emit sizeChanged();

        numFrames = camera->getProperty(CV_CAP_PROP_FRAME_COUNT); // returns zero for non-video files

        //Create new buffers, camera accessor and thread
        allocateCvImage();
        allocateVideoFrame();

        emit videoPropertiesChanged();
    }
}

void MVideoInfo::allocateCvImage()
{
    cvImage.release();
    delete[] cvImageBuf;
    cvImageBuf = new unsigned char[size.width()*size.height()*3];
    cvImage = cv::Mat(size.height(),size.width(),CV_8UC3,cvImageBuf);
}

void MVideoInfo::allocateVideoFrame()
{
    videoFrame = new QVideoFrame(size.width()*size.height()*4,size,size.width()*4,VIDEO_OUTPUT_FORMAT);
}

void MVideoInfo::update()
{
    if (sourceFile.isEmpty()) {
        emit sourceChanged();
        return;
    }
    qDebug() << "cleaning up the previous video";
    QMutexLocker locker(&lock);
    //Destroy old thread, camera accessor and buffers
    delete thread;
    thread = NULL;
    delete camera;
    camera = NULL;
    if (videoFrame && videoFrame->isMapped())
        videoFrame->unmap();
    delete videoFrame;
    videoFrame = NULL;
    delete[] cvImageBuf;
    cvImageBuf = NULL;

    camera = new MVideoCapture();
    //Open newly created device
    try {
        if(camera->open(sourceFile.toStdString())) {
            updateVideoSettings();
            qDebug() << "starting video at size: " << size;

            thread = new MCameraThread(camera,videoFrame,cvImageBuf,size.width(),size.height());
            connect(thread, SIGNAL(imageReady(int)), this, SIGNAL(imageReady(int)));
            connect(thread, SIGNAL(initPointsDetected(QList<MPoint>,QList<MPoint>)), this, SLOT(initPointsReceived(QList<MPoint>,QList<MPoint>)));
            connect(thread, SIGNAL(videoFinished(CameraTask::ProcessingState)), this, SIGNAL(videoFinished(CameraTask::ProcessingState)));
            connect(thread, SIGNAL(outputProgress(int)), this, SIGNAL(outputProgress(int)));
            thread->doSetEndFrame(numFrames);
            thread->doSetStartFrame(0);

            thread->start();
            locker.unlock(); // for seek
            emit sourceChanged();
            emit sourceUpdated();
            seek(0);
            qDebug() << "Opened file: " << sourceFile;
            logMetaData.setFileName(sourceUrl.toString());
            logMetaData.setFilePath(getSourceDir());
            emit videoLoaded(true, sourceUrl, getSourceName(), getSourceExtension(), getSourceDir());
        } else {
            qDebug() << "Could not open video file: " << sourceFile;
            emit videoLoaded(false, sourceUrl, getSourceName(), getSourceExtension(), getSourceDir());
        }
    }
    catch(int e) {
        qDebug() << "Exception" << e;
    }
}

void MVideoInfo::play()
{
    QMutexLocker locker(&lock);
    if (thread) {
        stopped = false;
        qDebug() << "playing now, metadata :  " << logMetaData.getFileName();
        thread->doSetLogMetaData(logMetaData);
        thread->doPlay();
    }
}

void MVideoInfo::continueProcessing()
{
    QMutexLocker locker(&lock);
    if (thread) {
        thread->doContinue();
    }
}

void MVideoInfo::pause()
{
    QMutexLocker locker(&lock);
    if (thread) {
        thread->doPause();
    }
}

void MVideoInfo::stop()
{
    QMutexLocker locker(&lock);
    if (thread) {
        stopped = true;
        thread->stop();
    }
}

void MVideoInfo::seek(int frame)
{
    QMutexLocker locker(&lock);
    if (thread) {
        thread->doSeek(frame);
    }
}

int MVideoInfo::getPlaybackState()
{
    if (camera && thread && !stopped && curFrame < numFrames) {
        return QMediaPlayer::PlayingState;
    }
    return QMediaPlayer::PausedState;
}


void MVideoInfo::initPointsReceived(QList<MPoint> top, QList<MPoint> bottom) {
    bool modified = false;
    if (top.size() < topPoints.size()) {
        for (int i = top.size(); i < topPoints.size();) {
            delete topPoints.takeAt(i);
        }
        modified = true;
    } else if (top.size() > topPoints.size()) {
        for (int i = topPoints.size(); i < top.size(); ++i) {
            topPoints.push_back(new MPoint(top.at(i).x(), top.at(i).y()));
        }
        modified = true;
    }
    for (int i = 0; i < top.size(); ++i) {
        if (top.at(i) != *topPoints.at(i)) {
            topPoints.at(i)->setX(top.at(i).x());
            topPoints.at(i)->setY(top.at(i).y());
            modified = true;
        }
    }

    if (bottom.size() < bottomPoints.size()) {
        for (int i = bottom.size(); i < bottomPoints.size();) {
            delete bottomPoints.takeAt(i);
        }
        modified = true;
    } else if (bottom.size() > bottomPoints.size()) {
        for (int i = bottomPoints.size(); i < bottom.size(); ++i) {
            bottomPoints.push_back(new MPoint(bottom.at(i).x(), bottom.at(i).y()));
        }
        modified = true;
    }
    for (int i = 0; i < bottom.size(); ++i) {
        if (bottom.at(i) != *bottomPoints.at(i)) {
            bottomPoints.at(i)->setX(bottom.at(i).x());
            bottomPoints.at(i)->setY(bottom.at(i).y());
            modified = true;
        }
    }

    if (modified) {
        emit initPointsChanged();
    }
}

void MVideoInfo::imageReceived(int frame)
{
    curFrame = frame;
    emit imageReady(frame);
}
void MVideoInfo::setSetupState(CameraTask::SetupState state) {
    QMutexLocker locker(&lock);
    if (thread) {
        thread->doSetSetupState(state);
    }
}

void MVideoInfo::setEndFrame(int frame)
{
    QMutexLocker locker(&lock);
    if (thread) {
        thread->doSetEndFrame(frame);
    }
}

void MVideoInfo::setStartFrame(int frame)
{
    QMutexLocker locker(&lock);
    if (thread) {
        thread->doSetStartFrame(frame);
    }
}

bool MVideoInfo::getProcessOutputVideo()
{
    if (thread) {
        return thread->doGetProcessOutputVideo();
    }
    return true;
}

void MVideoInfo::setProcessOutputVideo(bool process)
{
    if (thread) {
        thread->doSetProcessOutputVideo(process);
    }
}

QList<MPoint> convertList(const QVariant& newPoints) {
    QList<MPoint> list;
    if (newPoints.canConvert<QVariantList>()) {
        QList<QVariant> points = newPoints.toList();
        for (QVariant v : points) {
            if (!v.canConvert<QPointF>()) {
                qDebug() << "Cannot convert vaniant to QPointF. Skipping" << v;
                continue;
            }
            list.append(MPoint(v.toPointF()));
        }
    } else {
        qDebug() << "Cannot convert points from variant to list!";
    }
    return list;
}

void MVideoInfo::setNewTopPoints(QVariant newPoints)
{
    if (thread) {
        QList<MPoint> converted = convertList(newPoints);
        if (!converted.empty()) {
            thread->doSetNewTopPoints(converted);
        }
    }
}

void MVideoInfo::setNewBottomPoints(QVariant newPoints)
{
    if (thread) {
        QList<MPoint> converted = convertList(newPoints);
        if (!converted.empty()) {
            thread->doSetNewBottomPoints(converted);
        }
    }
}

QString MVideoInfo::getDiameterConversionUnits()
{
    return logMetaData.getUnits();
}

void MVideoInfo::setDiameterConversionUnits(QString diameterUnits)
{
    if (logMetaData.getUnits() != diameterUnits) {
        logMetaData.setUnits(diameterUnits);
        emit diameterConversionUnitsChanged();
    }
}

double MVideoInfo::getConversionPixels()
{
    return logMetaData.getPixels();
}

void MVideoInfo::setConversionPixels(double diameterConversionPixels)
{
    if (logMetaData.getPixels() != diameterConversionPixels) {
        logMetaData.setPixels(diameterConversionPixels);
        emit conversionPixelsChanged();
    }
}

QString MVideoInfo::getOutputDir()
{
    return logMetaData.getOutputDir();
}

void MVideoInfo::setOutputDir(QString outputDir)
{
    if (logMetaData.getOutputDir() != outputDir) {
        logMetaData.setOutputDir(outputDir);
        emit outputDirChanged();
    }
}

QString MVideoInfo::getVelocityConversionUnits()
{
    return logMetaData.getVelocityUnits();
}

void MVideoInfo::setVelocityConversionUnits(QString velocityConversionUnits)
{
    if (logMetaData.getVelocityUnits() != velocityConversionUnits) {
        logMetaData.setVelocityUnits(velocityConversionUnits);
        emit velocityConversionUnitsChanged();
    }}

double MVideoInfo::getVelocityConversionPixels()
{
    return logMetaData.getVelocityPixels();
}

void MVideoInfo::setVelocityConversionPixels(double velocityConversionPixels)
{
    if (logMetaData.getVelocityPixels() != velocityConversionPixels) {
        logMetaData.setVelocityPixels(velocityConversionPixels);
        emit velocityConversionPixelsChanged();
    }
}

double MVideoInfo::getVelocityTime()
{
    return logMetaData.getVelocityXSeconds();
}

void MVideoInfo::setVelocityTime(double velocityTime)
{
    if (logMetaData.getVelocityXSeconds() != velocityTime) {
        logMetaData.setVelocityXSeconds(velocityTime);
        emit velocityTimeChanged();
    }
}

void MVideoInfo::refreshAll()
{
    emit sizeChanged();
    emit videoPropertiesChanged();
    emit imageReady(curFrame);
    emit roiChanged();
    emit velocityROIChanged();
    emit sourceChanged();
    emit sourceUpdated();
    emit initPointsChanged();
    emit diameterConversionUnitsChanged();
    emit conversionPixelsChanged();
    emit outputDirChanged();
    emit velocityConversionUnitsChanged();
    emit velocityConversionPixelsChanged();
    emit velocityTimeChanged();
}
