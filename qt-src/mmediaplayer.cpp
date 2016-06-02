#include "mmediaplayer.h"

MMediaPlayer::MMediaPlayer(QObject *parent, Flags flags)
    : QMediaPlayer(parent, flags)
{
    connect(this, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(onStateChanged(QMediaPlayer::State)));
    connect(this, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(onStatusChanged(QMediaPlayer::MediaStatus)));
    setNotifyInterval(50);
}

void MMediaPlayer::setSourceFile(QString file)
{
    sourceFile = file;
    cvSource.open(file.toStdString());
    //setMedia(QUrl::fromLocalFile(sourceFile));
    //play();
}

void MMediaPlayer::onStateChanged(QMediaPlayer::State state)
{
    emit stateUpdated(state);

}

void MMediaPlayer::onStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::LoadedMedia) {
        qDebug() << "media loaded.";
        this->play();
        this->pause();
        this->setPosition(1);
    }
}

void MMediaPlayer::setVideoSurface(QAbstractVideoSurface* surface)
{
    m_surface = surface;
    setVideoOutput(m_surface);
}

QAbstractVideoSurface* MMediaPlayer::getVideoSurface()
{
    return m_surface;
}

void MMediaPlayer::seek(qint64 pos)
{
    QMediaPlayer::setPosition(pos);
}

