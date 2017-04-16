#ifndef MCVPLAYER_H
#define MCVPLAYER_H

#include <QDir>
#include <QObject>
#include <QAbstractVideoSurface>
#include <QFileInfo>
#include <QVideoFrame>
#include <QVideoSurfaceFormat>
#include <QString>

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
    Q_PROPERTY(QUrl sourceUrl READ getSourceUrl NOTIFY sourceUpdated)
    Q_PROPERTY(QString sourceDir READ getSourceDir NOTIFY sourceUpdated)
    Q_PROPERTY(QString sourceName READ getSourceName NOTIFY sourceUpdated)
    Q_PROPERTY(QString sourceExtension READ getSourceExtension NOTIFY sourceUpdated)
    Q_PROPERTY(QAbstractVideoSurface *videoSurface READ videoSurface WRITE setVideoSurface)
    Q_PROPERTY(QSize size READ getSize WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(int duration READ getNumFrames NOTIFY videoPropertiesChanged)
    Q_PROPERTY(int position READ getCurFrame WRITE setCurFrame NOTIFY curFrameChanged)
    Q_PROPERTY(int playbackState READ getPlaybackState NOTIFY playbackStateChanged)
    Q_PROPERTY(QRect roi READ getROI WRITE setROI NOTIFY roiChanged)
    Q_PROPERTY(QRect velocityROI READ getVelocityROI WRITE setVelocityROI NOTIFY velocityROIChanged)
    Q_PROPERTY(bool recomputeROIOnChange READ getRecomputeROIMode WRITE setRecomputeROIMode NOTIFY recomputeROIChanged)
    Q_PROPERTY(QQmlListProperty<MPoint> initTopPoints READ getTopPoints NOTIFY initPointsChanged)
    Q_PROPERTY(QQmlListProperty<MPoint> initBottomPoints READ getBottomPoints NOTIFY initPointsChanged)
    Q_PROPERTY(MLogMetaData logInfo MEMBER logMetaData NOTIFY logDataChanged)
    Q_PROPERTY(bool doProcessOutputVideo READ getProcessOutputVideo WRITE setProcessOutputVideo NOTIFY processOutputVideoChanged)

    Q_PROPERTY(QString conversionUnits READ getDiameterConversionUnits WRITE setDiameterConversionUnits NOTIFY diameterConversionUnitsChanged)
    Q_PROPERTY(double conversionPixels READ getConversionPixels WRITE setConversionPixels NOTIFY conversionPixelsChanged)
    Q_PROPERTY(QString outputDir READ getOutputDir WRITE setOutputDir NOTIFY outputDirChanged)
    Q_PROPERTY(QString velocityConversionUnits READ getVelocityConversionUnits WRITE setVelocityConversionUnits NOTIFY velocityConversionUnitsChanged)
    Q_PROPERTY(double velocityConversionPixels READ getVelocityConversionPixels WRITE setVelocityConversionPixels NOTIFY velocityConversionPixelsChanged)
    Q_PROPERTY(double velocityTime READ getVelocityTime WRITE setVelocityTime NOTIFY velocityTimeChanged)
public:
    MCVPlayer();
    ~MCVPlayer();
    QAbstractVideoSurface* videoSurface() const { return m_surface; }
    Q_ENUM(CameraTask::ProcessingState)
    Q_ENUM(CameraTask::SetupState)

    void setVideoSurface(QAbstractVideoSurface *surface);
public slots:
    QString getSourceFile() { return sourceFile; }
    QUrl getSourceUrl() { return sourceUrl; }
    void setSourceFile(QString file);
    QString getSourceDir() { return QFileInfo(sourceUrl.toLocalFile()).dir().absolutePath(); }
    QString getSourceExtension() { return QFileInfo(sourceUrl.fileName()).suffix(); }
    QString getSourceName() { return QFileInfo(sourceUrl.fileName()).completeBaseName(); }
    QSize getSize() const;
    void setSize(QSize size);
    int getNumFrames() const { return numFrames; }
    int getCurFrame() const { return curFrame; }
    QRect getROI() const { return roi; }
    void setROI(const QRect& newROI);
    QRect getVelocityROI() const { return velocityROI; }
    void setVelocityROI(const QRect& newROI);
    void forceROIRefresh();
    bool getRecomputeROIMode() const { return recomputeROIMode; }
    void setRecomputeROIMode(bool mode);
    void setCurFrame(int newFrame);
    int getPlaybackState();
    void setSetupState(CameraTask::SetupState state);
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
    void setNewTopPoints(QVariant newPoints);
    void setNewBottomPoints(QVariant newPoints);
    //FIXME: proper list access
    //QQmlListProperty::QQmlListProperty(QObject *object, void *data, AppendFunction append, CountFunction count, AtFunction at, ClearFunction clear)
    QQmlListProperty<MPoint> getTopPoints() { return QQmlListProperty<MPoint>(this, topPoints); }
    QQmlListProperty<MPoint> getBottomPoints() { return QQmlListProperty<MPoint>(this, bottomPoints); }
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
signals:
    void sizeChanged();
    void videoPropertiesChanged();
    void curFrameChanged();
    void videoFinished(CameraTask::ProcessingState state);
    void outputProgress(int progress);
    void playbackStateChanged();
    void sourceChanged();
    void roiChanged();
    void velocityROIChanged();
    void recomputeROIChanged();
    void initPointsChanged();
    void logDataChanged();
    void sourceUpdated();
    void processOutputVideoChanged();
    void videoLoaded(bool success, QUrl fullName, QString name, QString extension, QString dir);
    void diameterConversionUnitsChanged();
    void conversionPixelsChanged();
    void outputDirChanged();
    void velocityConversionUnitsChanged();
    void velocityConversionPixelsChanged();
    void velocityTimeChanged();
private:
    const QVideoFrame::PixelFormat VIDEO_OUTPUT_FORMAT = QVideoFrame::PixelFormat::Format_ARGB32;
    QAbstractVideoSurface *m_surface;
    QVideoSurfaceFormat m_format;
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
    MInitThread initThread;
    QVideoFrame* videoFrame = NULL;
    cv::Mat cvImage;
    unsigned char* cvImageBuf = NULL;
    bool stopped;
    MLogMetaData logMetaData;
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
