import QtQuick 2.0
import QtMultimedia 5.5

Item {
    id: m_root
    property alias source: m_player.source
    property double progress: 0

    Rectangle {
        anchors.fill: parent
        color: "black"
    }

    MediaPlayer {
        id: m_player
        onPositionChanged: {
            console.log("position changed: " + position)
            if (duration > 0) {
                m_root.progress = (position / duration)
            } else {
                m_root.progress = 0
            }
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
