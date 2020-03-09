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
    property alias dragActive: roi_area.pressed

    property bool adjustable: true
    property int cornerWidth: 16
    property int rectMin: 20
    property bool handleHover: (mouseBL.containsMouse || mouseTL.containsMouse || mouseTR.containsMouse || mouseBR.containsMouse
                             || mouseMT.containsMouse || mouseMR.containsMouse || mouseMB.containsMouse || mouseML.containsMouse)

    property color roiRestColor: Style.ui_color_dark_red
    property color roiHoverColor: Style.ui_color_bright_red
    property color roiColor: (handleHover ? roiHoverColor : roiRestColor)

    signal movedFromDrag()

    Rectangle {
        id: roi
        x: 50
        y: 50
        width: 100
        height: 100
        border.width: 3
        border.color: roiColor
        opacity: alpha
        color: "transparent"

        MouseArea {
            id: roi_area
            enabled: m_root.enabled && adjustable
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
            onPositionChanged: {
                if (drag.active)
                    updatePosition()
            }
            onReleased: {
                updatePosition()
            }
            function updatePosition() {
                m_root.movedFromDrag()
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
        visible: adjustable
        color: roi.border.color
        radius: width/2
        MouseArea {
            id: mouseBR
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
                m_root.movedFromDrag()
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
        visible: adjustable
        //opacity: alpha
        color: roi.border.color
        radius: width/2
        MouseArea {
            id: mouseTL
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
                m_root.movedFromDrag()
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
        visible: adjustable
        //opacity: alpha
        color: roi.border.color
        radius: width/2
        MouseArea {
            id: mouseTR
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
                roi.y = cornerTR.y - cornerTR.yOffset
                m_root.movedFromDrag()
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
        visible: adjustable
        //opacity: alpha
        color: roi.border.color
        radius: width/2
        MouseArea {
            id: mouseBL
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
                m_root.movedFromDrag()
            }
        }
    }
    Rectangle {
        id: midTop
        property int xOffset: -width/2
        property int yOffset: -height/2
        x: roi.x + xOffset + (roi.width / 2)
        y: roi.y + yOffset
        width: cornerWidth
        height: width
        visible: adjustable
        //opacity: alpha
        color: roi.border.color
        radius: width/2
        MouseArea {
            id: mouseMT
            enabled: m_root.enabled
            anchors.fill: parent
            hoverEnabled: true
            drag {
                target: midTop
                axis: Drag.YAxis
                minimumY: midTop.yOffset
                maximumY: roi.y + roi.height + midTop.yOffset - rectMin
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
                roi.height = roi.y - midTop.y + roi.height + midTop.yOffset
                roi.y = midTop.y - midTop.yOffset
                m_root.movedFromDrag()
            }
        }
    }
    Rectangle {
        id: midBottom
        property int xOffset: -width/2
        property int yOffset: -height/2
        x: roi.x + xOffset + (roi.width / 2)
        y: roi.y + yOffset + roi.height
        width: cornerWidth
        height: width
        visible: adjustable
        //opacity: alpha
        color: roi.border.color
        radius: width/2
        MouseArea {
            id: mouseMB
            enabled: m_root.enabled
            anchors.fill: parent
            hoverEnabled: true
            drag {
                target: midBottom
                axis: Drag.YAxis
                minimumY: roi.y + rectMin + midBottom.yOffset
                maximumY: m_root.height + midBottom.yOffset
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
                roi.height = midBottom.y - roi.y - midBottom.yOffset
                m_root.movedFromDrag()
            }
        }
    }
    Rectangle {
        id: midLeft
        property int xOffset: -width/2
        property int yOffset: -height/2
        x: roi.x + xOffset
        y: roi.y + yOffset + (roi.height / 2)
        width: cornerWidth
        height: width
        visible: adjustable
        //opacity: alpha
        color: roi.border.color
        radius: width/2
        MouseArea {
            id: mouseML
            enabled: m_root.enabled
            anchors.fill: parent
            hoverEnabled: true
            drag {
                target: midLeft
                axis: Drag.XAxis
                minimumX: midLeft.xOffset
                maximumX: roi.x + roi.width - rectMin + midLeft.xOffset
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
                roi.width = roi.x - midLeft.x + roi.width + midLeft.xOffset
                roi.x = midLeft.x - midLeft.xOffset
                m_root.movedFromDrag()
            }
        }
    }
    Rectangle {
        id: midRight
        property int xOffset: -width/2
        property int yOffset: -height/2
        x: roi.x + xOffset + roi.width
        y: roi.y + yOffset + (roi.height / 2)
        width: cornerWidth
        height: width
        visible: adjustable
        //opacity: alpha
        color: roi.border.color
        radius: width/2
        MouseArea {
            id: mouseMR
            enabled: m_root.enabled
            anchors.fill: parent
            hoverEnabled: true
            drag {
                target: midRight
                axis: Drag.XAxis
                minimumX: roi.x + rectMin + midRight.xOffset
                maximumX: m_root.width + midRight.xOffset
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
                roi.width = midRight.x - roi.x - midRight.xOffset
                m_root.movedFromDrag()
            }
        }
    }
}
