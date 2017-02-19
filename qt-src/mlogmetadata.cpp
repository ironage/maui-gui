#include "mlogmetadata.h"
#include "mremoteinterface.h"

#include <QDateTime>

MLogMetaData::MLogMetaData(QObject *parent) : QObject(parent), pixels(1), units("undefined")
{
    uniqueness = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");
}

MLogMetaData::MLogMetaData(const MLogMetaData &other)
    : fileName(other.fileName), filePath(other.filePath),
      outputDirectory(other.outputDirectory),
      pixels(other.pixels), units(other.units), uniqueness(other.uniqueness)
{
}

QString MLogMetaData::getTimestamp() const
{
    return QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
}

void MLogMetaData::touchWriteTime()
{
    uniqueness = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");
}

std::vector<QString> MLogMetaData::getHeader() const
{
    QString conversion = QString("") + QString::number(getPixels()) + " pixels = 1 " + getUnits();
    QString version = "MAUI version " + MRemoteInterface::getDisplayVersion();
    return std::vector<QString> {getFileName(), getFilePath(), getTimestamp(), conversion, version};
}

void MLogMetaData::operator=(const MLogMetaData &other)
{
    fileName = other.fileName;
    filePath = other.filePath;
    pixels = other.pixels;
    units = other.units;
    outputDirectory = other.outputDirectory;
    uniqueness = other.uniqueness;
}

bool operator!=(const MLogMetaData& lhs, const MLogMetaData& rhs) {
    return lhs.getFileName() != rhs.getFileName()
            || lhs.getFilePath() != rhs.getFilePath()
            || lhs.getPixels() != rhs.getPixels()
            || lhs.getUnits() != rhs.getUnits()
            || lhs.getOutputDir() != rhs.getOutputDir();
    // uniqueness not necessary here because it may change in the future.
}

