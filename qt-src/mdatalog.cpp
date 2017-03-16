#include "mdatalog.h"

#include <algorithm>

#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QTextStream>

QDebug operator<<(QDebug debug, const VelocityResults &r)
{
    QDebugStateSaver saver(debug);
//    debug.nospace() << "VelocityResults(" << r.maxPositive << ", "
//                    << r.avgPositive << ", "
//                    << r.maxNegative << ", "
//                    << r.avgNegative << ", "
//                    << r.xTrackingLocationIndividual << ", "
//                    << ')';
    debug.nospace() << "VelocityResults(with " << r.xTrackingLocationIndividual.size() << " elements)";
    return debug;
}

// This default ctr is just for iterator support via std::map
MDataEntry::MDataEntry(int frame, double time)
    : frameNumber(frame), OLDPixels(-1), topIMTPixels(-1), bottomIMTPixels(-1), timeSeconds(time)
{
}

MDataEntry::MDataEntry()
    : frameNumber(-1), OLDPixels(-1), topIMTPixels(-1), bottomIMTPixels(-1), timeSeconds(-1)
{
}

void MDataEntry::addWallPart(double old, double topIMT, double bottomIMT,
                        std::vector<cv::Point> &&topStrong,
                        std::vector<cv::Point> &&topWeak,
                        std::vector<cv::Point> &&bottomStrong,
                        std::vector<cv::Point> &&bottomWeak)
{
    OLDPixels = old;
    topIMTPixels = topIMT;
    bottomIMTPixels = bottomIMT;
    topStrongLine = std::move(topStrong);
    topWeakLine = std::move(topWeak);
    bottomStrongLine = std::move(bottomStrong);
    bottomWeakLine = std::move(bottomWeak);
}

void MDataEntry::addVelocityPart(VelocityResults &&vResults)
{
    velocity = std::move(vResults);
}

QString MDataEntry::getCSV(double conversion)
{
    return QString() + QString::number(frameNumber) + ","
                     + MDataEntry::getString(OLDPixels) + ","
                     + MDataEntry::getString(topIMTPixels) + ","
                     + MDataEntry::getString(bottomIMTPixels) + ","
                     + QString::number(timeSeconds) + ","
                     + MDataEntry::getString(OLDPixels * conversion) + ","
                     + MDataEntry::getString(topIMTPixels * conversion) + ","
                     + MDataEntry::getString(bottomIMTPixels * conversion) + ","
                     + getILTPixels() + ","
                     + getILTUnits(conversion);
}

QString MDataEntry::getVelocityCSV(double conversion, int index)
{
    if (index >= 0 && index < velocity.maxPositive.size() && index < velocity.avgPositive.size()
            && index < velocity.avgNegative.size() && index < velocity.maxNegative.size()) {
        return QString() + QString::number(frameNumber) + ","
                         + QString::number(timeSeconds) + ","
                         + QString::number(velocity.xTrackingLocationIndividual[index]) + ","
                         + QString::number(velocity.maxPositive[index]) + ","
                         + QString::number(velocity.avgPositive[index]) + ","
                         + QString::number(velocity.avgNegative[index]) + ","
                         + QString::number(velocity.maxNegative[index]) + ","
                         + QString::number(velocity.maxPositive[index] * conversion) + ","
                         + QString::number(velocity.avgPositive[index] * conversion) + ","
                         + QString::number(velocity.avgNegative[index] * conversion) + ","
                         + QString::number(velocity.maxNegative[index] * conversion);

    } else {
        return QString();
    }
}

QString MDataEntry::getHeader(QString units)
{
    return QString("frame number,media-media distance(pixels),Top intima-media distance(pixels),"
                   "Bottom intima-media distance(pixels),time(seconds),media-media distance(")
            + units + "),Top intima-media distance(" + units + "),Bottom intima-media distance("
            + units + "),intima-intima distance(pixels),intima-intima distance(" + units + ")";
}

QString MDataEntry::getVelocityHeader(QString units)
{
    return QString("frame number,time(seconds),xLocation,"
                   "max positive(pixels),avg positive(pixels),avg negative(pixels),max negative(pixels),"
                   "max positive(" + units + "),"
                   "avg positive(" + units + "),"
                   "avg negative(" + units + "),"
                   "max negative(" + units + ")");
}

