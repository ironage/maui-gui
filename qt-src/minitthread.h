#ifndef MINITTHREAD_H
#define MINITTHREAD_H

#include <QObject>
#include <QThread>

class MInitTask : public QObject
{
    Q_OBJECT
public:
    MInitTask(QObject *parent = 0) : QObject(parent) {}
    enum InitStats {
        SUCCESS,
        FAILURE_IN_AUTO_INIT,
        FAILURE_IN_MAUI_INIT,
        FAILURE_IN_APPLICATION_INIT
    };
public slots:
    void doWork();
signals:
    void done(MInitTask::InitStats success);
};

class MInitThread : public QObject
{
    Q_OBJECT
public:
    explicit MInitThread(QObject *parent = 0);
    virtual ~MInitThread();
    void init();
signals:
    void initFinished(MInitTask::InitStats status);
private:
    QThread workerThread;
    MInitTask* task = NULL;
};

Q_DECLARE_METATYPE(MInitTask::InitStats)

#endif // MINITTHREAD_H
