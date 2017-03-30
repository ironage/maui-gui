#include "mdatalog.h"

#include <algorithm>

#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QTextStream>

#include "mresultswriter.h"

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

QString MDataEntry::getILTPixels() const
{
    if (OLDPixels != 0 && topIMTPixels != 0 && bottomIMTPixels != 0) {
        return QString::number(OLDPixels - topIMTPixels - bottomIMTPixels);
    }
    return QString("NaN");
}

QString MDataEntry::getILTUnits(double conversion) const
{
    if (OLDPixels != 0 && topIMTPixels != 0 && bottomIMTPixels != 0) {
        return QString::number((OLDPixels - topIMTPixels - bottomIMTPixels) * conversion);
    }
    return QString("NaN");
}

void MDataLog::add(MDataEntry &&entry)
{
    entries[entry.getFrameNumber()] = std::move(entry);
}

void MDataLog::write(QString fileName)
{
    metaData.touchWriteTime();
    QString dataFileName     = fileName + "_data_"     + metaData.getWriteTime() + ".csv";
    QString velocityFileName = fileName + "_velocity_" + metaData.getWriteTime() + ".csv";
    QString combinedFileName = fileName + "_combined_" + metaData.getWriteTime() + ".csv";

    MDiameterWriter dWriter(dataFileName, metaData);
    MVelocityWriter vWriter(velocityFileName, metaData);
    MCombinedWriter cWriter(combinedFileName, metaData);

    std::vector<MResultsWriter*> writers = { &dWriter, &vWriter, &cWriter };

    for (MResultsWriter *writer : writers) {
        std::unique_ptr<QFile> file = writer->open();
        if (!file) {
            qDebug() << "skipping output on invalid writer file.";
            continue;
        }
        QTextStream out(file.get());
        std::vector<QString> header = writer->getMetaDataHeader();
        size_t maxLines = std::max(header.size(), entries.size()) + 1; // + 1 is for top line header
        std::map<int, MDataEntry>::iterator curData = entries.begin();
        int velocityIndex = 0;
        for (int i = 0; i < maxLines || curData != entries.end();) {
            QString curLine;
            if (i < header.size()) {
                curLine += header[i];
            }
            curLine += ",";
            if (i == 0) {
                curLine += writer->getHeader();
            } else {
                if (curData != entries.end()) {
                    QString velocityLine = writer->getEntry(curData->second, velocityIndex);
                    if (velocityLine.isEmpty()) {
                        velocityIndex = 0;
                        ++curData;
                        continue;
                    } else {
                        curLine += velocityLine;
                        ++velocityIndex;
                    }
                } else {
                    curLine += writer->getEmptyEntry();
                }
            }
            curLine += "\n";
            out << curLine;
            ++i;
        }
        // file is flushed and closed on destruction
    }
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

void MDataLog::setVelocityXAxisLocation(double xLoc)
{
    metaData.setVelocityXAxisLocation(xLoc);
}

MDataLog::MDataLog()
{
}

void MDataLog::initialize(MLogMetaData data)
{
    metaData = data;
}
