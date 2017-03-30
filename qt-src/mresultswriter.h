#ifndef MRESULTSWRITER_H
#define MRESULTSWRITER_H

#include <memory>
#include <QFile>
#include <QString>

#include "mlogmetadata.h"
#include "mdatalog.h"

class MResultsWriter {
public:
    MResultsWriter(QString name, MLogMetaData &attachedMetaData);
    virtual std::unique_ptr<QFile> open();
    virtual QString getHeader() const = 0;
    virtual std::vector<QString> getMetaDataHeader() const = 0;
    virtual QString getEmptyEntry() const = 0;
    virtual QString getEntry(const MDataEntry &entry, int index) const = 0;
    template<typename T> QString getString(T value) const;
protected:
    QString filename;
    MLogMetaData& metaData;
};

class MDiameterWriter : public MResultsWriter {
public:
    MDiameterWriter(QString name, MLogMetaData &attachedMetaData);
    QString getHeader() const override;
    std::vector<QString> getMetaDataHeader() const override;
    QString getEmptyEntry() const override;
    QString getEntry(const MDataEntry &entry, int index) const override;
private:
    QString getILTPixels(); // intima-intima, computed
    QString getILTUnits(double conversion);
    double conversion;
};

class MVelocityWriter : public MResultsWriter {
public:
    MVelocityWriter(QString name, MLogMetaData &attachedMetaData);
    QString getHeader() const override;
    std::vector<QString> getMetaDataHeader() const override;
    QString getEmptyEntry() const override;
    QString getEntry(const MDataEntry &entry, int index) const override;
    double getVelocityConversion() const { return conversion; }
private:
    double conversion;
};

class MCombinedWriter : public MResultsWriter {
public:
    MCombinedWriter(QString name, MLogMetaData &attachedMetaData);
    QString getHeader() const override;
    std::vector<QString> getMetaDataHeader() const override;
    QString getEmptyEntry() const override;
    QString getEntry(const MDataEntry &entry, int index) const override;
private:
    QString getVelocityEmptyEntry() const;
    QString getVelocityEntry(const MDataEntry &entry, int index) const;
    MVelocityWriter vWriter;
    MDiameterWriter dWriter;
};

#endif // MRESULTSWRITER_H