QString MDataEntry::getEmptyEntry()
{
    return QString(",,,,,,,,,");
}

QString MDataEntry::getEmptyVelocityEntry()
{
    return QString(",,,,,,,,,,");
}

QString MDataEntry::getILTPixels()
{
    if (OLDPixels != 0 && topIMTPixels != 0 && bottomIMTPixels != 0) {
        return QString::number(OLDPixels - topIMTPixels - bottomIMTPixels);
    }
    return QString("NaN");
}

QString MDataEntry::getILTUnits(double conversion)
{
    if (OLDPixels != 0 && topIMTPixels != 0 && bottomIMTPixels != 0) {
        return QString::number((OLDPixels - topIMTPixels - bottomIMTPixels) * conversion);
    }
    return QString("NaN");
}

template<typename T>
QString MDataEntry::getString(T value)
{
    if (value == 0) {
        return QString("NaN");
    }
    return QString::number(value);
}

void MDataLog::add(MDataEntry &&entry)
{
    entries[entry.getFrameNumber()] = std::move(entry);
}

void MDataLog::write(QString fileName)
{
    metaData.touchWriteTime();
    QString dataFileName     = fileName + "_data"     + metaData.getWriteTime() + ".csv";
    QString velocityFileName = fileName + "_velocity" + metaData.getWriteTime() + ".csv";
    writeVelocity(velocityFileName);
    QFile file(dataFileName);
    if (file.exists()) { // With dates on each file, this shouldn't happen!
        qDebug() << "File exsists already! Adding date to avoid conflict " << fileName;
        file.setFileName(fileName + QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss") + ".csv");
    }
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Could not open csv file for writing: " << file.fileName();
        return;
    }

    QTextStream out(&file);
    std::vector<QString> header = metaData.getHeader();
    double conversion = metaData.getPixels() > 0 ? 1.0 / metaData.getPixels() : 1;
    size_t maxLines = std::max(header.size(), entries.size());
    std::map<int, MDataEntry>::iterator curData = entries.begin();
    for (int i = 0; i < maxLines + 1; i++) {
        if (i < header.size()) {
            out << header[i];
        }
        out << ",";
        if (i == 0) {
            out << MDataEntry::getHeader(metaData.getUnits());
        } else {
            if (curData != entries.end()) {
                out << curData->second.getCSV(conversion);
                ++curData;
            } else {
                out << MDataEntry::getEmptyEntry();
            }
        }
        out << "\n";
    }
    // file is flushed and closed on destruction
}

void MDataLog::writeVelocity(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Could not open csv file for writing: " << file.fileName();
        return;
    }

    QTextStream out(&file);
    std::vector<QString> header = metaData.getVelocityHeader();
    double conversion = metaData.getVelocityPixels() > 0 ? 1.0 / metaData.getVelocityPixels() : 1;
    size_t maxLines = std::max(header.size(), entries.size());
    std::map<int, MDataEntry>::iterator curData = entries.begin();
    int velocityIndex = 0;
    for (int i = 0; i < maxLines + 1 || curData != entries.end(); i++) {
        QString curLine;
        if (i < header.size()) {
            curLine += header[i];
        }
        curLine += ",";
        if (i == 0) {
            curLine += MDataEntry::getVelocityHeader(metaData.getVelocityUnits());
        } else {
            if (curData != entries.end()) {
                QString velocityLine = curData->second.getVelocityCSV(conversion, velocityIndex);
                if (velocityLine.isEmpty()) {
                    velocityIndex = 0;
                    ++curData;
                    continue;
                } else {
                    curLine += velocityLine;
                    ++velocityIndex;
                }
            } else {
                curLine += MDataEntry::getEmptyVelocityEntry();
            }
        }
        curLine += "\n";
        out << curLine;
    }
    // file is flushed and closed on destruction
}

void MDataLog::clear()
{
    entries.clear();
}

const MDataEntry* MDataLog::get(int frame) const
{
    auto needle = entries.find(frame);
    if (needle != entries.end()) {
        return &(needle->second);
    }
    return nullptr;
}

MDataLog::MDataLog()
{
}

void MDataLog::initialize(MLogMetaData data)
{
    metaData = data;
}
