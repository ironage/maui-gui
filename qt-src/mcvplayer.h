#ifndef MCVPLAYER_H
#define MCVPLAYER_H

#include <QObject>
#include <QAbstractVideoSurface>
#include <QVideoFrame>
#include <QVideoSurfaceFormat>
#include <QString>

//FIXME: prune libs
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "mcamerathread.h"
#include "mlogmetadata.h"
#include "mpoint.h"
#include "mvideocapture.h"

#include <QMediaPlayer>
#include <QQmlListProperty>

class MCVPlayer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString sourceFile READ getSourceFile WRITE setSourceFile NOTIFY sourceChanged)
    Q_PROPERTY(QAbstractVideoSurface *videoSurface READ videoSurface WRITE setVideoSurface)
    Q_PROPERTY(QSize size READ getSize WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(int duration READ getNumFrames NOTIFY videoPropertiesChanged)
    Q_PROPERTY(int position READ getCurFrame WRITE setCurFrame NOTIFY curFrameChanged)
    Q_PROPERTY(int playbackState READ getPlaybackState NOTIFY playbackStateChanged)
    Q_PROPERTY(QRect roi READ getROI WRITE setROI NOTIFY roiChanged)
    Q_PROPERTY(QQmlListProperty<MPoint> initTopPoints READ getTopPoints NOTIFY initPointsChanged)
    Q_PROPERTY(QQmlListProperty<MPoint> initBottomPoints READ getBottomPoints NOTIFY initPointsChanged)
    Q_PROPERTY(MLogMetaData* logInfo MEMBER logMetaData NOTIFY logDataChanged)
public:
    MCVPlayer();
    ~MCVPlayer();
    QAbstractVideoSurface* videoSurface() const { return m_surface; }

    void setVideoSurface(QAbstractVideoSurface *surface);
public slots:
    void onNewVideoContentReceived(const QVideoFrame &frame);
    QString getSourceFile() { return sourceFile; }
    void setSourceFile(QString file);
    QSize getSize() const;
    void setSize(QSize size);
    int getNumFrames() { return numFrames; }
    int getCurFrame() { return curFrame; }
    QRect getROI() { return roi; }
    void setROI(const QRect& newROI);
    void setCurFrame(int newFrame);
    int getPlaybackState();
    void play();
    void continueProcessing();
    void stop();
    void pause();
    void seek(int frame);
    void setEndFrame(int frame);
    void setStartFrame(int frame);
    //FIXME: proper list access
    //QQmlListProperty::QQmlListProperty(QObject *object, void *data, AppendFunction append, CountFunction count, AtFunction at, ClearFunction clear)
    QQmlListProperty<MPoint> getTopPoints() { return QQmlListProperty<MPoint>(this, topPoints); }
    QQmlListProperty<MPoint> getBottomPoints() { return QQmlListProperty<MPoint>(this, bottomPoints); }

signals:
    void sizeChanged();
    void videoPropertiesChanged();
    void curFrameChanged();
    void videoFinished();
    void playbackStateChanged();
    void sourceChanged();
    void roiChanged();
    void initPointsChanged();
    void logDataChanged();
private:

#ifdef ANDROID
    const QVideoFrame::PixelFormat VIDEO_OUTPUT_FORMAT = QVideoFrame::PixelFormat::Format_YV12;
#else
    const QVideoFrame::PixelFormat VIDEO_OUTPUT_FORMAT = QVideoFrame::PixelFormat::Format_ARGB32;
#endif

    QAbstractVideoSurface *m_surface;
    QVideoSurfaceFormat m_format;
    QString sourceFile;

    int numFrames;
    int curFrame;
    QSize size;
    QRect roi;
    QList<MPoint*> topPoints;
    QList<MPoint*> bottomPoints;
    MVideoCapture* camera = NULL;
    MCameraThread* thread = NULL;
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

