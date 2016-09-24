#include "mdatalog.h"

#include <algorithm>

#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QTextStream>

MDataEntry::MDataEntry(int frame, double old, double topIMT, double bottomIMT, double time)
    : frameNumber(frame), OLDPixels(old), topIMTPixels(topIMT), bottomIMTPixels(bottomIMT),
      timeSeconds(time)
{
}

// This default ctr is just for iterator support via std::map
MDataEntry::MDataEntry()
    : frameNumber(-1), OLDPixels(-1), topIMTPixels(-1), bottomIMTPixels(-1), timeSeconds(-1)
{
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
    return QString("frame number, OLD distance(pixels),Top IMT distance(pixels),"
                   "Bottom IMT distance(pixels),time(seconds),OLD distance(")
            + units + "),Top IMT distance(" + units + "),Bottom IMT distance("
            + units + "),ILT distance(pixels),ILT distance(" + units + ")";
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

void MDataLog::add(MDataEntry entry)
{
    entries[entry.getFrameNumber()] = entry;
}

void MDataLog::write(QString fileName)
{
    QFile file(fileName + ".csv");
    if (file.exists()) {
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

MDataLog::MDataLog()
{
}

void MDataLog::initialize(MLogMetaData data)
{
    metaData = data;
}
