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
    Q_PROPERTY(double conversionPixels MEMBER pixels NOTIFY propertiesChanged)
    Q_PROPERTY(QString velocityConversionUnits MEMBER velocityUnits NOTIFY propertiesChanged)
    Q_PROPERTY(double velocityConversionPixels MEMBER velocityPixels NOTIFY propertiesChanged)
    Q_PROPERTY(double velocityTime MEMBER velocityXSeconds NOTIFY propertiesChanged)
public:
    explicit MLogMetaData(QObject *parent = 0);
    MLogMetaData(const MLogMetaData& other);

    QString getFileName() const { return fileName; }
    QString getFilePath() const { return filePath; }
    QString getTimestamp() const;
    double getPixels() const { return pixels; }
    QString getUnits() const { return units; }
    QString getVelocityUnits() const { return velocityUnits; }
    QString getOutputDir() const { return outputDirectory; }
    double getVelocityPixels() const { return velocityPixels; }
    double getVelocityXSeconds() const { return velocityXSeconds; }
    double getVelocityXAxisLocation() const { return velocityXAxisLocation; }
    void setFileName(QString name) { fileName = name; }
    void setFilePath(QString path) { filePath = path; }
    void setPixels(double numPixels) { pixels = numPixels; }
    void setUnits(QString newUnits) { units = newUnits; }
    void setVelocityUnits(QString newUnits) { velocityUnits = newUnits; }
    void setVelocityPixels(double numPixels) { velocityPixels = numPixels; }
    void setVelocityXSeconds(double numSeconds) { velocityXSeconds = numSeconds; }
    void setVelocityXAxisLocation(double xLoc) { velocityXAxisLocation = xLoc; }
    void touchWriteTime();
    QString getWriteTime() { return uniqueness; }
    std::vector<QString> getHeader() const;
    std::vector<QString> getVelocityHeader() const;
    void operator=(const MLogMetaData& other);
signals:
    void propertiesChanged();

private:
    QString fileName;
    QString filePath;
    QString outputDirectory;
    double pixels;
    QString units;
    QString uniqueness;
    QString velocityUnits;
    double velocityPixels;
    double velocityXSeconds;
    double velocityXAxisLocation;
};

bool operator!=(const MLogMetaData& lhs, const MLogMetaData& rhs);

Q_DECLARE_METATYPE(MLogMetaData)

#endif // MLOGMETADATA_H
