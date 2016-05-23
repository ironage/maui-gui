import QtQuick 2.2
import "."

Rectangle {
    id: root
    color: "transparent"
    radius: 5
    property alias value: grip.value
    property color gripColor: "transparent"
    property real gripSize: 20
    property real increment: 0.1
    property bool enabled: true

//    Rectangle {
//        id: grip
//        property real value: 0.5
//        x: (value * parent.width) - width/2
//        anchors.verticalCenter: parent.verticalCenter
//        width: root.gripSize
//        height: width
//        radius: width/2
//        color: "red"
    MTriangle {
        id: grip
        property real value: 0.5
        x: (value * parent.width) - width/2
        //anchors.verticalCenter: parent.verticalCenter
        //anchors.left: parent.right
        //anchors.verticalCenter: parent.top
        triangle_width: root.gripSize
        triangle_height: triangle_width
        stroke_color: Style.ui_color_light_dblue
        fill_color: Style.ui_color_dark_dblue

        MouseArea {
            id: mouseArea
            enabled: root.enabled
            anchors.fill:  parent
            drag {
                target: grip
                axis: Drag.XAxis
                minimumX: -parent.width/2
                maximumX: root.width - parent.width/2
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
                value = (grip.x + grip.width/2) / grip.parent.width
            }
        }
    }
}
