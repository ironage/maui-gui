#include "mdatalog.h"

#include <algorithm>

#include <QDebug>
#include <QFile>
#include <QTextStream>

MDataEntry::MDataEntry(int frame, double old, double topIMT, double bottomIMT, double time)
    : frameNumber(frame), OLDPixels(old), topIMTPixels(topIMT), bottomIMTPixels(bottomIMT),
      timeSeconds(time)
{
}

QString MDataEntry::getCSV(double conversion)
{
    return QString() + QString::number(frameNumber) + ","
                     + QString::number(OLDPixels) + ","
                     + QString::number(topIMTPixels) + ","
                     + QString::number(bottomIMTPixels) + ","
                     + QString::number(timeSeconds) + ","
                     + QString::number(OLDPixels * conversion) + ","
                     + QString::number(topIMTPixels * conversion) + ","
                     + QString::number(bottomIMTPixels * conversion);
}

QString MDataEntry::getHeader(QString units)
{
    return QString("frame number, OLD distance(pixels),Top IMT distance(pixels),"
                   "Bottom IMT distance(pixels),time(seconds),OLD distance(")
            + units + "),Top IMT distance(" + units + "),Bottom IMT distance("
            + units + ")";
}

QString MDataEntry::getEmptyEntry()
{
    return QString(",,,,,,,");
}

void MDataLog::add(MDataEntry entry)
{
    entries.push_back(entry);
}

void MDataLog::write(QString fileName)
{
    QFile file(fileName);
    if (file.exists()) {
        qDebug() << "File exsists already! Aborting! " << fileName;
        return;
    }
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Could not open csv file for writing: " << fileName;
        return;
    }

    QTextStream out(&file);
    std::vector<QString> header = metaData.getHeader();
    double conversion = metaData.getPixels() > 0 ? 1 / metaData.getPixels() : 1;
    size_t maxLines = std::max(header.size(), entries.size());
    for (int i = 0; i < maxLines + 1; i++) {
        if (i < header.size()) {
            out << header[i];
        }
        out << ",";
        if (i == 0) {
            out << MDataEntry::getHeader(metaData.getUnits());
        } else {
            if (i - 1 < entries.size()) {
                out << entries[i - 1].getCSV(conversion);
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
