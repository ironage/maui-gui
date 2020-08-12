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
 * @file BetterVideoCapture.cpp
 * @brief A wrapper for either cv::VideoCapture for desktop or CVCaptureAndroid for Android
 * @author Ayberk Özgür
 * @version 1.0
 * @date 2014-10-02
 */

#include "mvideocapture.h"
#include "mbench.h"
#include <QDebug>

MVideoCapture::MVideoCapture()
    : capture()
    , cachedFrame()
    , imageMode(false)
    , cachedFrameIndex(-2)
    , numTotalFrames(-1)
{
}

MVideoCapture::~MVideoCapture()
{
    capture.release();
}

bool MVideoCapture::open(int device)
{
    return capture.open(device);
}

bool MVideoCapture::open(std::string filename)
{
    // attempt to read an image, if this doesn't succeed, let the VideoCapture have at it
    cachedFrame = cv::imread(filename);
    if (cachedFrame.data != nullptr) {
        imageMode = true;
        cachedFrameIndex = 0;
        qDebug() << "opening in image mode";
        return true;
    }
    qDebug() << "opening in video mode";
    imageMode = false;
    return capture.open(filename);
}

int MVideoCapture::getCurrentFrameIndex()
{
    int frameIndex = 0;
    if (imageMode) {
        frameIndex = cachedFrameIndex;
    }
    frameIndex = int(capture.get(CV_CAP_PROP_POS_FRAMES));
    assert(cachedFrameIndex == frameIndex);
    return frameIndex;
}

int MVideoCapture::getNumTotalFrames()
{
    if (numTotalFrames < 0) {
        if (imageMode) {
             numTotalFrames = 1;
        } else {
            numTotalFrames = int(capture.get(CV_CAP_PROP_FRAME_COUNT));
        }
        qDebug() << "getNumTotalFrames returns: " << numTotalFrames;
    }
    return numTotalFrames;
}

int MVideoCapture::getFrameWidth()
{
    if (imageMode) {
        return cachedFrame.size().width;
    }
    return int(capture.get(CV_CAP_PROP_FRAME_WIDTH));
}

int MVideoCapture::getFrameHeight()
{
    if (imageMode) {
        return cachedFrame.size().height;
    }
    return int(capture.get(CV_CAP_PROP_FRAME_HEIGHT));
}

double MVideoCapture::getFrameRate()
{
    if (imageMode) {
        return 1;
    }
    return capture.get(CV_CAP_PROP_FPS);
}

unsigned char *MVideoCapture::getFrameData(int frameIndex)
{
    if (frameIndex < getNumTotalFrames()) {
        if (frameIndex == cachedFrameIndex) {
            return cachedFrame.ptr();
        } else {
            assert(!imageMode);
            bool didSet = true;
            if (frameIndex != cachedFrameIndex + 1){
                didSet = capture.set(CV_CAP_PROP_POS_FRAMES, double(frameIndex));
            }
            if (didSet && capture.read(cachedFrame)) {
                cachedFrameIndex = frameIndex;
                return cachedFrame.ptr();
            }
        }
    }
    qDebug() << "getFrameData failed for frame " << frameIndex;
    return nullptr;
}

bool MVideoCapture::isOpened() const
{
    if (imageMode) {
        return true;
    }
    return capture.isOpened();
}

bool MVideoCapture::isImage() const
{
    return imageMode;
}
