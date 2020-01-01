#ifndef MLOGMETADATA_H
#define MLOGMETADATA_H

#include <vector>

#include <QObject>
#include <QString>

class MLogMetaData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString inputFileName MEMBER fileName NOTIFY propertiesChanged)
    Q_PROPERTY(QString inputFilePath MEMBER filePath NOTIFY propertiesChanged)
    Q_PROPERTY(QString outputDir MEMBER outputDirectory NOTIFY propertiesChanged)
    Q_PROPERTY(QString conversionUnits MEMBER units NOTIFY propertiesChanged)
    Q_PROPERTY(double diameterConversion MEMBER diameterScaleConversion NOTIFY propertiesChanged)
    Q_PROPERTY(double diameterPixelHeight MEMBER diameterScalePixelHeight NOTIFY propertiesChanged)
    Q_PROPERTY(QString velocityConversionUnits MEMBER velocityUnits NOTIFY propertiesChanged)
    Q_PROPERTY(double velocityConversion MEMBER velocityScaleConversion NOTIFY propertiesChanged)
    Q_PROPERTY(double velocityPixelHeight MEMBER velocityScalePixelHeight NOTIFY propertiesChanged)
    Q_PROPERTY(double velocityTime MEMBER velocityXSeconds NOTIFY propertiesChanged)
public:
    explicit MLogMetaData(QObject *parent = nullptr);
    MLogMetaData(const MLogMetaData& other);

    QString getFileName() const { return fileName; }
    QString getFilePath() const { return filePath; }
    QString getTimestamp() const;
    double getDiameterScalePixelHeight() const { return diameterScalePixelHeight; }
    double getDiameterScaleConversion() const { return diameterScaleConversion; }
    QString getUnits() const { return units; }
    QString getVelocityUnits() const { return velocityUnits; }
    QString getOutputDir() const { return outputDirectory; }
    double getVelocityScalePixelHeight() const { return velocityScalePixelHeight; }
    double getVelocityScaleConversion() const { return velocityScaleConversion; }
    double getVelocityXSeconds() const { return velocityXSeconds; }
    double getVelocityXAxisLocation() const { return velocityXAxisLocation; }
    void setFileName(QString name) { fileName = name; }
    void setFilePath(QString path) { filePath = path; }
    void setDiameterPixelHeight(double numPixels) { diameterScalePixelHeight = numPixels; }
    void setDiameterConversion(double conversion) { diameterScaleConversion = conversion; }
    void setUnits(QString newUnits) { units = newUnits; }
    void setVelocityUnits(QString newUnits) { velocityUnits = newUnits; }
    void setVelocityPixelHeight(double numPixels) { velocityScalePixelHeight = numPixels; }
    void setVelocityConversion(double conversion) { velocityScaleConversion = conversion; }
    void setVelocityXSeconds(double numSeconds) { velocityXSeconds = numSeconds; }
    void setVelocityXAxisLocation(double xLoc) { velocityXAxisLocation = xLoc; }
    void setOutputDir(QString dir) { outputDirectory = dir; }
    void touchWriteTime();
    QString getWriteTime() { return uniqueness; }
    void operator=(const MLogMetaData& other);
signals:
    void propertiesChanged(); //FIXME: use this!

private:
    QString fileName;
    QString filePath;
    QString outputDirectory;
    QString units;
    QString uniqueness;
    QString velocityUnits;
    double diameterScaleConversion;
    double diameterScalePixelHeight;
    double velocityScaleConversion;
    double velocityScalePixelHeight;
    double velocityXSeconds;
    double velocityXAxisLocation;
};

bool operator!=(const MLogMetaData& lhs, const MLogMetaData& rhs);

Q_DECLARE_METATYPE(MLogMetaData)

#endif // MLOGMETADATA_H
