import QtQuick 2.0
import "."

Rectangle {
    id: grip
    property real value: 0.5

    property int width_bound: parent.width
    property bool enabled: true
    property color selected_color: Style.ui_color_light_red
    property color idle_color: Style.ui_color_dark_dblue
    property color disabled_color: Style.ui_color_dark_grey
    property string description: ""
    property alias minimumX: dragArea.minimumX
    property alias maximumX: dragArea.maximumX

    x: (value * parent.width) - width/2
    anchors.bottom: parent.top
    width: 18
    height: width
    radius: width/2
    color: enabled ? (dragArea.active ? selected_color : idle_color) : disabled_color

    MouseArea {
        id: mouseArea
        enabled: grip.enabled
        anchors.fill:  parent
        drag {
            id: dragArea
            target: grip
            axis: Drag.XAxis
            minimumX: -parent.width/2
            maximumX: width_bound - parent.width/2
            threshold: Style.drag_threshold
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
        width: 3
        height: grip.parent.height
        color: parent.color
        anchors.top: grip.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }
    MText {
        text: grip.description
        anchors.horizontalCenter: m_vbar.horizontalCenter
        y: m_vbar.y + m_vbar.height + 5
        font.pixelSize: 13
        color: grip.color
        styleColor: "white"
        visible: parent.enabled
    }
}
