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

class MCVPlayer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString sourceFile READ getSourceFile WRITE setSourceFile )
    Q_PROPERTY(QAbstractVideoSurface *videoSurface READ videoSurface WRITE setVideoSurface)
    Q_PROPERTY(QSize size READ getSize WRITE setSize NOTIFY sizeChanged)

public:
    MCVPlayer();
    ~MCVPlayer();
    QAbstractVideoSurface* videoSurface() const { return m_surface; }

    void setVideoSurface(QAbstractVideoSurface *surface);
    static void matDeleter(void* mat) { delete static_cast<cv::Mat*>(mat); }
public slots:
    void onNewVideoContentReceived(const QVideoFrame &frame);
    QString getSourceFile() { return sourceFile; }
    void setSourceFile(QString file);
    QSize getSize() const;
    void setSize(QSize size);
signals:
    void sizeChanged();
private:
    void process();

#ifdef ANDROID
    const QVideoFrame::PixelFormat VIDEO_OUTPUT_FORMAT = QVideoFrame::PixelFormat::Format_YV12;
#else
    const QVideoFrame::PixelFormat VIDEO_OUTPUT_FORMAT = QVideoFrame::PixelFormat::Format_ARGB32;
#endif

    QAbstractVideoSurface *m_surface;
    QVideoSurfaceFormat m_format;
    QString sourceFile;
    //cv::VideoCapture cvSource;
    QVideoFrame curFrame;

    QSize size;
    MVideoCapture* camera = NULL;
    MCameraThread* thread = NULL;
    QVideoFrame* videoFrame = NULL;
    cv::Mat cvImage;
    unsigned char* cvImageBuf = NULL;
    void update();
    void allocateCvImage();
    void allocateVideoFrame();
private slots:
    void imageReceived();
};

#endif // MCVPLAYER_H

