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
    curVideo(nullptr)
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
    curVideo = nullptr;
    for (int i = 0; i < videos.size(); ++i) {
        delete videos[i];
    }
    videos.clear();
    // Camera release is automatic when cv::VideoCapture is destroyed
    // MATLAB is cleaned up via the initThread destructor
}

QSize MCVPlayer::getSize() const
{
    if (curVideo) {
        return curVideo->getSize();
    }
    return QSize();
}

void MCVPlayer::setSize(QSize size)
{
    // FIXME: remove?
    qDebug() << "cv player set size... to remove";
//    if(this->size.width() != size.width() || this->size.height() != size.height()){
//       // this->size = size;
//        //update();
//      //  emit sizeChanged();
//    }
}

int MCVPlayer::getNumFrames() const
{
    if (curVideo) {
        return curVideo->getNumFrames();
    }
    return 0;
}

int MCVPlayer::getCurFrame() const
{
    if (curVideo) {
        return curVideo->getCurFrame();
    }
    return 0;
}

QRect MCVPlayer::getROI() const
{
    if (curVideo) {
        return curVideo->getROI();
    }
    return QRect();
}

void MCVPlayer::setROI(const QRect &newROI)
{
    if (curVideo) {
        curVideo->setROI(newROI);
    }
}

QRect MCVPlayer::getVelocityROI() const
{
    if (curVideo) {
        return curVideo->getVelocityROI();
    }
    return QRect();
}

void MCVPlayer::setVelocityROI(const QRect &newROI)
{
    if (curVideo) {
        curVideo->setVelocityROI(newROI);
    }
}

void MCVPlayer::forceROIRefresh()
{
    if (curVideo) {
        curVideo->forceROIRefresh();
    }
}

bool MCVPlayer::getRecomputeROIMode() const
{
    if (curVideo) {
        return curVideo->getRecomputeROIMode();
    }
    return false;
}

void MCVPlayer::setRecomputeROIMode(bool mode)
{
    if (curVideo) {
        curVideo->setRecomputeROIMode(mode);
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
        //FIXME: check if needed
//        if (curVideo) {
//            curVideo->update();
//        }
    }
}

void MCVPlayer::imageReceived(int frameNumber)
{
    if (m_surface && curVideo && curVideo->getVideoFrame()) {
        if (!m_surface->present(*(curVideo->getVideoFrame()))) {
            qDebug() << "Could not present QVideoFrame to QAbstractVideoSurface, error: " << m_surface->error();
        }
    }
    emit curFrameChanged();
}

void MCVPlayer::initPointsReceived(QList<MPoint> top, QList<MPoint> bottom)
{
    if (curVideo) {
        curVideo->initPointsReceived(top, bottom);
    }
}

void MCVPlayer::onVideoPropertiesChanged()
{
    if(m_surface && curVideo) {
        if(m_surface->isActive())
            m_surface->stop();
        if(!m_surface->start(QVideoSurfaceFormat(curVideo->getSize(), curVideo->getVideoFormat())))
            qDebug() << "Could not start QAbstractVideoSurface, error: %d" << m_surface->error();
    }
    emit videoPropertiesChanged();
}

void MCVPlayer::addVideoFile(QString file) {

    MVideoInfo *v = new MVideoInfo(QUrl(file));
    videos.push_back(v);
    changeToVideoFile(file);
    v->update();
}

void MCVPlayer::removeVideoFile(QString file)
{
    for (int i = 0; i < videos.size(); ++i) {
        if (videos[i] && videos[i]->getSourceUrl() == QUrl(file)) {
            if (curVideo && curVideo->getSourceUrl() == videos[i]->getSourceUrl()) {
                curVideo = nullptr;
            }
            auto toRemove = videos.erase(videos.begin() + i);
            delete (*toRemove);
            return;
        }
    }
    qDebug() << "Could not find video to remove: " << file;
}

