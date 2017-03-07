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
MDataEntry::MDataEntry(int frame)
    : frameNumber(frame), OLDPixels(-1), topIMTPixels(-1), bottomIMTPixels(-1), timeSeconds(-1)
{
}

MDataEntry::MDataEntry()
    : frameNumber(-1), OLDPixels(-1), topIMTPixels(-1), bottomIMTPixels(-1), timeSeconds(-1)
{
}

void MDataEntry::addWallPart(double old, double topIMT, double bottomIMT, double time,
                        std::vector<cv::Point> &&topStrong,
                        std::vector<cv::Point> &&topWeak,
                        std::vector<cv::Point> &&bottomStrong,
                        std::vector<cv::Point> &&bottomWeak)
{
    OLDPixels = old;
    topIMTPixels = topIMT;
    bottomIMTPixels = bottomIMT;
    timeSeconds = time;
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

QString MDataEntry::getHeader(QString units)
{
    return QString("frame number,media-media distance(pixels),Top intima-media distance(pixels),"
                   "Bottom intima-media distance(pixels),time(seconds),media-media distance(")
            + units + "),Top intima-media distance(" + units + "),Bottom intima-media distance("
            + units + "),intima-intima distance(pixels),intima-intima distance(" + units + ")";
}

QString MDataEntry::getEmptyEntry()
{
    return QString(",,,,,,,,,");
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
    QFile file(fileName + metaData.getWriteTime() + ".csv");
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
