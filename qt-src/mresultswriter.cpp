#include "mresultswriter.h"
#include "mdatalog.h"
#include "mremoteinterface.h"

#include <QDateTime>

#include <cmath>

MResultsWriter::MResultsWriter(QString name, MLogMetaData &attachedMetaData)
    : filename(name), metaData(attachedMetaData)
{
}

template<typename T>
QString MResultsWriter::getString(T value) const
{
    if (value == 0) {
        return QString("NaN");
    }
    return QString::number(value);
}

std::unique_ptr<QFile> MResultsWriter::open()
{
    std::unique_ptr<QFile> file = std::make_unique<QFile>(filename);
    if (file && file->exists()) { // With dates on each file, this shouldn't happen!
        qDebug() << "File exsists already! Adding date to avoid conflict " << filename;
        file->setFileName(filename + QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss") + ".csv");
    }
    if (file && !file->open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Could not open csv file for writing: " << file->fileName();
    }
    return file;
}

MDiameterWriter::MDiameterWriter(QString name, MLogMetaData &attachedMetaData)
    : MResultsWriter(name, attachedMetaData)
{
    conversion = metaData.getPixels() > 0 ? 1.0 / metaData.getPixels() : 1;
}

QString MDiameterWriter::getHeader() const
{
    QString units = metaData.getUnits();
    return QString("frame number,media-media distance(pixels),Top intima-media distance(pixels),"
                   "Bottom intima-media distance(pixels),time(seconds),media-media distance(")
            + units + "),Top intima-media distance(" + units + "),Bottom intima-media distance("
            + units + "),intima-intima distance(pixels),intima-intima distance(" + units + ")";
}

std::vector<QString> MDiameterWriter::getMetaDataHeader() const
{
    QString conversion = QString("") + QString::number(metaData.getPixels()) + " pixels = 1 " + metaData.getUnits();
    QString version = "MAUI version " + MRemoteInterface::getDisplayVersion();
    return std::vector<QString> { metaData.getFileName(), metaData.getFilePath(),
                metaData.getTimestamp(), conversion, version};
}

QString MDiameterWriter::getEmptyEntry() const
{
    return QString(",,,,,,,,,");
}

QString MDiameterWriter::getEntry(const MDataEntry &entry, int index) const
{
    if (index) { return QString(); } // empty for all but index 0
    return QString() + QString::number(entry.getFrameNumber()) + ","
                     + MResultsWriter::getString(entry.getOLDPixels()) + ","
                     + MResultsWriter::getString(entry.getTopIMTPixels()) + ","
                     + MResultsWriter::getString(entry.getBottomIMTPixels()) + ","
                     + QString::number(entry.getTime()) + ","
                     + MResultsWriter::getString(entry.getOLDPixels() * conversion) + ","
                     + MResultsWriter::getString(entry.getTopIMTPixels() * conversion) + ","
                     + MResultsWriter::getString(entry.getBottomIMTPixels() * conversion) + ","
                     + entry.getILTPixels() + ","
                     + entry.getILTUnits(conversion);
}

// MVelocityWriter

MVelocityWriter::MVelocityWriter(QString name, MLogMetaData &attachedMetaData)
    : MResultsWriter(name, attachedMetaData)
{
    conversion = metaData.getVelocityPixels() > 0 ? 1.0 / metaData.getVelocityPixels() : 1;
}

QString MVelocityWriter::getHeader() const
{
    QString units = metaData.getVelocityUnits();
    return QString("frame number,time(seconds),xLocation,"
                   "max positive(pixels),avg positive(pixels),avg negative(pixels),max negative(pixels),"
                   "max positive(" + units + "),"
                   "avg positive(" + units + "),"
                   "avg negative(" + units + "),"
                   "max negative(" + units + ")");
}

std::vector<QString> MVelocityWriter::getMetaDataHeader() const
{
    QString conversion = QString("") + QString::number(metaData.getVelocityPixels()) + " pixels = 1 " + metaData.getVelocityUnits();
    QString version = "MAUI version " + MRemoteInterface::getDisplayVersion();
    return std::vector<QString> {metaData.getFileName(), metaData.getFilePath(),
                metaData.getTimestamp(), conversion, version };
}

QString MVelocityWriter::getEmptyEntry() const
{
    return QString(",,,,,,,,,,");
}

QString MVelocityWriter::getEntry(const MDataEntry &entry, int index) const
{
    const VelocityResults &velocity = entry.getVelocity();
    if (index >= 0
            && index < velocity.maxPositive.size()
            && index < velocity.avgPositive.size()
            && index < velocity.avgNegative.size()
            && index < velocity.maxNegative.size()) {
        double xAxisLocation = metaData.getVelocityXAxisLocation();
        return QString() + QString::number(entry.getFrameNumber()) + ","
                         + QString::number(entry.getTime()) + ","
                         + QString::number(velocity.xTrackingLocationIndividual[index]) + ","
                         + QString::number(xAxisLocation - velocity.maxPositive[index]) + ","
                         + QString::number(xAxisLocation - velocity.avgPositive[index]) + ","
                         + QString::number(xAxisLocation - velocity.avgNegative[index]) + ","
                         + QString::number(xAxisLocation - velocity.maxNegative[index]) + ","
                         + QString::number((xAxisLocation - velocity.maxPositive[index]) * conversion) + ","
                         + QString::number((xAxisLocation - velocity.avgPositive[index]) * conversion) + ","
                         + QString::number((xAxisLocation - velocity.avgNegative[index]) * conversion) + ","
                         + QString::number((xAxisLocation - velocity.maxNegative[index]) * conversion);
    } else {
        return QString();
    }
}

// MCombinedWriter

MCombinedWriter::MCombinedWriter(QString name, MLogMetaData &attachedMetaData)
    : MResultsWriter(name, attachedMetaData), vWriter("", attachedMetaData), dWriter("", attachedMetaData)
{
    QString diameterUnits = metaData.getUnits();
    if (diameterUnits.compare("in", Qt::CaseInsensitive) == 0) {
        diameterToCMConversion = 2.54;
    } else if (diameterUnits.compare("mm", Qt::CaseInsensitive) == 0) {
        diameterToCMConversion = 0.10;
    } else {
        if (!diameterUnits.compare("cm", Qt::CaseInsensitive) == 0) {
            qDebug() << "Unknown diameter units! assuming cm! " << diameterUnits;
        }
        diameterToCMConversion = 1;
    }

    QString velocityUnits = metaData.getVelocityUnits();
    if (velocityUnits.compare("in/s", Qt::CaseInsensitive) == 0) {
        flowUnits = "fl oz/min";
        velocityConversionType = IN_PER_SECOND;
    } else if (velocityUnits.compare("cm/s", Qt::CaseInsensitive) == 0) {
        flowUnits = "mL/min";
        velocityConversionType = CM_PER_SECOND;
    } else {
        if (!velocityUnits.compare("mm/s", Qt::CaseInsensitive) == 0) {
            qDebug() << "unhandled flow units! defaulting to mm/s!" << velocityUnits;
        }
        flowUnits = "mL/min";
        velocityConversionType = MM_PER_SECOND;
    }
}

QString MCombinedWriter::getHeader() const
{
    QString units = metaData.getVelocityUnits();
    return dWriter.getHeader() + ",velocity xLocation,"
                                 "velocity max positive(pixels),"
                                 "velocity avg positive(pixels),"
                                 "velocity avg negative(pixels),"
                                 "velocity max negative(pixels),"
                                 "velocity max positive(" + units + "),"
                                 "velocity avg positive(" + units + "),"
                                 "velocity avg negative(" + units + "),"
                                 "velocity max negative(" + units + "),"
                                 "flow max positive(" + flowUnits + "),"
                                 "flow avg positive(" + flowUnits + "),"
                                 "flow avg negative(" + flowUnits + "),"
                                 "flow max negative(" + flowUnits + ")";
}

std::vector<QString> MCombinedWriter::getMetaDataHeader() const
{
    QString conversionDiameter = QString("") + QString::number(metaData.getPixels()) + " pixels = 1 " + metaData.getUnits();
    QString conversionVelocity = QString("") + QString::number(metaData.getVelocityPixels()) + " pixels = 1 " + metaData.getVelocityUnits();
    QString version = "MAUI version " + MRemoteInterface::getDisplayVersion();
    return std::vector<QString> {metaData.getFileName(), metaData.getFilePath(),
                metaData.getTimestamp(), conversionDiameter, conversionVelocity, version};
}

QString MCombinedWriter::getEmptyEntry() const
{
    return dWriter.getEmptyEntry() + getVelocityEmptyEntry();
}

QString MCombinedWriter::getEntry(const MDataEntry &entry, int index) const
{
    QString velocityEntry = getVelocityEntry(entry, index);
    QString diameterEntry = dWriter.getEntry(entry, 0);
    if (index == 0) { // always at least write one entry on this frame
        if (velocityEntry.isEmpty()) {
            velocityEntry = getVelocityEmptyEntry();
        }
        return diameterEntry + velocityEntry;
    } else {
        if (!velocityEntry.isEmpty()) {
            return diameterEntry + velocityEntry;
        }
    }
    return QString(); // no more data for this entry
}

double MCombinedWriter::calculateFlow(double diameter, double velocity) const
{
    if (diameter <= 0 || velocity <= 0) {
        return 0;
    }
    const long double LOCAL_PI = 3.141592653589793238L;
    const double secondsPerMinute = 60.0;
    const double mmPerCm = 0.10;
    const double inPerCm = 0.393701;
    const double mLToFlOz = 0.03381402;
    // cross sectional area * velocity * 60 (seconds to minutes)
    // (PI * r^2) * velocity * 60
    switch (velocityConversionType) {
    case IN_PER_SECOND:
        return (LOCAL_PI * pow(((diameter * diameterToCMConversion) / 2), 2)) * (velocity * inPerCm) * secondsPerMinute * mLToFlOz;
    case CM_PER_SECOND:
        return (LOCAL_PI * pow(((diameter * diameterToCMConversion) / 2), 2)) * velocity * secondsPerMinute;
    default:
    case MM_PER_SECOND:
        return (LOCAL_PI * pow(((diameter * diameterToCMConversion) / 2), 2)) * (velocity * mmPerCm) * secondsPerMinute;
    }
}

QString MCombinedWriter::getVelocityEmptyEntry() const
{
    return ",,,,,,,,,,,,,";
}

QString MCombinedWriter::getVelocityEntry(const MDataEntry &entry, int index) const
{
    const VelocityResults &velocity = entry.getVelocity();
    if (index >= 0
            && index < velocity.maxPositive.size()
            && index < velocity.avgPositive.size()
            && index < velocity.avgNegative.size()
            && index < velocity.maxNegative.size()) {
        double xAxisLocation = metaData.getVelocityXAxisLocation();
        double velocityConversion = vWriter.getVelocityConversion();
        double diameterConversion = dWriter.getDiameterConversion();
        double diameter = entry.getOLDPixels() * diameterConversion;

        double maxPosV = (xAxisLocation - velocity.maxPositive[index]) * velocityConversion;
        double avgPosV = (xAxisLocation - velocity.avgPositive[index]) * velocityConversion;
        double avgNegV = (xAxisLocation - velocity.avgNegative[index]) * velocityConversion;
        double maxNegV = (xAxisLocation - velocity.maxNegative[index]) * velocityConversion;


        return QString() + ","
                         + QString::number(velocity.xTrackingLocationIndividual[index]) + ","
                         + QString::number(xAxisLocation - velocity.maxPositive[index]) + ","
                         + QString::number(xAxisLocation - velocity.avgPositive[index]) + ","
                         + QString::number(xAxisLocation - velocity.avgNegative[index]) + ","
                         + QString::number(xAxisLocation - velocity.maxNegative[index]) + ","
                         + QString::number(maxPosV) + ","
                         + QString::number(avgPosV) + ","
                         + QString::number(avgNegV) + ","
                         + QString::number(maxNegV) + ","
                         + QString::number(calculateFlow(diameter, maxPosV)) + ","
                         + QString::number(calculateFlow(diameter, avgPosV)) + ","
                         + QString::number(calculateFlow(diameter, avgNegV)) + ","
                         + QString::number(calculateFlow(diameter, maxNegV));
    } else {
        return QString();
    }
}
