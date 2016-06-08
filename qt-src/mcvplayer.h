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

class MCVPlayer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString sourceFile READ getSourceFile WRITE setSourceFile )
    Q_PROPERTY(QAbstractVideoSurface *videoSurface READ videoSurface WRITE setVideoSurface)

public:
    MCVPlayer();
    QAbstractVideoSurface* videoSurface() const { return m_surface; }

    void setVideoSurface(QAbstractVideoSurface *surface);
    static void matDeleter(void* mat) { delete static_cast<cv::Mat*>(mat); }
public slots:
    void onNewVideoContentReceived(const QVideoFrame &frame);
    QString getSourceFile() { return sourceFile; }
    void setSourceFile(QString file);

private:
    void process();

    QAbstractVideoSurface *m_surface;
    QVideoSurfaceFormat m_format;
    QString sourceFile;
    cv::VideoCapture cvSource;
    QVideoFrame curFrame;
};

#endif // MCVPLAYER_H

