import QtQuick 2.5
import "."
import com.maui.custom 1.0 // MPoint

Rectangle {
    id: m_root
    anchors.fill: parent
    color: "transparent"
    property real alpha: 1
    property int cornerWidth: 18
    property color idlePointColor: Style.ui_color_silver
    property color hoverPointColor: Style.ui_color_light_turquoise
    property color pointColor: (ptMA.containsMouse === true
                                || pt2MA.containsMouse === true
                                || pt3MA.containsMouse === true
                                /*|| pt4MA.containsMouse === true
                                || pt5MA.containsMouse === true*/) ? hoverPointColor : idlePointColor

    property var pointList
    signal manualChange()

    Rectangle {
        id: pt
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
            id: ptMA
            enabled: m_root.enabled
            anchors.fill: parent
            hoverEnabled: true
            drag {
                target: pt
                axis: Drag.XAndYAxis
                minimumX: pt.xOffset
                maximumX: m_root.width + pt.xOffset
                minimumY: pt.yOffset
                maximumY: m_root.height + pt.yOffset
                threshold: Style.drag_threshold
            }
            onPositionChanged:  {
                if (drag.active)
                    updatePosition()
            }
            onReleased: {
                updatePosition()
                m_root.manualChange()
            }
            function updatePosition() {
                if (pointList.length >= 1) {
                    pointList[0].x = (pt.x - pt.xOffset)
                    pointList[0].y = (pt.y - pt.yOffset)
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
            id: pt2MA
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
                m_root.manualChange()
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
            id: pt3MA
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
                m_root.manualChange()
            }
            function updatePosition() {
                if (pointList.length >= 3) {
                    pointList[2].x = (point3.x - point3.xOffset)
                    pointList[2].y = (point3.y - point3.yOffset)
                }
            }
        }
    }
//    Rectangle {
//        id: point4
//        property int xOffset: -width/2
//        property int yOffset: -height/2
//        visible: (pointList.length >= 4)
//        x: (pointList.length >= 4) ? pointList[3].x + xOffset : 0
//        y: (pointList.length >= 4) ? pointList[3].y + yOffset : 0
//        width: cornerWidth
//        height: width
//        opacity: alpha
//        color: pointColor
//        radius: width/2
//        MouseArea {
//            id: pt4MA
//            enabled: m_root.enabled
//            anchors.fill: parent
//            hoverEnabled: true
//            drag {
//                target: point4
//                axis: Drag.XAndYAxis
//                minimumX: point4.xOffset
//                maximumX: m_root.width + point4.xOffset
//                minimumY: point4.yOffset
//                maximumY: m_root.height + point4.yOffset
//                threshold: Style.drag_threshold
//            }
//            onPositionChanged:  {
//                if (drag.active)
//                    updatePosition()
//            }
//            onReleased: {
//                updatePosition()
//                m_root.manualChange()
//            }
//            function updatePosition() {
//                if (pointList.length >= 4) {
//                    pointList[3].x = (point4.x - point4.xOffset)
//                    pointList[3].y = (point4.y - point4.yOffset)
//                }
//            }
//        }
//    }
//    Rectangle {
//        id: point5
//        property int xOffset: -width/2
//        property int yOffset: -height/2
//        visible: (pointList.length >= 3)
//        x: (pointList.length >= 5) ? pointList[4].x + xOffset : 0
//        y: (pointList.length >= 5) ? pointList[4].y + yOffset : 0
//        width: cornerWidth
//        height: width
//        opacity: alpha
//        color: pointColor
//        radius: width/2
//        MouseArea {
//            id: pt5MA
//            enabled: m_root.enabled
//            anchors.fill: parent
//            hoverEnabled: true
//            drag {
//                target: point5
//                axis: Drag.XAndYAxis
//                minimumX: point5.xOffset
//                maximumX: m_root.width + point5.xOffset
//                minimumY: point5.yOffset
//                maximumY: m_root.height + point5.yOffset
//                threshold: Style.drag_threshold
//            }
//            onPositionChanged:  {
//                if (drag.active)
//                    updatePosition()
//            }
//            onReleased: {
//                updatePosition()
//                m_root.manualChange()
//            }
//            function updatePosition() {
//                if (pointList.length >= 5) {
//                    pointList[4].x = (point5.x - point5.xOffset)
//                    pointList[4].y = (point5.y - point5.yOffset)
//                }
//            }
//        }
//    }
    MLine {
        strokeColor: pointColor
        fillColor: pointColor
        visible: (pointList.length >= 2)
        x1: pt.x - pt.xOffset
        x2: point2.x - point2.xOffset
        y1: pt.y - pt.yOffset
        y2: point2.y - point2.yOffset
    }
    MLine {
        strokeColor: pointColor
        fillColor: pointColor
        visible: (pointList.length >= 3)
        x1: point2.x - point2.xOffset
        x2: point3.x - point3.xOffset
        y1: point2.y - point2.yOffset
        y2: point3.y - point3.yOffset
    }
//    MLine {
//        strokeColor: pointColor
//        fillColor: pointColor
//        visible: (pointList.length >= 4)
//        x1: point3.x - point3.xOffset
//        x2: point4.x - point4.xOffset
//        y1: point3.y - point3.yOffset
//        y2: point4.y - point4.yOffset
//    }
//    MLine {
//        strokeColor: pointColor
//        fillColor: pointColor
//        visible: (pointList.length >= 5)
//        x1: point4.x - point4.xOffset
//        x2: point5.x - point5.xOffset
//        y1: point4.y - point4.yOffset
//        y2: point5.y - point5.yOffset
//    }
}
