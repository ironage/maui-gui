#include "mcvplayer.h"

#include <QDebug>
#include <QSize>
#include <QUrl>
#include <QRgb>

MCVPlayer::MCVPlayer() : QObject(),
    m_format(QSize(500, 500), QVideoFrame::Format_ARGB32),
    m_surface(NULL),
    stopped(true),
    numFrames(0)
{
    qRegisterMetaType<QQmlListProperty<MPoint>>();
    qRegisterMetaType<QList<MPoint*>>();
    qRegisterMetaType<QList<MPoint>>();
    qRegisterMetaType<MLogMetaData>();
    qRegisterMetaType<MInitTask::InitStats>();
    initThread = new MInitThread();
    connect(initThread, SIGNAL(initFinished(MInitTask::InitStats)), this, SLOT(matlabInitFinished(MInitTask::InitStats)));
    initThread->init();
}

MCVPlayer::~MCVPlayer()
{
    if(thread)
        thread->stop();
    delete thread;
    delete camera;
    delete initThread;  // cleans up MATLAB

    while (topPoints.size() > 0) {
        delete topPoints.takeAt(0);
    }
    while (bottomPoints.size() > 0) {
        delete bottomPoints.takeAt(0);
    }

    //Camera release is automatic when cv::VideoCapture is destroyed
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
        if (thread) {
            thread->doSetROI(roi);
        }
        emit roiChanged();
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
    if (sourceFile.isEmpty()) return;
    qDebug() << "Starting video width: " << size.width() << " height: " << size.height();
    //Destroy old thread, camera accessor and buffers
    delete thread;
    delete camera;
    if(videoFrame && videoFrame->isMapped())
        videoFrame->unmap();
    delete videoFrame;
    videoFrame = NULL;
    delete[] cvImageBuf;
    cvImageBuf = NULL;

    camera = new MVideoCapture();

    //Open newly created device
    try{
        if(camera->open(sourceFile.toStdString())){
            updateVideoSettings();
            emit sourceChanged();
            qDebug() << "starting video at size: " << size;
            //Create new buffers, camera accessor and thread
            allocateCvImage();
            if(m_surface)
                allocateVideoFrame();

            thread = new MCameraThread(camera,videoFrame,cvImageBuf,size.width(),size.height());
            connect(thread,SIGNAL(imageReady(int)), this, SLOT(imageReceived(int)));
            connect(thread, SIGNAL(initPointsDetected(QList<MPoint>,QList<MPoint>)), this, SLOT(initPointsReceived(QList<MPoint>,QList<MPoint>)));
            connect(thread, SIGNAL(videoFinished(CameraTask::ProcessingState)), this, SIGNAL(videoFinished(CameraTask::ProcessingState)));
            thread->doSetEndFrame(numFrames);
            thread->doSetStartFrame(0);
            if(m_surface){
                if(m_surface->isActive())
                    m_surface->stop();
                if(!m_surface->start(QVideoSurfaceFormat(size,VIDEO_OUTPUT_FORMAT)))
                    qDebug() << "Could not start QAbstractVideoSurface, error: %d" << m_surface->error();
            }
            thread->start();
            seek(0);
            qDebug() << "Opened file: " << sourceFile;
        }
        else
            qDebug() << "Could not open video file: " << sourceFile;
    }
    catch(int e){
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

/// remove
void MCVPlayer::onNewVideoContentReceived(const QVideoFrame &frame)
{
    qDebug() << "Presenting: " << frame << " to " << m_surface;
    if (m_surface) {
        qDebug() << "before present: " << m_surface->error();
        bool success = m_surface->present(frame);
        qDebug() << "after present: " << m_surface->error() << "success: " << success;
    }
}

void MCVPlayer::setSourceFile(QString file)
{
    QUrl fileUrl(file);
    qDebug() << "source file set: " << fileUrl.path();;
    sourceFile = fileUrl.path();
    if (sourceFile.size() > 1 && sourceFile.at(0) == QChar('/')) {
        sourceFile.remove(0, 1);
        qDebug() << "prefix / removed.";
    }
    update();
}

void MCVPlayer::play()
{
    if (thread) {
        stopped = false;
        qDebug() << "playing now, metadata :  " << logMetaData->getFileName();
        thread->doSetLogMetaData(*logMetaData);
        thread->doPlay();
    }
}

void MCVPlayer::continueProcessing()
{
    if (thread) {
        thread->doContinue();
    }
}

void MCVPlayer::pause()
{
    if (thread) {
        thread->doPause();
    }
}

void MCVPlayer::stop()
{
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

void MCVPlayer::seek(int frame)
{
    if (thread) {
        thread->doSeek(frame);
    }
}

void MCVPlayer::setEndFrame(int frame)
{
    if (thread) {
        thread->doSetEndFrame(frame);
    }
}

void MCVPlayer::setStartFrame(int frame)
{
    if (thread) {
        thread->doSetStartFrame(frame);
    }
}

void MCVPlayer::matlabInitFinished(MInitTask::InitStats status)
{
    qDebug() << "init status recieved: " << status;
}

//void MCVPlayer::setTopPoints(QList<MPoint> points)
//{
//    if (points != topPoints) {
//        topPoints = points;
//        // TODO: set in thread
//        emit initPointsChanged();
//    }
//}

//void MCVPlayer::setBottomPoints(QList<MPoint> points)
//{
//    if (points != bottomPoints) {
//        bottomPoints = points;
//        // TODO: set in thread
//        emit initPointsChanged();
//    }
//}
