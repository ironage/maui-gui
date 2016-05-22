import QtQuick 2.0
import "."

Rectangle {
    id: grip
    property real value: 0.5

    property int width_bound: parent.width
    property bool enabled: true

    x: (value * parent.width) - width/2
    anchors.bottom: parent.top
    width: 18
    height: width
    radius: width/2
    color: Style.ui_color_dark_dblue

    MouseArea {
        id: mouseArea
        enabled: grip.enabled
        anchors.fill:  parent
        drag {
            target: grip
            axis: Drag.XAxis
            minimumX: -parent.width/2
            maximumX: width_bound - parent.width/2
        }
        onPositionChanged:  {
            if (drag.active)
                updatePosition()
        }
        onReleased: {
            updatePosition()
        }
        function updatePosition() {
            grip.value = (grip.x + grip.width/2) / width_bound
        }
    }
    Rectangle {
        id: m_vbar
        width: 2
        height: grip.parent.height
        color: parent.color
        anchors.top: grip.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }
}
