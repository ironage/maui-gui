import QtQuick 2.0
import QtMultimedia 5.5
import QtGraphicalEffects 1.0

import com.maui.custom 1.0  // MMediaPlayer.h

Item {
    id: m_root
    property alias source: cv_player.sourceFile
    property double progress: 0
    property alias duration: cv_player.duration
    property double progress_min: 0.0
    property double progress_max: 1.0
    property alias playback_state: cv_player.playbackState
    property alias video_width: cv_player.size.width
    property alias video_height: cv_player.size.height
    property alias roi: cv_player.roi
    signal videoRectChanged()

    function viewPointToVideoPoint(viewPoint) {
        return output.mapPointToSource(viewPoint)
    }

    function seek(offset) {
        cv_player.seek(offset)
    }
    function play() {
        cv_player.play()
    }
    function pause() {
        cv_player.pause()
    }

    onProgress_minChanged: {
        cv_player.setStartFrame(progress_min * cv_player.duration);
    }
    onProgress_maxChanged: {
        cv_player.setEndFrame(progress_max * cv_player.duration);
    }

    Rectangle {
        anchors.fill: parent
        color: "black"
    }

    MCVPlayer {
        id: cv_player

        onPositionChanged: {
            if (duration > 0) {
                m_root.progress = (position / duration)
            } else {
                m_root.progress = 0
            }
        }
    }

    VideoOutput {
        id: output
        anchors.fill: parent
        source: cv_player
        onSourceRectChanged: m_root.videoRectChanged()
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
