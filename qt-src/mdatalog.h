#ifndef MDATALOG_H
#define MDATALOG_H

#include <opencv2/video/video.hpp>
#include <QDebug>
#include <QString>

#include <map>
#include <vector>

#include "mlogmetadata.h"
struct VelocityResults
{
    std::vector<double> maxPositive;
    std::vector<double> avgPositive;
    std::vector<double> maxNegative;
    std::vector<double> avgNegative;
    std::vector<double> xTrackingLocationIndividual;
};

QDebug operator<<(QDebug debug, const VelocityResults &r);

class MDataEntry
{
public:
    MDataEntry(int frame, double time);
    MDataEntry();

    void addWallPart(double old, double topIMT, double bottomIMT,
                std::vector<cv::Point> &&topStrong,
                std::vector<cv::Point> &&topWeak,
                std::vector<cv::Point> &&bottomStrong,
                std::vector<cv::Point> &&bottomWeak);
    void addVelocityPart(VelocityResults &&vResults);

    int getFrameNumber() const { return frameNumber; }
    QString getCSV(double conversion);
    QString getVelocityCSV(double conversion, int index);
    static QString getHeader(QString units);
    static QString getVelocityHeader(QString units);
    static QString getEmptyEntry();
    static QString getEmptyVelocityEntry();
    template<typename T> static QString getString(T value);
    const std::vector<cv::Point>& getTopStrongLine() const { return topStrongLine; }
    const std::vector<cv::Point>& getTopWeakLine() const { return topWeakLine; }
    const std::vector<cv::Point>& getBottomStrongLine() const { return bottomStrongLine; }
    const std::vector<cv::Point>& getBottomWeakLine() const { return bottomWeakLine; }
    const VelocityResults& getVelocity() const { return velocity; }
private:
    QString getILTPixels(); // intima-intima, computed
    QString getILTUnits(double conversion);
    int frameNumber;
    double OLDPixels;       // media-media
    double topIMTPixels;    // top intima-media
    double bottomIMTPixels; // bottom intima-media
    double timeSeconds;
    std::vector<cv::Point> topStrongLine;
    std::vector<cv::Point> topWeakLine;
    std::vector<cv::Point> bottomStrongLine;
    std::vector<cv::Point> bottomWeakLine;
    VelocityResults velocity;
};

class MDataLog
{
public:
    MDataLog();
    void initialize(MLogMetaData data);
    void add(MDataEntry &&entry);
    void write(QString fileName);
    void clear();
    MLogMetaData getMetaData() { return metaData; }
    const MDataEntry* get(int frame) const;
private:
    void writeVelocity(QString fileName);
    std::map<int, MDataEntry> entries;
    MLogMetaData metaData;
};

#endif // MDATALOG_H
