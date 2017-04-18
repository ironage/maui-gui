#ifndef MVIDEOINFO_H
#define MVIDEOINFO_H

#include <QDir>
#include <QFileInfo>
#include <QList>
#include <QObject>
#include <QRect>
#include <QSize>
#include <QString>
#include <QUrl>
#include <QQmlListProperty>

#include "mcamerathread.h"
#include "mlogmetadata.h"
#include "mpoint.h"
#include "mvideocapture.h"

struct MVideoInfo : public QObject {
    Q_OBJECT
public:
    MVideoInfo(QUrl path);
    ~MVideoInfo();

    void setROI(const QRect &newROI);
    void setVelocityROI(const QRect &newROI);
    void forceROIRefresh();
    void setRecomputeROIMode(bool mode);
    void updateVideoSettings();
    void update();
    QString getSourceFile() const { return sourceFile; }
    QUrl getSourceUrl() const { return sourceUrl; }
    QSize getSize() const { return size; }
    QString getSourceDir() { return QFileInfo(sourceUrl.toLocalFile()).dir().absolutePath(); }
    QString getSourceExtension() { return QFileInfo(sourceUrl.fileName()).suffix(); }
    QString getSourceName() { return QFileInfo(sourceUrl.fileName()).completeBaseName(); }
    int getNumFrames() const { return numFrames; }
    int getCurFrame() const { return curFrame; }
    QRect getROI() const { return roi; }
    QRect getVelocityROI() const { return velocityROI; }
    bool getRecomputeROIMode() const { return recomputeROIMode; }
    QList<MPoint*> getTopPoints() { return topPoints; }
    QList<MPoint*> getBottomPoints() { return bottomPoints; }
    MLogMetaData getLogMetaData() const { return logMetaData; }

    void play();
    void continueProcessing();
    void pause();
    void stop();
    void seek(int frame);
    int getPlaybackState();
    void setSetupState(CameraTask::SetupState state);
    void setEndFrame(int frame);
    void setStartFrame(int frame);
    bool getProcessOutputVideo();
    void setProcessOutputVideo(bool process);
    void setNewTopPoints(QVariant newPoints);
    void setNewBottomPoints(QVariant newPoints);

    QString getDiameterConversionUnits();
    void setDiameterConversionUnits(QString diameterUnits);
    double getConversionPixels();
    void setConversionPixels(double diameterConversionPixels);
    QString getOutputDir();
    void setOutputDir(QString outputDir);
    QString getVelocityConversionUnits();
    void setVelocityConversionUnits(QString velocityConversionUnits);
    double getVelocityConversionPixels();
    void setVelocityConversionPixels(double velocityConversionPixels);
    double getVelocityTime();
    void setVelocityTime(double velocityTime);

    QVideoFrame* getVideoFrame() { return videoFrame; }
    QVideoFrame::PixelFormat getVideoFormat() const { return VIDEO_OUTPUT_FORMAT; }

private:
    QString sourceFile;
    QUrl sourceUrl;
    int numFrames;
    int curFrame;
    QSize size;
    QRect roi;
    QRect velocityROI;
    bool recomputeROIMode;
    QList<MPoint*> topPoints;
    QList<MPoint*> bottomPoints;
    QMutex lock; // protects camera and thread from race conditions
    MVideoCapture* camera = NULL;
    MCameraThread* thread = NULL;
    bool stopped;
    MLogMetaData logMetaData;
    const QVideoFrame::PixelFormat VIDEO_OUTPUT_FORMAT = QVideoFrame::PixelFormat::Format_ARGB32;


    cv::Mat cvImage;
    unsigned char* cvImageBuf = NULL;
    QVideoFrame* videoFrame = NULL;
    void allocateCvImage();
    void allocateVideoFrame();
signals:
    void roiChanged();
    void velocityROIChanged();
    void videoPropertiesChanged();
    void imageReady(int);
    void initPointsDetected(QList<MPoint>,QList<MPoint>);
    void videoFinished(CameraTask::ProcessingState);
    void outputProgress(int);
    void sourceChanged();
    void sourceUpdated();
    void sizeChanged();
    void videoLoaded(bool success, QUrl fullName, QString name, QString extension, QString dir);
    void initPointsChanged();
    void diameterConversionUnitsChanged();
    void conversionPixelsChanged();
    void outputDirChanged();
    void velocityConversionUnitsChanged();
    void velocityConversionPixelsChanged();
    void velocityTimeChanged();

public slots:
    void initPointsReceived(QList<MPoint> top, QList<MPoint> bottom);
    void imageReceived(int frame);
};

#endif // MVIDEOINFO_H
