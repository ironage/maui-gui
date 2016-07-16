#ifndef MDATALOG_H
#define MDATALOG_H

#include <QString>

#include <vector>

#include "mlogmetadata.h"

class MDataEntry
{
public:
    MDataEntry(int frame, double old, double topIMT, double bottomIMT, double time);

    QString getCSV(double conversion);
    static QString getHeader(QString units);
    static QString getEmptyEntry();
private:
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
private:
    std::vector<MDataEntry> entries;
    MLogMetaData metaData;
};

#endif // MDATALOG_H
