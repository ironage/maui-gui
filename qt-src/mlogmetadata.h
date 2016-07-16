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
    Q_PROPERTY(QString conversionUnits MEMBER units NOTIFY propertiesChanged)
    Q_PROPERTY(int conversionPixels MEMBER pixels NOTIFY propertiesChanged)
public:
    explicit MLogMetaData(QObject *parent = 0);
    MLogMetaData(const MLogMetaData& other);

    QString getFileName() const { return fileName; }
    QString getFilePath() const { return filePath; }
    QString getTimestamp() const;
    int getPixels() const { return pixels; }
    QString getUnits() const { return units; }
    void setFileName(QString name) { fileName = name; }
    void setFilePath(QString path) { filePath = path; }
    void setPixels(int numPixels) { pixels = numPixels; }
    void setUnits(QString newUnits) { units = newUnits; }
    std::vector<QString> getHeader() const;
    void operator=(const MLogMetaData& other);
signals:
    void propertiesChanged();
public slots:

private:
    QString fileName;
    QString filePath;
    int pixels;
    QString units;
};

bool operator!=(const MLogMetaData& lhs, const MLogMetaData& rhs);

Q_DECLARE_METATYPE(MLogMetaData)

#endif // MLOGMETADATA_H
