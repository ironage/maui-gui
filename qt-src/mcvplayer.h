#ifndef MCVPLAYER_H
#define MCVPLAYER_H

#include <QDir>
#include <QObject>
#include <QAbstractVideoSurface>
#include <QFileInfo>
#include <QVideoFrame>
#include <QVideoSurfaceFormat>
#include <QString>

//FIXME: prune libs
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "mcamerathread.h"
#include "minitthread.h"
#include "mlogmetadata.h"
#include "mpoint.h"
#include "mvideocapture.h"

#include <QMediaPlayer>
#include <QMutex>
#include <QQmlListProperty>

class MCVPlayer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString sourceFile READ getSourceFile WRITE setSourceFile NOTIFY sourceChanged)
    Q_PROPERTY(QString sourceDir READ getSourceDir NOTIFY sourceUpdated)
    Q_PROPERTY(QString sourceName READ getSourceName NOTIFY sourceUpdated)
    Q_PROPERTY(QString sourceExtension READ getSourceExtension NOTIFY sourceUpdated)
    Q_PROPERTY(QAbstractVideoSurface *videoSurface READ videoSurface WRITE setVideoSurface)
    Q_PROPERTY(QSize size READ getSize WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(int duration READ getNumFrames NOTIFY videoPropertiesChanged)
    Q_PROPERTY(int position READ getCurFrame WRITE setCurFrame NOTIFY curFrameChanged)
    Q_PROPERTY(int playbackState READ getPlaybackState NOTIFY playbackStateChanged)
    Q_PROPERTY(QRect roi READ getROI WRITE setROI NOTIFY roiChanged)
    Q_PROPERTY(bool recomputeROIOnChange READ getRecomputeROIMode WRITE setRecomputeROIMode NOTIFY recomputeROIChanged)
    Q_PROPERTY(QQmlListProperty<MPoint> initTopPoints READ getTopPoints NOTIFY initPointsChanged)
    Q_PROPERTY(QQmlListProperty<MPoint> initBottomPoints READ getBottomPoints NOTIFY initPointsChanged)
    Q_PROPERTY(MLogMetaData* logInfo MEMBER logMetaData NOTIFY logDataChanged)
    Q_PROPERTY(bool doProcessOutputVideo READ getProcessOutputVideo WRITE setProcessOutputVideo NOTIFY processOutputVideoChanged)
public:
    MCVPlayer();
    ~MCVPlayer();
    QAbstractVideoSurface* videoSurface() const { return m_surface; }
    Q_ENUM(CameraTask::ProcessingState)

    void setVideoSurface(QAbstractVideoSurface *surface);
public slots:
    void onNewVideoContentReceived(const QVideoFrame &frame);
    QString getSourceFile() { return sourceFile; }
    void setSourceFile(QString file);
    QString getSourceDir() { return QFileInfo(sourceUrl.toLocalFile()).dir().absolutePath(); }
    QString getSourceExtension() { return QFileInfo(sourceUrl.fileName()).suffix(); }
    QString getSourceName() { return QFileInfo(sourceUrl.fileName()).completeBaseName(); }
    QSize getSize() const;
    void setSize(QSize size);
    int getNumFrames() { return numFrames; }
    int getCurFrame() { return curFrame; }
    QRect getROI() { return roi; }
    void setROI(const QRect& newROI);
    bool getRecomputeROIMode() { return recomputeROIMode; }
    void setRecomputeROIMode(bool mode);
    void setCurFrame(int newFrame);
    int getPlaybackState();
    void play();
    void continueProcessing();
    void stop();
    void pause();
    void seek(int frame);
    void setEndFrame(int frame);
    void setStartFrame(int frame);
    void matlabInitFinished(MInitTask::InitStats status);
    bool getProcessOutputVideo();
    void setProcessOutputVideo(bool process);
    //FIXME: proper list access
    //QQmlListProperty::QQmlListProperty(QObject *object, void *data, AppendFunction append, CountFunction count, AtFunction at, ClearFunction clear)
    QQmlListProperty<MPoint> getTopPoints() { return QQmlListProperty<MPoint>(this, topPoints); }
    QQmlListProperty<MPoint> getBottomPoints() { return QQmlListProperty<MPoint>(this, bottomPoints); }

signals:
    void sizeChanged();
    void videoPropertiesChanged();
    void curFrameChanged();
    void videoFinished(CameraTask::ProcessingState state);
    void outputProgress(int progress);
    void playbackStateChanged();
    void sourceChanged();
    void roiChanged();
    void recomputeROIChanged();
    void initPointsChanged();
    void logDataChanged();
    void sourceUpdated();
    void processOutputVideoChanged();
private:

#ifdef ANDROID
    const QVideoFrame::PixelFormat VIDEO_OUTPUT_FORMAT = QVideoFrame::PixelFormat::Format_YV12;
#else
    const QVideoFrame::PixelFormat VIDEO_OUTPUT_FORMAT = QVideoFrame::PixelFormat::Format_ARGB32;
#endif

    QAbstractVideoSurface *m_surface;
    QVideoSurfaceFormat m_format;
    QString sourceFile;
    QUrl sourceUrl;

    int numFrames;
    int curFrame;
    QSize size;
    QRect roi;
    bool recomputeROIMode;
    QList<MPoint*> topPoints;
    QList<MPoint*> bottomPoints;
    QMutex lock; // protects camera and thread from race conditions
    MVideoCapture* camera = NULL;
    MCameraThread* thread = NULL;
    MInitThread* initThread = NULL;
    QVideoFrame* videoFrame = NULL;
    cv::Mat cvImage;
    unsigned char* cvImageBuf = NULL;
    bool stopped;
    MLogMetaData* logMetaData;
    void update();
    void updateVideoSettings();
    void allocateCvImage();
    void allocateVideoFrame();
private slots:
    void imageReceived(int frameNumber);
    void initPointsReceived(QList<MPoint> top, QList<MPoint> bottom);
};

Q_DECLARE_METATYPE(QQmlListProperty<MPoint>)
Q_DECLARE_METATYPE(QList<MPoint>)
Q_DECLARE_METATYPE(QList<MPoint*>)

#endif // MCVPLAYER_H