void MCVPlayer::changeToVideoFile(QString fileUrl)
{
    for (int i = 0; i < videos.size(); ++i) {
        if (videos[i]) {
            if (videos[i]->getSourceUrl() == QUrl(fileUrl)) {
                if (!curVideo || curVideo->getSourceUrl() != QUrl(fileUrl)) {
                    curVideo = videos[i];
                    connect(curVideo, SIGNAL(videoPropertiesChanged()), this, SLOT(onVideoPropertiesChanged()));
                    connect(curVideo, SIGNAL(imageReady(int)), this, SLOT(imageReceived(int)));
                    connect(curVideo, SIGNAL(initPointsDetected(QList<MPoint>,QList<MPoint>)), this, SIGNAL(initPointsChanged()));
                    connect(curVideo, SIGNAL(videoFinished(CameraTask::ProcessingState)), this, SIGNAL(videoFinished(CameraTask::ProcessingState)));
                    connect(curVideo, SIGNAL(outputProgress(int)), this, SIGNAL(outputProgress(int)));
                    connect(curVideo, SIGNAL(sourceChanged()), this, SIGNAL(sourceChanged()));
                    connect(curVideo, SIGNAL(sourceUpdated()), this, SIGNAL(sourceUpdated()));
                    connect(curVideo, SIGNAL(videoLoaded(bool, QUrl, QString, QString, QString)), this, SIGNAL(videoLoaded(bool, QUrl, QString, QString, QString)));
                    connect(curVideo, SIGNAL(sizeChanged()), this, SIGNAL(sizeChanged()));
                    connect(curVideo, SIGNAL(initPointsChanged()), this, SIGNAL(initPointsChanged()));
                    connect(curVideo, SIGNAL(diameterConversionUnitsChanged()), this, SIGNAL(diameterConversionUnitsChanged()));
                    connect(curVideo, SIGNAL(conversionPixelsChanged()), this, SIGNAL(conversionPixelsChanged()));
                    connect(curVideo, SIGNAL(outputDirChanged()), this, SIGNAL(outputDirChanged()));
                    connect(curVideo, SIGNAL(velocityConversionUnitsChanged()), this, SIGNAL(velocityConversionUnitsChanged()));
                    connect(curVideo, SIGNAL(velocityConversionPixelsChanged()), this, SIGNAL(velocityConversionPixelsChanged()));
                    connect(curVideo, SIGNAL(velocityTimeChanged()), this, SIGNAL(velocityTimeChanged()));
                    emit logDataChanged();

                    curVideo->refreshAll();
                }
            } else {
                videos[i]->disconnect(); // breaks all connections
            }
        }
    }
}

void MCVPlayer::setSourceFile(QString file)
{
    // FIXME: readonly property
    qDebug() << "not supported operation setting source file";
}

QString MCVPlayer::getSourceDir()
{
    if (curVideo) {
        return curVideo->getSourceDir();
    }
    return QString();
}

QString MCVPlayer::getSourceExtension()
{
    if (curVideo) {
        return curVideo->getSourceExtension();
    }
    return QString();
}

QString MCVPlayer::getSourceName()
{
    if (curVideo) {
        return curVideo->getSourceName();
    }
    return QString();
}


QString MCVPlayer::getSourceFile()
{
    if (curVideo) {
        return curVideo->getSourceFile();
    }
    return QString();
}

QUrl MCVPlayer::getSourceUrl()
{
    if (curVideo) {
        return curVideo->getSourceUrl();
    }
    return QUrl();
}

void MCVPlayer::play()
{
    if (curVideo) {
        curVideo->play();
    }
}

void MCVPlayer::continueProcessing()
{
    if (curVideo) {
        curVideo->continueProcessing();
    }
}

void MCVPlayer::pause()
{
    if (curVideo) {
        curVideo->pause();
    }
}

void MCVPlayer::stop()
{
    if (curVideo) {
        curVideo->stop();
    }
}

void MCVPlayer::setCurFrame(int frame)
{
    seek(frame);
}

void MCVPlayer::seek(int frame)
{
    if (curVideo) {
        curVideo->seek(frame);
    }
}

