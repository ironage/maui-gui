import QtQuick 2.5
import "."

Rectangle {
    id: m_root
    anchors.fill: parent
    color: "transparent"
    property real alpha: 1
    property int cornerWidth: 13
    property color pointColor: "white"

    property int p1X: 0
    property int p1Y: 0
    property int p2X: 0
    property int p2Y: 0
    property int p3X: 0
    property int p3Y: 0

    Rectangle {
        id: point1
        property int xOffset: -width/2
        property int yOffset: -height/2
        x: p1X + xOffset
        y: p1Y + yOffset
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
                p1X = (point1.x - point1.xOffset)
                p1Y = (point1.y - point1.yOffset)
            }
        }
    }
    Rectangle {
        id: point2
        property int xOffset: -width/2
        property int yOffset: -height/2
        x: p2X + xOffset
        y: p2Y + yOffset
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
                p2X = (point2.x - point2.xOffset)
                p2Y = (point2.y - point2.yOffset)
            }
        }
    }
    Rectangle {
        id: point3
        property int xOffset: -width/2
        property int yOffset: -height/2
        x: p3X + xOffset
        y: p3Y + yOffset
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
                p3X = (point3.x - point3.xOffset)
                p3Y = (point3.y - point3.yOffset)
            }
        }
    }
    MLine {
        x1: point1.x - point1.xOffset
        x2: point2.x - point2.xOffset
        y1: point1.y - point1.yOffset
        y2: point2.y - point2.yOffset
    }
    MLine {
        x1: point2.x - point2.xOffset
        x2: point3.x - point3.xOffset
        y1: point2.y - point2.yOffset
        y2: point3.y - point3.yOffset
    }
}
