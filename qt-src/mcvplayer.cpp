#include "mcvplayer.h"

#include <QDebug>
#include <QSize>
#include <QUrl>
#include <QRgb>

MCVPlayer::MCVPlayer() : QObject(),
    m_format(QSize(500, 500), QVideoFrame::Format_ARGB32),
    m_surface(NULL),
    stopped(true)
{
}

MCVPlayer::~MCVPlayer()
{
    if(thread)
        thread->stop();
    delete thread;
    delete camera;
    //Camera release is automatic when cv::VideoCapture is destroyed
}

QSize MCVPlayer::getSize() const
{
    return size;
}

void MCVPlayer::setSize(QSize size)
{
    if(this->size.width() != size.width() || this->size.height() != size.height()){
        this->size = size;
        //update();
        emit sizeChanged();
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

        camera->getProperty(CV_CAP_PROP_FRAME_COUNT); // returns zero for non-video files
        emit videoPropertiesChanged();
    }
}

void MCVPlayer::update()
{
    DPRINT("Opening camera %d, width: %d, height: %d", 0, size.width(), size.height());

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
            connect(thread,SIGNAL(imageReady()), this, SLOT(imageReceived()));

            if(m_surface){
                if(m_surface->isActive())
                    m_surface->stop();
                if(!m_surface->start(QVideoSurfaceFormat(size,VIDEO_OUTPUT_FORMAT)))
                    qDebug() << "Could not start QAbstractVideoSurface, error: %d" << m_surface->error();
            }
//            thread->start();
            qDebug() << "Opened file: " << sourceFile;
        }
        else
            qDebug() << "Could not open video file: " << sourceFile;
    }
    catch(int e){
        qDebug() << "Exception" << e;
    }
}

void MCVPlayer::imageReceived()
{
    //Update VideoOutput
    if(m_surface) {
        if(!m_surface->present(*videoFrame)) {
            qDebug() << "Could not present QVideoFrame to QAbstractVideoSurface, error: " << m_surface->error();
        }
        if (camera) {
            curFrame = camera->getProperty(CV_CAP_PROP_POS_FRAMES);
            emit curFrameChanged();
        }
    }
}

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
        thread->start();
    }
}

void MCVPlayer::pause()
{
    stop();
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
    if (camera && thread) {
        pause();
        camera->setProperty(CV_CAP_PROP_POS_FRAMES, frame);
        thread->step();
    }
}
