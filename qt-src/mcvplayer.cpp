#include "mcvplayer.h"

#include <QDebug>
#include <QMutexLocker>
#include <QSize>
#include <QUrl>
#include <QRgb>
#include <QVariantList>

MCVPlayer::MCVPlayer() : QObject(),
    m_format(QSize(50, 50), QVideoFrame::Format_ARGB32),
    m_surface(NULL),
    recomputeROIMode(false),
    stopped(true),
    numFrames(0)
{
    qRegisterMetaType<QQmlListProperty<MPoint>>();
    qRegisterMetaType<QList<MPoint*>>();
    qRegisterMetaType<QList<MPoint>>();
    qRegisterMetaType<MLogMetaData>();
    qRegisterMetaType<MInitTask::InitStats>();
    connect(&initThread, SIGNAL(initFinished(MInitTask::InitStats)), this, SLOT(matlabInitFinished(MInitTask::InitStats)));
    initThread.init();
}

MCVPlayer::~MCVPlayer()
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

    // Camera release is automatic when cv::VideoCapture is destroyed
    // MATLAB is cleaned up via the initThread destructor
}

QSize MCVPlayer::getSize() const
{
    return size;
}

void MCVPlayer::setSize(QSize size)
{
    if(this->size.width() != size.width() || this->size.height() != size.height()){
       // this->size = size;
        //update();
      //  emit sizeChanged();
    }
}

void MCVPlayer::setROI(const QRect &newROI)
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

void MCVPlayer::setVelocityROI(const QRect &newROI)
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

void MCVPlayer::forceROIRefresh()
{
    QMutexLocker locker(&lock);
    if (thread) {
        thread->doForceROIRefresh();
    }
}

void MCVPlayer::setRecomputeROIMode(bool mode)
{
    if (recomputeROIMode != mode) {
        recomputeROIMode = mode;
        QMutexLocker locker(&lock);
        if (thread) {
            thread->doSetRecomputeROIMode(recomputeROIMode);
        }
    }
}

void MCVPlayer::setVideoSurface(QAbstractVideoSurface *surface)
{
    qDebug() << "setVideoSurface called";
    if (m_surface != surface && m_surface && m_surface->isActive()) {
        m_surface->stop();
    }
    m_surface = surface;
    if (m_surface) {
        m_surface->start(m_format);
        update();
    }
}

void MCVPlayer::allocateCvImage()
{
    cvImage.release();
    delete[] cvImageBuf;
#ifdef ANDROID
    cvImageBuf = new unsigned char[size.width()*size.height()*3/2];
    cvImage = cv::Mat(size.height()*3/2,size.width(),CV_8UC1,cvImageBuf);
#else
    cvImageBuf = new unsigned char[size.width()*size.height()*3];
    cvImage = cv::Mat(size.height(),size.width(),CV_8UC3,cvImageBuf);
#endif
}

void MCVPlayer::allocateVideoFrame()
{
#ifdef ANDROID
    videoFrame = new QVideoFrame(size.width()*size.height()*3/2,size,size.width(),VIDEO_OUTPUT_FORMAT);
#else
    videoFrame = new QVideoFrame(size.width()*size.height()*4,size,size.width()*4,VIDEO_OUTPUT_FORMAT);
#endif
}

void MCVPlayer::updateVideoSettings() {
    if (camera) {
        int videoWidth = camera->getProperty(CV_CAP_PROP_FRAME_WIDTH);
        int videoHeight = camera->getProperty(CV_CAP_PROP_FRAME_HEIGHT);
        size = QSize(videoWidth, videoHeight);
        emit sizeChanged();

        numFrames = camera->getProperty(CV_CAP_PROP_FRAME_COUNT); // returns zero for non-video files
        emit videoPropertiesChanged();
    }
}

void MCVPlayer::update()
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
            //Create new buffers, camera accessor and thread
            allocateCvImage();
            if(m_surface)
                allocateVideoFrame();

            thread = new MCameraThread(camera,videoFrame,cvImageBuf,size.width(),size.height());
            connect(thread, SIGNAL(imageReady(int)), this, SLOT(imageReceived(int)));
            connect(thread, SIGNAL(initPointsDetected(QList<MPoint>,QList<MPoint>)), this, SLOT(initPointsReceived(QList<MPoint>,QList<MPoint>)));
            connect(thread, SIGNAL(videoFinished(CameraTask::ProcessingState)), this, SIGNAL(videoFinished(CameraTask::ProcessingState)));
            connect(thread, SIGNAL(outputProgress(int)), this, SIGNAL(outputProgress(int)));
            thread->doSetEndFrame(numFrames);
            thread->doSetStartFrame(0);
            if(m_surface) {
                if(m_surface->isActive())
                    m_surface->stop();
                if(!m_surface->start(QVideoSurfaceFormat(size,VIDEO_OUTPUT_FORMAT)))
                    qDebug() << "Could not start QAbstractVideoSurface, error: %d" << m_surface->error();
            }
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

void MCVPlayer::imageReceived(int frameNumber)
{
    if(m_surface) {
        if(!m_surface->present(*videoFrame)) {
            qDebug() << "Could not present QVideoFrame to QAbstractVideoSurface, error: " << m_surface->error();
        }
        curFrame = frameNumber;
        emit curFrameChanged();
    }
}

