import QtQuick 2.5
import QtQuick.Layouts 1.1
import "."

Item {
    id: m_root
    width: 10
    height: 15
    property double progress: 0
    property alias start_percent: m_start_pin.value
    property alias end_percent: m_end_pin.value
    property bool movable: true

    signal setProgress(double percent)

    onProgressChanged: {
        if (movable) {
            if (progress < start_percent) {
                start_percent = progress
            }
            if (progress > end_percent) {
                end_percent = progress
            }
        }
    }

    Rectangle {
        width: m_root.width
        height: m_root.height
        color: Style.ui_color_light_grey
        MouseArea {
            anchors.fill: parent
            onPositionChanged:  {
                if (containsPress)
                    updatePosition()
            }
            onReleased: {
                updatePosition()
            }
            function updatePosition() {
                setProgress(mouseX / m_root.width)
            }
        }
        Rectangle {
            width: m_root.progress * m_root.width
            height: m_root.height
            color: Style.ui_color_light_green
        }
        Rectangle {
            color: Style.ui_color_dark_grey
            height: parent.height
            anchors.left: parent.left
            anchors.right: m_start_pin.horizontalCenter
        }
        Rectangle {
            color: Style.ui_color_dark_grey
            height: parent.height
            anchors.left: m_end_pin.horizontalCenter
            anchors.right: parent.right
        }
        MPin {
            id: m_start_pin
            value: 0.0
        }
        MPin {
            id: m_end_pin
            value: 1.0
        }
    }
}
