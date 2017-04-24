import QtQuick 2.0
import QtMultimedia 5.5

import com.maui.custom 1.0  // MMediaPlayer.h

Item {
    id: m_root
    property alias source: cv_player.sourceFile
    property alias contentRect: output.contentRect
    property alias readSrcDir: cv_player.sourceDir
    property alias readSrcName: cv_player.sourceName
    property alias readSrcExtension: cv_player.sourceExtension
    property double progress: 0
    property alias frameIndex: cv_player.position
    property alias duration: cv_player.duration
    property double progress_min: 0.0
    property double progress_max: 1.0
    property alias playback_state: cv_player.playbackState
    property alias setupState: cv_player.setupState
    property alias video_width: cv_player.size.width
    property alias video_height: cv_player.size.height
    property alias roiMapping: cv_player.roi
    property alias velocityROIMapping: cv_player.velocityROI
    property alias diameterScale: cv_player.diameterScale
    property alias velocityScaleVertical: cv_player.velocityScaleVertical
    property alias velocityScaleHorizontal: cv_player.velocityScaleHorizontal
    property alias recomputeROIMode: cv_player.recomputeROIOnChange
    property alias topPoints: cv_player.initTopPoints
    property alias bottomPoints: cv_player.initBottomPoints
    property alias logData: cv_player.logInfo
    property alias videoRect: output.sourceRect
    property alias doProcessOutputVideo: cv_player.doProcessOutputVideo
    property alias conversionUnits: cv_player.conversionUnits
    property alias diameterConversion: cv_player.diameterConversion
    property alias outputDir: cv_player.outputDir
    property alias velocityConversionUnits: cv_player.velocityConversionUnits
    property alias velocityConversion: cv_player.velocityConversion
    property alias velocityTime: cv_player.velocityTime

    signal videoFinished(int state)
    signal outputProgress(int progress)
    signal videoLoaded(bool success, string fullName, string name, string extension, string dir)
    signal videoControlInfoChanged()

    function viewPointToVideoPoint(viewPoint) {
        return output.mapPointToSource(viewPoint)
    }
    function videoPointToViewPoint(videoPoint) {
        return output.mapPointToItem(videoPoint)
    }

    function seek(offset) {
        cv_player.seek(offset)
    }
    function play() {
        cv_player.play()
    }
    function continueProcessing() {
        cv_player.continueProcessing()
    }
    function pause() {
        cv_player.pause()
    }
    function forceROIRefresh() {
        cv_player.forceROIRefresh()
    }
    function setSetupState(state) {
        cv_player.setSetupState(state)
    }
    function setNewTopPoints(newPoints) {
        cv_player.setNewTopPoints(newPoints)
    }
    function setNewBottomPoints(newPoints) {
        cv_player.setNewBottomPoints(newPoints)
    }
    function addVideo(path) {
        cv_player.addVideoFile(path)
    }
    function removeVideo(path) {
        cv_player.removeVideoFile(path)
    }
    function changeToVideo(path) {
        cv_player.changeToVideoFile(path)
    }

    onProgress_minChanged: {
        cv_player.setStartFrame(progress_min * (cv_player.duration - 1));
    }
    onProgress_maxChanged: {
        cv_player.setEndFrame(progress_max * (cv_player.duration - 1));
    }

    Rectangle {
        anchors.fill: parent
        color: "black"
    }

    MCVPlayer {
        id: cv_player
        onPositionChanged: {
            console.log("position, duration: " + position + " " + duration)
            if (duration > 0) {
                m_root.progress = (position / duration)
            } else {
                m_root.progress = 0
            }
        }
        onVideoFinished: m_root.videoFinished(state)
        onOutputProgress: m_root.outputProgress(progress)
        onVideoLoaded: m_root.videoLoaded(success, fullName, name, extension, dir)
        onVideoControlInfoChanged: m_root.videoControlInfoChanged()
    }

    VideoOutput {
        id: output
        anchors.fill: parent
        visible: cv_player.sourceFile !== ""
        source: cv_player
        onWidthChanged: {
            setSize()
        }
        onHeightChanged: {
            setSize()
        }
        function setSize() {
            cv_player.size = Qt.size(output.width, output.height)
        }
    }

    focus: true
}
