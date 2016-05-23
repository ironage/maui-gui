import QtQuick 2.2
import "."

Rectangle {
    id: m_root
    anchors.fill: parent
    color: "transparent"
    property alias value: slider.value
    property alias lineWidth: line.width
    property alias gripSize: slider.gripSize

    Rectangle {
        id: line
        anchors { top: parent.top; bottom: parent.bottom }
        x: parent.value * parent.width - (width / 2)
        width: 4
        color: "#14aaff"
    }

    // topgrip
    MScaleHandle {
        id: slider
        increment: 0.0
        anchors {
            top: parent.top
            topMargin: (gripSize / 2) + 5
            left: parent.left
            right: parent.right
        }
        onValueChanged: slider2.value = slider.value
    }

    // bottomgrip
    MScaleHandle {
        id: slider2
        increment: 0.0
        anchors {
            bottom: parent.bottom
            bottomMargin: (gripSize / 2) + 5
            left: parent.left
            right: parent.right
        }
        onValueChanged: slider.value = slider2.value
    }
}