void MCVPlayer::initPointsReceived(QList<MPoint> top, QList<MPoint> bottom)
{
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

void MCVPlayer::setSourceFile(QString file)
{
    sourceUrl = QUrl(file);
    qDebug() << "source file set: " << sourceUrl.toLocalFile();
    sourceFile = sourceUrl.toLocalFile();
    update();
}

void MCVPlayer::play()
{
    QMutexLocker locker(&lock);
    if (thread) {
        stopped = false;
        qDebug() << "playing now, metadata :  " << logMetaData.getFileName();
        thread->doSetLogMetaData(logMetaData);
        thread->doPlay();
    }
}

void MCVPlayer::continueProcessing()
{
    QMutexLocker locker(&lock);
    if (thread) {
        thread->doContinue();
    }
}

void MCVPlayer::pause()
{
    QMutexLocker locker(&lock);
    if (thread) {
        thread->doPause();
    }
}

void MCVPlayer::stop()
{
    QMutexLocker locker(&lock);
    if (thread) {
        stopped = true;
        thread->stop();
    }
}

void MCVPlayer::setCurFrame(int frame)
{
    seek(frame);
}

int MCVPlayer::getPlaybackState()
{
    if (camera && thread && !stopped && curFrame < numFrames) {
        return QMediaPlayer::PlayingState;
    }
    return QMediaPlayer::PausedState;
}

void MCVPlayer::setSetupState(CameraTask::SetupState state)
{
    QMutexLocker locker(&lock);
    if (thread) {
        thread->doSetSetupState(state);
    }
}

void MCVPlayer::seek(int frame)
{
    QMutexLocker locker(&lock);
    if (thread) {
        thread->doSeek(frame);
    }
}

void MCVPlayer::setEndFrame(int frame)
{
    QMutexLocker locker(&lock);
    if (thread) {
        thread->doSetEndFrame(frame);
    }
}

void MCVPlayer::setStartFrame(int frame)
{
    QMutexLocker locker(&lock);
    if (thread) {
        thread->doSetStartFrame(frame);
    }
}

void MCVPlayer::matlabInitFinished(MInitTask::InitStats status)
{
    qDebug() << "init status recieved: " << status;
}

bool MCVPlayer::getProcessOutputVideo()
{
    if (thread) {
        return thread->doGetProcessOutputVideo();
    }
    return true;
}

void MCVPlayer::setProcessOutputVideo(bool process)
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

void MCVPlayer::setNewTopPoints(QVariant newPoints)
{
    if (thread) {
        QList<MPoint> converted = convertList(newPoints);
        if (!converted.empty()) {
            thread->doSetNewTopPoints(converted);
        }
    }
}

void MCVPlayer::setNewBottomPoints(QVariant newPoints)
{
    if (thread) {
        QList<MPoint> converted = convertList(newPoints);
        if (!converted.empty()) {
            thread->doSetNewBottomPoints(converted);
        }
    }
}

QString MCVPlayer::getDiameterConversionUnits()
{
    return logMetaData.getUnits();
}

void MCVPlayer::setDiameterConversionUnits(QString diameterUnits)
{
    if (logMetaData.getUnits() != diameterUnits) {
        logMetaData.setUnits(diameterUnits);
        emit diameterConversionUnitsChanged();
    }
}

double MCVPlayer::getConversionPixels()
{
    return logMetaData.getPixels();
}

void MCVPlayer::setConversionPixels(double diameterConversionPixels)
{
    if (logMetaData.getPixels() != diameterConversionPixels) {
        logMetaData.setPixels(diameterConversionPixels);
        emit conversionPixelsChanged();
    }
}

QString MCVPlayer::getOutputDir()
{
    return logMetaData.getOutputDir();
}

void MCVPlayer::setOutputDir(QString outputDir)
{
    if (logMetaData.getOutputDir() != outputDir) {
        logMetaData.setOutputDir(outputDir);
        emit outputDirChanged();
    }
}

QString MCVPlayer::getVelocityConversionUnits()
{
    return logMetaData.getVelocityUnits();
}

void MCVPlayer::setVelocityConversionUnits(QString velocityConversionUnits)
{
    if (logMetaData.getVelocityUnits() != velocityConversionUnits) {
        logMetaData.setVelocityUnits(velocityConversionUnits);
        emit velocityConversionUnitsChanged();
    }
}

double MCVPlayer::getVelocityConversionPixels()
{
    return logMetaData.getVelocityPixels();
}

void MCVPlayer::setVelocityConversionPixels(double velocityConversionPixels)
{
    if (logMetaData.getVelocityPixels() != velocityConversionPixels) {
        logMetaData.setVelocityPixels(velocityConversionPixels);
        emit velocityConversionPixelsChanged();
    }
}

double MCVPlayer::getVelocityTime()
{
    return logMetaData.getVelocityXSeconds();
}

void MCVPlayer::setVelocityTime(double velocityTime)
{
    if (logMetaData.getVelocityXSeconds() != velocityTime) {
        logMetaData.setVelocityXSeconds(velocityTime);
        emit velocityTimeChanged();
    }
}
