#include "mcvsource.h"

MOCVSource::MOCVSource(QQuickItem *parent) : QQuickItem(parent)
{
    qRegisterMetaType<cv::Mat>("Mat");
}

QVariant MOCVSource::getImage()
{
    qDebug() << "MOCVSource::getImage()";
    QVariant container(QVariant::UserType);
    container.setValue(1);
    //container.setValue(cvImage);
    return container;
}

void MOCVSource::setSourceFile(QString file)
{
    sourceFile = file;
    //cvSource.open(file.toStdString());
}

QAbstractVideoSurface* MOCVSource::getVideoSurface() const
{
    qDebug() << "MOCVSource::getVideoSurface()";
    return videoSurface;
}

void MOCVSource::setVideoSurface(QAbstractVideoSurface* surface)
{
    qDebug() << "MOCVSource::setVideoSurface(QAbstractVideoSurface* surface)";
    if(videoSurface != surface){
        qDebug() << "set is actually used";
        videoSurface = surface;
    }
}

void MOCVSource::imageReceived()
{
    qDebug() << "imageReceived()";
    //Update VideoOutput
    if(videoSurface)
        if(!videoSurface->present(*videoFrame))
            qDebug() << "Could not present QVideoFrame to QAbstractVideoSurface, error: %d",videoSurface->error();

    //Update exported CV image
    if(exportCvImage)
        emit imageChanged();
}
