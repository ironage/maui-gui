import QtQuick 2.5
import "."
import com.maui.custom 1.0 // MPoint

Rectangle {
    id: m_root
    anchors.fill: parent
    color: "transparent"
    property real alpha: 1
    property int cornerWidth: 14
    property color pointColor: "white"

    property var pointList

    Rectangle {
        id: point1
        visible: pointList.length >= 1
        property int xOffset: -width/2
        property int yOffset: -height/2
        x: (pointList !== null && pointList.length >= 1) ? pointList[0].x + xOffset : 0
        y: (pointList !== null && pointList.length >= 1) ? pointList[0].y + yOffset : 0

        width: cornerWidth
        height: width
        opacity: alpha
        color: pointColor
        radius: width/2
        MouseArea {
            enabled: m_root.enabled
            anchors.fill: parent
            hoverEnabled: true
            drag {
                target: point1
                axis: Drag.XAndYAxis
                minimumX: point1.xOffset
                maximumX: m_root.width + point1.xOffset
                minimumY: point1.yOffset
                maximumY: m_root.height + point1.yOffset
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
                if (pointList.length >= 1) {
                    pointList[0].x = (point1.x - point1.xOffset)
                    pointList[0].y = (point1.y - point1.yOffset)
                }
            }
        }
    }
    Rectangle {
        id: point2
        visible: (pointList.length >= 2)
        property int xOffset: -width/2
        property int yOffset: -height/2
        x: (pointList.length >= 2) ? pointList[1].x + xOffset : 0
        y: (pointList.length >= 2) ? pointList[1].y + yOffset : 0
        width: cornerWidth
        height: width
        opacity: alpha
        color: pointColor
        radius: width/2
        MouseArea {
            enabled: m_root.enabled
            anchors.fill: parent
            hoverEnabled: true
            drag {
                target: point2
                axis: Drag.XAndYAxis
                minimumX: point2.xOffset
                maximumX: m_root.width + point2.xOffset
                minimumY: point2.yOffset
                maximumY: m_root.height + point2.yOffset
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
                if (pointList.length >= 2) {
                    pointList[1].x = (point2.x - point2.xOffset)
                    pointList[1].y = (point2.y - point2.yOffset)
                }
            }
        }
    }
    Rectangle {
        id: point3
        property int xOffset: -width/2
        property int yOffset: -height/2
        visible: (pointList.length >= 3)
        x: (pointList.length >= 3) ? pointList[2].x + xOffset : 0
        y: (pointList.length >= 3) ? pointList[2].y + yOffset : 0
        width: cornerWidth
        height: width
        opacity: alpha
        color: pointColor
        radius: width/2
        MouseArea {
            enabled: m_root.enabled
            anchors.fill: parent
            hoverEnabled: true
            drag {
                target: point3
                axis: Drag.XAndYAxis
                minimumX: point3.xOffset
                maximumX: m_root.width + point3.xOffset
                minimumY: point3.yOffset
                maximumY: m_root.height + point3.yOffset
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
                if (pointList.length >= 3) {
                    pointList[2].x = (point3.x - point3.xOffset)
                    pointList[2].y = (point3.y - point3.yOffset)
                }
            }
        }
    }
    MLine {
        visible: (pointList.length >= 2)
        x1: point1.x - point1.xOffset
        x2: point2.x - point2.xOffset
        y1: point1.y - point1.yOffset
        y2: point2.y - point2.yOffset
    }
    MLine {
        visible: (pointList.length >= 3)
        x1: point2.x - point2.xOffset
        x2: point3.x - point3.xOffset
        y1: point2.y - point2.yOffset
        y2: point3.y - point3.yOffset
    }
}
