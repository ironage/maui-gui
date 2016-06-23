import QtQuick 2.5
import "."

Rectangle {
    id: m_root
    anchors.fill: parent
    color: "transparent"
    property real alpha: 0.70

    property int cornerWidth: 15
    property int rectMin: 7
    Rectangle {
        id: roi
        x: 50
        y: 50
        width: 100
        height: 100
        border.width: 3
        border.color: Style.ui_color_light_red
        opacity: alpha
        color: "transparent"//        x: (parent.h_value * parent.width) - 1

        MouseArea {
            id: roi_area
            enabled: m_root.enabled
            anchors.fill:  parent
            hoverEnabled: true
            drag {
                target: roi
                axis: Drag.XAndYAxis
                minimumX: 0
                maximumX: m_root.width - roi.width
                minimumY: 0
                maximumY: m_root.height - roi.height
                threshold: Style.drag_threshold
            }
        }
    }
    Rectangle {
        id: cornerBR
        property int xOffset: -width/2
        property int yOffset: -height/2
        x: roi.x + roi.width + xOffset
        y: roi.y + roi.height + yOffset
        width: cornerWidth
        height: width
        //opacity: alpha
        color: roi.border.color
        radius: width/2
        MouseArea {
            enabled: m_root.enabled
            anchors.fill: parent
            hoverEnabled: true
            drag {
                target: cornerBR
                axis: Drag.XAndYAxis
                minimumX: roi.x + rectMin + cornerBR.xOffset
                maximumX: m_root.width + cornerBR.xOffset
                minimumY: roi.y + rectMin + cornerBR.yOffset
                maximumY: m_root.height + cornerBR.yOffset
                threshold: Style.drag_threshold
            }
            onPositionChanged: {
                if (drag.active)
                    updatePosition()
            }
            onReleased: {
                updatePosition()
            }
            function updatePosition() {
                roi.width = cornerBR.x - roi.x - cornerBR.xOffset
                roi.height = cornerBR.y - roi.y - cornerBR.yOffset
            }
        }
    }

    Rectangle {
        id: cornerTL
        property int xOffset: -width/2
        property int yOffset: -height/2
        x: roi.x + xOffset
        y: roi.y + yOffset
        width: cornerWidth
        height: width
        //opacity: alpha
        color: roi.border.color
        radius: width/2
        MouseArea {
            enabled: m_root.enabled
            anchors.fill: parent
            hoverEnabled: true
            drag {
                target: cornerTL
                axis: Drag.XAndYAxis
                minimumX: cornerTL.xOffset
                maximumX: roi.x + roi.width - rectMin + cornerBL.xOffset
                minimumY: roi.y + rectMin + cornerBL.yOffset
                maximumY: m_root.height + cornerBL.yOffset
                threshold: Style.drag_threshold
            }
            onPositionChanged: {
                if (drag.active)
                    updatePosition()
            }
            onReleased: {
                updatePosition()
            }
            function updatePosition() {
                roi.width = roi.x - cornerBL.x + roi.width + cornerBL.xOffset
                roi.x = cornerBL.x - cornerBL.xOffset
                roi.height = cornerBL.y - roi.y - cornerBL.yOffset
            }
        }
    }

    Rectangle {
        id: cornerTR
        property int xOffset: -width/2
        property int yOffset: -height/2
        x: roi.x + xOffset
        y: roi.y + roi.height + yOffset
        width: cornerWidth
        height: width
        //opacity: alpha
        color: roi.border.color
        radius: width/2
        MouseArea {
            enabled: m_root.enabled
            anchors.fill: parent
            hoverEnabled: true
            drag {
                target: cornerBL
                axis: Drag.XAndYAxis
                minimumX: cornerBR.xOffset
                maximumX: roi.x + roi.width - rectMin + cornerBL.xOffset
                minimumY: roi.y + rectMin + cornerBL.yOffset
                maximumY: m_root.height + cornerBL.yOffset
                threshold: Style.drag_threshold
            }
            onPositionChanged: {
                if (drag.active)
                    updatePosition()
            }
            onReleased: {
                updatePosition()
            }
            function updatePosition() {
                roi.width = roi.x - cornerBL.x + roi.width + cornerBL.xOffset
                roi.x = cornerBL.x - cornerBL.xOffset
                roi.height = cornerBL.y - roi.y - cornerBL.yOffset
            }
        }
    }

    Rectangle {
        id: cornerBL
        property int xOffset: -width/2
        property int yOffset: -height/2
        x: roi.x + xOffset
        y: roi.y + roi.height + yOffset
        width: cornerWidth
        height: width
        //opacity: alpha
        color: roi.border.color
        radius: width/2
        MouseArea {
            enabled: m_root.enabled
            anchors.fill: parent
            hoverEnabled: true
            drag {
                target: cornerBL
                axis: Drag.XAndYAxis
                minimumX: cornerBR.xOffset
                maximumX: roi.x + roi.width - rectMin + cornerBL.xOffset
                minimumY: roi.y + rectMin + cornerBL.yOffset
                maximumY: m_root.height + cornerBL.yOffset
                threshold: Style.drag_threshold
            }
            onPositionChanged: {
                if (drag.active)
                    updatePosition()
            }
            onReleased: {
                updatePosition()
            }
            function updatePosition() {
                roi.width = roi.x - cornerBL.x + roi.width + cornerBL.xOffset
                roi.x = cornerBL.x - cornerBL.xOffset
                roi.height = cornerBL.y - roi.y - cornerBL.yOffset
            }
        }
    }

}
