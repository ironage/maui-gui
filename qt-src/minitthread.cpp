#include "minitthread.h"

#include <QDebug>

// Disable warnings for unreferenced formal parameter with visual
// studio on the matlab generated header files.
#pragma warning(push)
#pragma warning( disable : 4100 )
#include "MAUIVelocityDllWithAutoInit.h"   // custom generated header (with lib) from matlab code
#pragma warning(pop)

MInitThread::MInitThread(QObject *parent) : QObject(parent)
{
    task = new MInitTask();
    task->moveToThread(&workerThread);
    connect(&workerThread, SIGNAL(started()), task, SLOT(doWork()));
    connect(task, SIGNAL(done(MInitTask::InitStats)), this, SIGNAL(initFinished(MInitTask::InitStats)));
}

MInitThread::~MInitThread()
{
    workerThread.quit();
    workerThread.wait();
    if (task) {
        delete task;
    }
    MAUIVelocityDllWithAutoInitTerminate();
    mclTerminateApplication();  // can only be called once per application

    qDebug() << "Cleaned up init thread successfully";
}

void MInitThread::init()
{
    workerThread.start();
}

void MInitTask::doWork()
{
    qDebug() << "Starting initialization of matlab.";

    const char *pStrings[]={"-nojvm","-nojit"};
    // Initialize the MATLAB Compiler Runtime global state
    if (!mclInitializeApplication(pStrings,2))
    {
        qDebug() << "Could not initialize the application properly.";
        emit done(InitStats::FAILURE_IN_APPLICATION_INIT);
        return;
    }

    qDebug() << "Initializing autoMaui matlab library";
    if (!MAUIVelocityDllWithAutoInitInitialize()) {
        qDebug() << "Could not initialize the autoMaui matlab library.";
        emit done(InitStats::FAILURE_IN_MAUI_INIT);
        return;
    }
    qDebug() << "Done initialization stage.";
    emit done(InitStats::SUCCESS);
}
