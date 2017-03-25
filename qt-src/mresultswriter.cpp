#include "mresultswriter.h"
#include "mdatalog.h"

#include <QDateTime>

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
    return metaData.getHeader();
}

QString MDiameterWriter::getEmptyEntry() const
{
    return QString(",,,,,,,,,");
}

QString MDiameterWriter::getEntry(const MDataEntry &entry) const
{
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
