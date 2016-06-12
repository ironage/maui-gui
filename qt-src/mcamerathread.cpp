/*
 * Copyright (C) 2014 EPFL
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

/**
 * @file CameraThread.cpp
 * @brief Listens to the camera in a separate thread
 * @author Ayberk Özgür
 * @version 1.0
 * @date 2014-09-23
 */
#define SHOW_FRAMERATE 1


#include <QCoreApplication>

#include"mcamerathread.h"

CameraTask::CameraTask(MVideoCapture* camera, QVideoFrame* videoFrame,
                       unsigned char* cvImageBuf, int width, int height)
    : running(true), camera(camera), videoFrame(videoFrame),cvImageBuf(cvImageBuf),
    width(width), height(height), curPlayState(Paused), curFrame(-1), frameToSeekTo(-1),
    startFrame(0), endFrame(0)
{
}

CameraTask::~CameraTask()
{
    qDebug() << "CameraTask destructed";
    //Leave camera and videoFrame alone, they will be destroyed elsewhere
}

void CameraTask::stop()
{
    running = false;
}

void CameraTask::convertUVsp2UVp(unsigned char* __restrict srcptr, unsigned char* __restrict dstptr, int stride)
{
    for(int i=0;i<stride;i++){
        dstptr[i]           = srcptr[i*2];
        dstptr[i + stride]  = srcptr[i*2 + 1];
    }
}

void CameraTask::doWork()
{

#if defined(SHOW_FRAMERATE) && !defined(ANDROID)
    QElapsedTimer timer;
    float fps = 0.0f;
    int millisElapsed = 0;
    int millis;
    timer.start();
#endif

    if(videoFrame)
        videoFrame->map(QAbstractVideoBuffer::ReadOnly);

#ifndef ANDROID //Assuming desktop, RGB camera image and RGBA QVideoFrame
    cv::Mat screenImage;
    if(videoFrame)
        screenImage = cv::Mat(height,width,CV_8UC4,videoFrame->bits());
#endif

    while(running && videoFrame != NULL && camera != NULL) {
         QCoreApplication::processEvents();

        switch (curPlayState) {
        case PlayState::Paused:
            QThread::msleep(10);
            continue; // loop
        case PlayState::Seeking:
            camera->setProperty(CV_CAP_PROP_POS_FRAMES, frameToSeekTo);
            curPlayState = PlayState::Paused;
            // fall through
        case PlayState::Playing:
        default:
            break; // switch
        }

        curFrame = camera->getProperty(CV_CAP_PROP_POS_FRAMES);
        if (curPlayState == PlayState::Playing && curFrame > endFrame) {
            curPlayState = PlayState::Paused;
            continue;
        }

        if(!camera->grabFrame())
            continue;
        unsigned char* cameraFrame = camera->retrieveFrame();

        //Get camera image into screen frame buffer
        if(videoFrame){

#ifdef ANDROID //Assume YUV420sp camera image and YUV420p QVideoFrame

            //Copy over Y channel
            memcpy(videoFrame->bits(),cameraFrame,height*width);

            //Convert semiplanar UV to planar UV
            convertUVsp2UVp(cameraFrame + height*width, videoFrame->bits() + height*width, height/2*width/2);

#else //Assuming desktop, RGB camera image and RGBA QVideoFrame
            cv::Mat tempMat(height,width,CV_8UC3,cameraFrame);
            cv::cvtColor(tempMat,screenImage,cv::COLOR_RGB2RGBA);
#endif

        }

        //Export camera image
        if(cvImageBuf){

#ifdef ANDROID //Assume YUV420sp camera image
            memcpy(cvImageBuf,cameraFrame,height*width*3/2);
#else //Assuming desktop, RGB camera image
            memcpy(cvImageBuf,cameraFrame,height*width*3);
#endif
        }

        emit imageReady(curFrame);

#if defined(SHOW_FRAMERATE) && !defined(ANDROID)
        millis = (int)timer.restart();
        millisElapsed += millis;
        fps = CAM_FPS_RATE*fps + (1.0f - CAM_FPS_RATE)*(1000.0f/millis);
        if(millisElapsed >= CAM_FPS_PRINT_PERIOD){
            qDebug("Camera is running at %f FPS",fps);
            millisElapsed = 0;
        }
#endif
    }
}

void CameraTask::play()
{
    curPlayState = PlayState::Playing;
}

void CameraTask::pause()
{
    curPlayState = PlayState::Paused;
}

void CameraTask::seek(int frameNumber)
{
    if (frameToSeekTo != frameNumber) {
        frameToSeekTo = frameNumber;
        curPlayState = PlayState::Seeking;
    } else {
        //curPlayState = PlayState::Paused;
    }
}

void CameraTask::setStartFrame(int frameNumber)
{
    startFrame = frameNumber;
    seek(frameNumber);
}

void CameraTask::setEndFrame(int frameNumber)
{
    endFrame = frameNumber;
    seek(frameNumber);
}






MCameraThread::MCameraThread(MVideoCapture* camera, QVideoFrame* videoFrame, unsigned char* cvImageBuf, int width, int height)
{
    task = new CameraTask(camera,videoFrame,cvImageBuf,width,height);
    task->moveToThread(&workerThread);
    connect(&workerThread, SIGNAL(started()), task, SLOT(doWork()));
    connect(task, SIGNAL(imageReady(int)), this, SIGNAL(imageReady(int)));
    connect(this, SIGNAL(play()), task, SLOT(play()), Qt::QueuedConnection);
    connect(this, SIGNAL(pause()), task, SLOT(pause()), Qt::QueuedConnection);
    connect(this, SIGNAL(seek(int)), task, SLOT(seek(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setStartFrame(int)), task, SLOT(setStartFrame(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(setEndFrame(int)), task, SLOT(setEndFrame(int)), Qt::QueuedConnection);
}

MCameraThread::~MCameraThread()
{
    stop();
    delete task;
}

void MCameraThread::start()
{
    workerThread.start();
}

void MCameraThread::stop()
{
    if(task != NULL)
        task->stop();
    workerThread.wait(1000);
    workerThread.quit();
    workerThread.wait();
}

void MCameraThread::doPlay()
{
    emit play();
}
void MCameraThread::doPause()
{
    emit pause();
}

void MCameraThread::doSeek(int frameNumber)
{
    emit seek(frameNumber);
}

void MCameraThread::doSetStartFrame(int frameNumber)
{
    emit setStartFrame(frameNumber);
}

void MCameraThread::doSetEndFrame(int frameNumber)
{
    emit setEndFrame(frameNumber);
}
