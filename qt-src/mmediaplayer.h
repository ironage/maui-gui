#ifndef MMEDIAPLAYER_H
#define MMEDIAPLAYER_H

#include <QObject>
#include <QMediaPlayer>
#include <QAbstractVideoSurface>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

class MMediaPlayer : public QMediaPlayer
{
    Q_OBJECT
    Q_PROPERTY(QString sourceFile READ getSourceFile WRITE setSourceFile )
    Q_PROPERTY(QMediaPlayer::State playbackState READ state() NOTIFY stateUpdated(QMediaPlayer::State))
    Q_PROPERTY(QAbstractVideoSurface* videoSurface READ getVideoSurface WRITE setVideoSurface)
public:
    MMediaPlayer(QObject * parent = 0, Flags flags = 0);

public slots:
    QString getSourceFile() { return sourceFile; }
    void setSourceFile(QString file);
    void setVideoSurface(QAbstractVideoSurface* surface);
    QAbstractVideoSurface* getVideoSurface();
    void seek(qint64 pos);

signals:
    void stateUpdated(QMediaPlayer::State state);
private slots:
    void onStateChanged(QMediaPlayer::State state);
    void onStatusChanged(QMediaPlayer::MediaStatus status);
private:
    QAbstractVideoSurface* m_surface;
    QString sourceFile;
    cv::VideoCapture cvSource;
};

#endif // MMEDIAPLAYER_H
