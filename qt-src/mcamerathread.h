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
 * @file CameraThread.h
 * @brief Listens to the camera in a separate thread
 * @author Ayberk Özgür
 * @version 1.0
 * @date 2014-09-23
 */

#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include<QDebug>
#include<QThread>
#include<QObject>
#include<QElapsedTimer>
#include<QVideoFrame>

#include<opencv2/highgui/highgui.hpp>
//#include<opencv2/videoio/videoio_c.h>
#include<opencv2/video/video.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/imgproc/types_c.h>

#include<vector>

#include"mvideocapture.h"

/**
 * @brief Object that contains the camera loop and its parameters
 */
class CameraTask : public QObject{
Q_OBJECT

public:

    /**
     * @brief Creates a new camera access task
     *
     * @param camera Camera object to get data from
     * @param videoFrame Place to draw the camera image, pass NULL to not draw camera image to a QVideoFrame
     * @param cvImageBuf Place to export the camera image, pass NULL to not export the camera image
     * @param width Width of the camera image
     * @param height Height of the camera image
     */
    CameraTask(MVideoCapture* camera, QVideoFrame* videoFrame, unsigned char* cvImageBuf, int width, int height);

    /**
     * @brief Destroys this camera access task, does not touch the camera or the videoFrame
     */
    virtual ~CameraTask();

    /**
     * @brief Asks for the main loop to stop
     */
    void stop();

private:
    enum PlayState {
        Playing,
        Paused,
        Seeking
    };

#if defined(SHOW_FRAMERATE) && !defined(ANDROID) //Android camera has its own FPS debug info
    const float CAM_FPS_RATE = 0.9f;            ///< Rate of using the older FPS estimates
    const int CAM_FPS_PRINT_PERIOD = 500;       ///< Period of printing the FPS estimate, in milliseconds
#endif
    PlayState curPlayState;
    int width;                                  ///< Width of the camera image
    int height;                                 ///< Height of the camera image
    MVideoCapture* camera;                 ///< The camera to get data from
    bool running = false;                       ///< Whether the worker thread is running
    QVideoFrame* videoFrame;                    ///< Place to draw camera image to
    unsigned char* cvImageBuf;                  ///< Place to export camera image to
    int curFrame;
    int frameToSeekTo;
    int startFrame;
    int endFrame;
    /**
     * @brief Converts the semi-planar UV plane to a planar UV plane
     *
     * @param srcptr Beginning address of the interleaved UV plane
     * @param dstptr Beginning address of the U plane
     * @param stride Size of the U or V planes, i.e half the size of the interleaved UV plane
     */
    void convertUVsp2UVp(unsigned char* __restrict srcptr, unsigned char* __restrict dstptr, int stride);

public slots:
    void doWork();
    void play();
    void pause();
    void seek(int frameNumber);
    void setStartFrame(int frameNumber);
    void setEndFrame(int frameNumber);
signals:
    void imageReady(int);
};

class MCameraThread : public QObject{
Q_OBJECT

public:

    /**
     * @brief Creates a new camera controller
     *
     * @param camera Camera object to get data from
     * @param videoFrame Place to draw the camera image, pass NULL to not draw camera image to a QVideoFrame
     * @param cvImageBuf Place to export the camera image, pass NULL to not export the camera image
     * @param width Width of the camera image
     * @param height Height of the camera image
     */
    MCameraThread(MVideoCapture* camera, QVideoFrame* videoFrame, unsigned char* cvImageBuf, int width, int height);
    virtual ~MCameraThread();
    void start();
    void stop();
    void doPlay();
    void doPause();
    void doSeek(int frameNumber);
    void doSetStartFrame(int frameNumber);
    void doSetEndFrame(int frameNumber);
private:
    QThread workerThread;
    CameraTask* task = NULL;
signals:
    void imageReady(int frameNumber);
    void play();
    void pause();
    void seek(int frameNumber);
    void setStartFrame(int frameNumber);
    void setEndFrame(int frameNumber);
};

#endif /* CAMERATHREAD_H */