int MCVPlayer::getPlaybackState()
{
    if (curVideo) {
        return curVideo->getPlaybackState();
    }
    return QMediaPlayer::StoppedState;
}

void MCVPlayer::setSetupState(CameraTask::SetupState state)
{
    if (curVideo) {
        curVideo->setSetupState(state);
    }
}

void MCVPlayer::setEndFrame(int frame)
{
    if (curVideo) {
        curVideo->setEndFrame(frame);
    }
}

void MCVPlayer::setStartFrame(int frame)
{
    if (curVideo) {
        curVideo->setStartFrame(frame);
    }
}

void MCVPlayer::matlabInitFinished(MInitTask::InitStats status)
{
    qDebug() << "init status recieved: " << status;
}

bool MCVPlayer::getProcessOutputVideo()
{
    if (curVideo) {
        return curVideo->getProcessOutputVideo();
    }
    return true;
}

void MCVPlayer::setProcessOutputVideo(bool process)
{
    if (curVideo) {
        curVideo->setProcessOutputVideo(process);
    }
}

void MCVPlayer::setNewTopPoints(QVariant newPoints)
{
    if (curVideo) {
        curVideo->setNewTopPoints(newPoints);
    }
}

void MCVPlayer::setNewBottomPoints(QVariant newPoints)
{
    if (curVideo) {
        curVideo->setNewBottomPoints(newPoints);
    }
}

QQmlListProperty<MPoint> MCVPlayer::getTopPoints()
{
    if (curVideo) {
        return QQmlListProperty<MPoint>(this, curVideo->getTopPoints());
    }
    return {};
}

QQmlListProperty<MPoint> MCVPlayer::getBottomPoints()
{
    if (curVideo) {
        return QQmlListProperty<MPoint>(this, curVideo->getBottomPoints());
    }
    return {};
}

MLogMetaData MCVPlayer::getLogMetaData() const
{
    if (curVideo) {
        return curVideo->getLogMetaData();
    }
    qDebug() << "Invalid log meta data returned";
    return MLogMetaData();
}

QString MCVPlayer::getDiameterConversionUnits()
{
    if (curVideo) {
        return curVideo->getDiameterConversionUnits();
    }
    return QString();
}

void MCVPlayer::setDiameterConversionUnits(QString diameterUnits)
{
    if (curVideo) {
        curVideo->setDiameterConversionUnits(diameterUnits);
    }
}

double MCVPlayer::getConversionPixels()
{
    if (curVideo) {
        return curVideo->getConversionPixels();
    }
    return 1.0;
}

void MCVPlayer::setConversionPixels(double diameterConversionPixels)
{
    if (curVideo) {
        curVideo->setConversionPixels(diameterConversionPixels);
    }
}

QString MCVPlayer::getOutputDir()
{
    if (curVideo) {
        return curVideo->getOutputDir();
    }
    return QString();
}

void MCVPlayer::setOutputDir(QString outputDir)
{
    if (curVideo) {
        curVideo->setOutputDir(outputDir);
    }
}

QString MCVPlayer::getVelocityConversionUnits()
{
    if (curVideo) {
        return curVideo->getVelocityConversionUnits();
    }
    return QString();
}

void MCVPlayer::setVelocityConversionUnits(QString velocityConversionUnits)
{
    if (curVideo) {
        curVideo->setVelocityConversionUnits(velocityConversionUnits);
    }
}

double MCVPlayer::getVelocityConversionPixels()
{
    if (curVideo) {
        return curVideo->getVelocityConversionPixels();
    }
    return 1.0;
}

void MCVPlayer::setVelocityConversionPixels(double velocityConversionPixels)
{
    if (curVideo) {
        curVideo->setVelocityConversionPixels(velocityConversionPixels);
    }
}

double MCVPlayer::getVelocityTime()
{
    if (curVideo) {
        return curVideo->getVelocityTime();
    }
    return 1.0;
}

void MCVPlayer::setVelocityTime(double velocityTime)
{
    if (curVideo) {
        curVideo->setVelocityTime(velocityTime);
    }
}
