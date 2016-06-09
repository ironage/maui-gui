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
        if (m_player.playbackState === MediaPlayer.PlayingState) {
            m_player.pause()
        }
        m_player.seek(progress_min * m_player.duration)
    }
    onProgress_maxChanged: {
        if (m_player.playbackState === MediaPlayer.PlayingState) {
            m_player.pause()
        }
        m_player.seek(progress_max * m_player.duration)
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
                if (position < duration * progress_min) {
                    seek(duration * progress_min)
                }
                if (position > duration * progress_max) {
                    seek(duration * progress_max)
                    if (playbackState === MediaPlayer.PlayingState) {
                        pause()
                    }
                }
            } else {
                m_root.progress = 0
            }
        }
    }

//    MMediaPlayer {
//        id: m_player
//        property bool show_first_frame: false
//        onPositionChanged: {
//            if (duration > 0) {
//                m_root.progress = (position / duration)
//                if (position < duration * progress_min) {
//                    seek(duration * progress_min)
//                }
//                if (position > duration * progress_max) {
//                    seek(duration * progress_max)
//                    if (playbackState === MediaPlayer.PlayingState) {
//                        pause()
//                    }
//                }
//            } else {
//                m_root.progress = 0
//            }
//        }
////        onBufferProgressChanged: {
////            if (show_first_frame && bufferProgress >= 1.0) {
////                show_first_frame = false
////                play()
////                seek(1)
////                pause()
////            }
////        }
////        onDurationChanged: {
////            show_first_frame = true
////        }
//    }

    VideoOutput {
        id: output
        anchors.fill: parent
        source: cv_player
        //visible: false
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
    Keys.onSpacePressed: cv_player.playbackState === MediaPlayer.PlayingState ? cv_player.pause() : cv_player.play()
//    Keys.onLeftPressed: m_player.seek(m_player.position - 5000)
//    Keys.onRightPressed: m_player.seek(m_player.position + 5000)

}
