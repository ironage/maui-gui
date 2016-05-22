import QtQuick 2.0
import QtMultimedia 5.5

Item {
    id: m_root
    property alias source: m_player.source
    property double progress: 0
    property alias duration: m_player.duration
    property double progress_min: 0.0
    property double progress_max: 1.0
    property alias playback_state: m_player.playbackState


    function seek(offset) {
        m_player.seek(offset)
    }
    function play() {
        m_player.play()
    }
    function pause() {
        m_player.pause()
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

    MediaPlayer {
        id: m_player
        property bool show_first_frame: false
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
        onBufferProgressChanged: {
            if (show_first_frame && bufferProgress >= 1.0) {
                show_first_frame = false
                play()
                seek(1)
                pause()
            }
        }

        onDurationChanged: {
            show_first_frame = true
        }
    }

    VideoOutput {
        anchors.fill: parent
        source: m_player
    }

    focus: true
    Keys.onSpacePressed: m_player.playbackState == MediaPlayer.PlayingState ? m_player.pause() : m_player.play()
    Keys.onLeftPressed: m_player.seek(m_player.position - 5000)
    Keys.onRightPressed: m_player.seek(m_player.position + 5000)

}
