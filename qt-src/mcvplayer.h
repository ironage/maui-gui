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

#include "mvideocapture.h"
#include "mcamerathread.h"

#include <QMediaPlayer>

class MCVPlayer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString sourceFile READ getSourceFile WRITE setSourceFile NOTIFY sourceChanged)
    Q_PROPERTY(QAbstractVideoSurface *videoSurface READ videoSurface WRITE setVideoSurface)
    Q_PROPERTY(QSize size READ getSize WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(int duration READ getNumFrames NOTIFY videoPropertiesChanged)
    Q_PROPERTY(int position READ getCurFrame WRITE setCurFrame NOTIFY curFrameChanged)
    Q_PROPERTY(int playbackState READ getPlaybackState NOTIFY playbackStateChanged)
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
    void setCurFrame(int newFrame);
    int getPlaybackState();
    void play();
    void stop();
    void pause();
    void seek(int frame);
signals:
    void sizeChanged();
    void videoPropertiesChanged();
    void curFrameChanged();
    void playbackStateChanged();
    void sourceChanged();
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
    MVideoCapture* camera = NULL;
    MCameraThread* thread = NULL;
    QVideoFrame* videoFrame = NULL;
    cv::Mat cvImage;
    unsigned char* cvImageBuf = NULL;
    bool stopped;
    void update();
    void updateVideoSettings();
    void allocateCvImage();
    void allocateVideoFrame();
private slots:
    void imageReceived();
};

#endif // MCVPLAYER_H

