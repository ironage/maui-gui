import QtQuick 2.5
import "."

Rectangle {
    id: m_root
    anchors.fill: parent
    color: "transparent"
    property real alpha: 0.70

    property alias roiX: roi.x
    property alias roiY: roi.y
    property alias roiWidth: roi.width
    property alias roiHeight: roi.height

    property int cornerWidth: 15
    property int rectMin: 20
    property bool backgroundActive: true
    property color backgroundColor: Style.ui_color_dark_grey
    property double backgroundAlpha: 0.40

    Rectangle {
        visible: backgroundActive
        x: 0
        y: 0
        width: m_root.width
        height: roi.y
        color: backgroundColor
        opacity: backgroundAlpha
    }
    Rectangle {
        visible: backgroundActive
        x: 0
        y: roi.y
        width: roi.x
        height: roi.height
        color: backgroundColor
        opacity: backgroundAlpha
    }
    Rectangle {
        visible: backgroundActive
        x: 0
        y: roi.y + roi.height
        width: m_root.width
        height: m_root.height - (roi.y + roi.height)
        color: backgroundColor
        opacity: backgroundAlpha
    }
    Rectangle {
        visible: backgroundActive
        x: roi.x + roi.width
        y: roi.y
        width: m_root.width - (roi.x + roi.width)
        height: roi.height
        color: backgroundColor
        opacity: backgroundAlpha
    }
    Rectangle {
        id: roi
        x: 50
        y: 50
        width: 100
        height: 100
        border.width: 3
        border.color: Style.ui_color_light_red
        opacity: alpha
        color: "transparent"

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
                maximumX: roi.x + roi.width - rectMin + cornerTL.xOffset
                minimumY: cornerTL.yOffset
                maximumY: roi.y + roi.height + cornerTL.yOffset - rectMin
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
                roi.width = roi.x - cornerTL.x + roi.width + cornerTL.xOffset
                roi.x = cornerTL.x - cornerTL.xOffset
                roi.height = roi.y - cornerTL.y + roi.height + cornerTL.yOffset
                roi.y = cornerTL.y - cornerTL.yOffset
            }
        }
    }

    Rectangle {
        id: cornerTR
        property int xOffset: -width/2
        property int yOffset: -height/2
        x: roi.x + roi.width + xOffset
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
                target: cornerTR
                axis: Drag.XAndYAxis
                minimumX: roi.x + rectMin + cornerTR.xOffset
                maximumX: m_root.width + cornerTR.xOffset
                minimumY: cornerTR.yOffset
                maximumY: roi.y + roi.height + cornerTR.yOffset - rectMin
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
                roi.width = cornerTR.x - roi.x - cornerTR.xOffset
                roi.height = roi.y - cornerTR.y + roi.height + cornerTR.yOffset
                roi.y = cornerTR.y - cornerTR.yOffset            }
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
