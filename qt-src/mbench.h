#ifndef MBENCH_H
#define MBENCH_H

#include <QDateTime>
#include <QDebug>
#include <QString>

struct BenchScope
{
    BenchScope(QString name)
        :name(name)
    {
        start = QDateTime::currentMSecsSinceEpoch();
    }
    ~BenchScope()
    {
        qDebug() << "Benchmark: " << name << " took: " << (QDateTime::currentMSecsSinceEpoch() - start) << " ms";

    }
    QString name;
    qint64 start;
};


#endif // MBENCH_H
