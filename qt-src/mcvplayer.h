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
#include "mvideoinfo.h"

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
    Q_PROPERTY(int setupState READ getSetupState WRITE setSetupState NOTIFY videoControlInfoChanged)
    Q_PROPERTY(int processingMillisecondsSinceStart READ getprocessingMillisecondsSinceStart NOTIFY curFrameChanged)
    Q_PROPERTY(QRect roi READ getROI WRITE setROI NOTIFY roiChanged)
    Q_PROPERTY(QRect velocityROI READ getVelocityROI WRITE setVelocityROI NOTIFY velocityROIChanged)
    Q_PROPERTY(QRect diameterScale READ getDiameterScale WRITE setDiameterScale NOTIFY videoControlInfoChanged)
    Q_PROPERTY(QRect velocityScaleVertical READ getVelocityScaleVertical WRITE setVelocityScaleVertical NOTIFY videoControlInfoChanged)
    Q_PROPERTY(QRect velocityScaleHorizontal READ getVelocityScaleHorizontal WRITE setVelocityScaleHorizontal NOTIFY videoControlInfoChanged)
    Q_PROPERTY(bool recomputeROIOnChange READ getRecomputeROIMode WRITE setRecomputeROIMode NOTIFY recomputeROIChanged)
    Q_PROPERTY(QQmlListProperty<MPoint> initTopPoints READ getTopPoints NOTIFY initPointsChanged)
    Q_PROPERTY(QQmlListProperty<MPoint> initBottomPoints READ getBottomPoints NOTIFY initPointsChanged)
    Q_PROPERTY(MLogMetaData logInfo READ getLogMetaData NOTIFY logDataChanged)
    Q_PROPERTY(bool doProcessOutputVideo READ getProcessOutputVideo WRITE setProcessOutputVideo NOTIFY processOutputVideoChanged)
    Q_PROPERTY(bool setupAllVideos MEMBER m_setupAllVideos NOTIFY setupAllVideosChanged)

    Q_PROPERTY(QString conversionUnits READ getDiameterConversionUnits WRITE setDiameterConversionUnits NOTIFY diameterConversionUnitsChanged)
    Q_PROPERTY(double diameterConversion READ getDiameterConversion WRITE setDiameterConversion NOTIFY diameterConversionChanged)
    Q_PROPERTY(QString outputDir READ getOutputDir WRITE setOutputDir NOTIFY outputDirChanged)
    Q_PROPERTY(QString velocityConversionUnits READ getVelocityConversionUnits WRITE setVelocityConversionUnits NOTIFY velocityConversionUnitsChanged)
    Q_PROPERTY(double velocityConversion READ getVelocityConversion WRITE setVelocityConversion NOTIFY velocityConversionChanged)
    Q_PROPERTY(double velocityTime READ getVelocityTime WRITE setVelocityTime NOTIFY velocityTimeChanged)
public:
    MCVPlayer();
    ~MCVPlayer();
    QAbstractVideoSurface* videoSurface() const { return m_surface; }
    Q_ENUM(CameraTask::ProcessingState)
    Q_ENUM(CameraTask::SetupState)

    void setVideoSurface(QAbstractVideoSurface *surface);
public slots:
    QString getSourceFile();
    QUrl getSourceUrl();
    void setSourceFile(QString file);
    QString getSourceDir();
    QString getSourceExtension();
    QString getSourceName();
    QSize getSize() const;
    void setSize(QSize size);
    int getNumFrames() const;
    int getCurFrame() const;
    QRect getROI() const;
    void setROI(const QRect& newROI);
    QRect getVelocityROI() const;
    void setVelocityROI(const QRect &newROI);
    QRect getDiameterScale() const;
    void setDiameterScale(const QRect &newScale);
    QRect getVelocityScaleVertical() const;
    void setVelocityScaleVertical(const QRect &newScale);
    QRect getVelocityScaleHorizontal() const;
    void setVelocityScaleHorizontal(const QRect &newScale);
    void forceROIRefresh();
    bool getRecomputeROIMode() const;
    void setRecomputeROIMode(bool mode);
    void setCurFrame(int newFrame);
    int getPlaybackState();
    void setSetupState(int state);
    int getSetupState();
    int getprocessingMillisecondsSinceStart();
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
    QQmlListProperty<MPoint> getTopPoints();
    QQmlListProperty<MPoint> getBottomPoints();
    MLogMetaData getLogMetaData() const;
    QString getDiameterConversionUnits();
    void setDiameterConversionUnits(QString diameterUnits);
    double getDiameterConversion();
    void setDiameterConversion(double diameterConversion);
    QString getOutputDir();
    void setOutputDir(QString outputDir);
    QString getVelocityConversionUnits();
    void setVelocityConversionUnits(QString velocityConversionUnits);
    double getVelocityConversion();
    void setVelocityConversion(double velocityConversion);
    double getVelocityTime();
    void setVelocityTime(double velocityTime);
    void addVideoFile(QString file);
    void removeVideoFile(QString file);
    void changeToVideoFile(QString fileUrl);
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
    void diameterConversionChanged();
    void outputDirChanged();
    void velocityConversionUnitsChanged();
    void velocityConversionChanged();
    void velocityTimeChanged();
    void videoControlInfoChanged();
    void setupAllVideosChanged();
private:
    QAbstractVideoSurface *m_surface;
    QVideoSurfaceFormat m_format;
    std::vector<MVideoInfo*> videos;
    MVideoInfo *curVideo;
    MInitThread initThread;
    bool m_setupAllVideos;

    void allocateCvImage();
    void allocateVideoFrame();
private slots:
    void imageReceived(int frameNumber);
    void initPointsReceived(QList<MPoint> top, QList<MPoint> bottom);
    void videoInitialized();
};

Q_DECLARE_METATYPE(QQmlListProperty<MPoint>)
Q_DECLARE_METATYPE(QList<MPoint>)
Q_DECLARE_METATYPE(QList<MPoint*>)

#endif // MCVPLAYER_H
