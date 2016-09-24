#include "mlogmetadata.h"
#include "mremoteinterface.h"

#include <QDateTime>

MLogMetaData::MLogMetaData(QObject *parent) : QObject(parent), pixels(1), units("undefined")
{
}

MLogMetaData::MLogMetaData(const MLogMetaData &other)
    : fileName(other.fileName), filePath(other.filePath),
      outputFileName(other.outputFileName), outputDirectory(other.outputDirectory),
      pixels(other.pixels), units(other.units)
{
}

QString MLogMetaData::getTimestamp() const
{
    return QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
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
    outputFileName = other.outputFileName;
    outputDirectory = other.outputDirectory;
}

bool operator!=(const MLogMetaData& lhs, const MLogMetaData& rhs) {
    return lhs.getFileName() != rhs.getFileName()
            || lhs.getFilePath() != rhs.getFilePath()
            || lhs.getPixels() != rhs.getPixels()
            || lhs.getUnits() != rhs.getUnits()
            || lhs.getOutputDir() != rhs.getOutputDir()
            || lhs.getOutputName() != rhs.getOutputName();
}

