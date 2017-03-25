#include "mlogmetadata.h"
#include "mremoteinterface.h"

#include <QDateTime>

MLogMetaData::MLogMetaData(QObject *parent) : QObject(parent), pixels(1), units("undefined"),
  velocityUnits("undefined"), velocityPixels(1), velocityXSeconds(1), velocityXAxisLocation(0)
{
    uniqueness = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");
}

MLogMetaData::MLogMetaData(const MLogMetaData &other)
    : fileName(other.fileName), filePath(other.filePath),
      outputDirectory(other.outputDirectory),
      pixels(other.pixels), units(other.units), uniqueness(other.uniqueness),
      velocityUnits(other.velocityUnits), velocityPixels(other.velocityPixels),
      velocityXSeconds(other.velocityXSeconds), velocityXAxisLocation(other.velocityXAxisLocation)
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

void MLogMetaData::operator=(const MLogMetaData &other)
{
    fileName = other.fileName;
    filePath = other.filePath;
    pixels = other.pixels;
    units = other.units;
    outputDirectory = other.outputDirectory;
    uniqueness = other.uniqueness;
    velocityUnits = other.velocityUnits;
    velocityPixels = other.velocityPixels;
    velocityXSeconds = other.velocityXSeconds;
    velocityXAxisLocation = other.velocityXAxisLocation;
}

bool operator!=(const MLogMetaData& lhs, const MLogMetaData& rhs) {
    return lhs.getFileName() != rhs.getFileName()
            || lhs.getFilePath() != rhs.getFilePath()
            || lhs.getPixels() != rhs.getPixels()
            || lhs.getUnits() != rhs.getUnits()
            || lhs.getOutputDir() != rhs.getOutputDir()
            || lhs.getVelocityUnits() != rhs.getVelocityUnits()
            || lhs.getVelocityPixels() != rhs.getVelocityPixels()
            || lhs.getVelocityXSeconds() != rhs.getVelocityXSeconds()
            || lhs.getVelocityXAxisLocation() != rhs.getVelocityXAxisLocation();
    // uniqueness not necessary here because it may change in the future.
}

