#include "mlogmetadata.h"
#include "mremoteinterface.h"

#include <QDateTime>

MLogMetaData::MLogMetaData(QObject *parent)
    : QObject(parent),
        units("cm"),
        diameterScaleConversion(1),
        diameterScalePixelHeight(1),
        velocityScaleConversion(1),
        velocityScalePixelHeight(1),
        velocityUnits("cm/s"),
        velocityXSeconds(1),
        velocityXAxisLocation(0)
{
    uniqueness = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");
}

MLogMetaData::MLogMetaData(const MLogMetaData &other)
    : fileName(other.fileName), filePath(other.filePath),
      outputDirectory(other.outputDirectory),
      units(other.units), uniqueness(other.uniqueness),
      diameterScaleConversion(other.diameterScaleConversion),
      diameterScalePixelHeight(other.diameterScalePixelHeight),
      velocityScaleConversion(other.velocityScaleConversion),
      velocityScalePixelHeight(other.velocityScalePixelHeight),
      velocityUnits(other.velocityUnits),
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
    units = other.units;
    outputDirectory = other.outputDirectory;
    uniqueness = other.uniqueness;
    velocityUnits = other.velocityUnits;
    velocityXSeconds = other.velocityXSeconds;
    velocityXAxisLocation = other.velocityXAxisLocation;
    diameterScaleConversion = other.diameterScaleConversion;
    diameterScalePixelHeight = other.diameterScalePixelHeight;
    velocityScaleConversion = other.velocityScaleConversion;
    velocityScalePixelHeight = other.velocityScalePixelHeight;
}

bool operator!=(const MLogMetaData& lhs, const MLogMetaData& rhs) {
    return lhs.getFileName() != rhs.getFileName()
            || lhs.getFilePath() != rhs.getFilePath()
            || lhs.getDiameterScaleConversion() != rhs.getDiameterScaleConversion()
            || lhs.getDiameterScalePixelHeight() != rhs.getDiameterScalePixelHeight()
            || lhs.getUnits() != rhs.getUnits()
            || lhs.getOutputDir() != rhs.getOutputDir()
            || lhs.getVelocityUnits() != rhs.getVelocityUnits()
            || lhs.getVelocityScaleConversion() != rhs.getVelocityScaleConversion()
            || lhs.getVelocityScalePixelHeight() != rhs.getVelocityScaleConversion()
            || lhs.getVelocityXSeconds() != rhs.getVelocityXSeconds()
            || lhs.getVelocityXAxisLocation() != rhs.getVelocityXAxisLocation();
    // uniqueness not necessary here because it may change in the future.
}

