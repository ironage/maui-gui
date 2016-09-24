#ifndef MDATALOG_H
#define MDATALOG_H

#include <QString>

#include <map>

#include "mlogmetadata.h"

class MDataEntry
{
public:
    MDataEntry(int frame, double old, double topIMT, double bottomIMT, double time);
    MDataEntry();

    int getFrameNumber() const { return frameNumber; }
    QString getCSV(double conversion);
    static QString getHeader(QString units);
    static QString getEmptyEntry();
    template<typename T> static QString getString(T value);
private:
    QString getILTPixels();
    QString getILTUnits(double conversion);
    int frameNumber;
    double OLDPixels;
    double topIMTPixels;
    double bottomIMTPixels;
    double timeSeconds;
};

class MDataLog
{
public:
    MDataLog();
    void initialize(MLogMetaData data);
    void add(MDataEntry entry);
    void write(QString fileName);
    void clear();
    MLogMetaData getMetaData() { return metaData; }
private:
    std::map<int, MDataEntry> entries;
    MLogMetaData metaData;
};

#endif // MDATALOG_H
