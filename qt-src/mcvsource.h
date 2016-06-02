#ifndef MOCVSOURCE_H
#define MOCVSOURCE_H

#include <QObject>
#include <QQuickItem>
#include <QAbstractVideoSurface>
#include <QTimer>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


class MOCVSource : public QQuickItem
{
    Q_OBJECT
    Q_DISABLE_COPY(MOCVSource)
    Q_PROPERTY(QString sourceFile READ getSourceFile WRITE setSourceFile )
    //Q_PROPERTY(QAbstractVideoSurface* videoSurface READ getVideoSurface WRITE setVideoSurface)
    Q_PROPERTY(QVariant imageVariant READ getImage NOTIFY imageChanged)
//    Q_PROPERTY(QMediaPlayer::State playbackState READ state() NOTIFY stateUpdated(QMediaPlayer::State))
//    Q_PROPERTY(QAbstractVideoSurface* videoSurface READ getVideoSurface WRITE setVideoSurface)

public:
    MOCVSource(QQuickItem *parent = NULL);
    ~MOCVSource() {}
    QAbstractVideoSurface* getVideoSurface() const;
    void setVideoSurface(QAbstractVideoSurface* videoSurface);
    QVariant getImage();

public slots:
    QString getSourceFile() { return sourceFile; }
    void setSourceFile(QString file);
    void imageReceived();
signals:
    void imageChanged();
private:
    QString sourceFile;
    //cv::VideoCapture cvSource;
    QTimer test;
    QVideoFrame* videoFrame = NULL;
    QAbstractVideoSurface* videoSurface = NULL;
    bool exportCvImage = false;
    cv::Mat cvImage;
    unsigned char* cvImageBuf = NULL;
};

//Q_DECLARE_METATYPE(cv::Mat)

#endif // MOCVSOURCE_H
