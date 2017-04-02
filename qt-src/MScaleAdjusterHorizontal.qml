import QtQuick 2.5
import "."

Rectangle {
    id: m_root
    anchors.fill: parent
    color: "transparent"
    property alias lineWidth: line.height
    property alias gripSize: slider.gripSize
    property real alpha: 0.70
    property int endMarkWidth: 5
    property int minLineHeight: 2
    property alias text: m_text.text
    property int mappedVValue: 20
    property int mappedLeftValue: 20
    property int mappedRightValue: 40
    property color scaleColor: Style.ui_color_dark_red
    property color scaleHighlightColor: Style.ui_color_light_red
    property int verticalLineOffset: ((slider.gripSize / 2) + 2)
    property bool highlightAll: slider.activeHover || slider2.activeHover || lineArea.containsMouse || lineArea.drag.active

    function changeMappedPoints(vValue, leftValue, rightValue) {
        mappedVValue = vValue
        mappedLeftValue = leftValue
        mappedRightValue = rightValue
    }
    function updateViewPoints(vValue, leftValue, rightValue) {
        slider.updateVPos(vValue)
        slider.updateHPos(leftValue)
        slider2.updateHPos(rightValue)
    }
    function initializeMappedPoints(newWidth, newHeight, newVPos, newLeftPos, newRightPos) {
        updateViewPoints(newVPos * newHeight, newLeftPos * newWidth, newRightPos * newWidth)
        viewPointsChanged(newVPos * newHeight, newLeftPos * newWidth, newRightPos * newWidth)
    }
    function initializeMappedPointsToCurrent(newWidth, newHeight) {
        initializeMappedPoints(newWidth, newHeight, slider.vPos / newHeight, slider.hPos / newWidth, slider2.hPos / newWidth)
    }

    signal viewPointsChanged(int vValue, int leftValue, int rightValue)

    MText {
        id: m_text
        text: "10 cm"
        x: line.x + (line.width / 2) - (width / 2)
        y: line.y + (slider.height / 2) + (slider.height / 2)
    }
    MScaleHandle {
        id: slider
        increment: 0.0
        gripSize: 20 // keep this even, else off by one from rounding errors happen
        rotationAngle: 90
        alpha: m_root.alpha
        startY: 0.25 * m_root.height
        startX: 0.25 * m_root.width
        fillColorHighlight: scaleHighlightColor
        fillColor: scaleColor
        outsideHighlight: m_root.highlightAll
        endMarkMax: vPos + verticalLineOffset
        dragSpecs.maximumX: slider2.hPos - minLineHeight
        onVPosChanged: {
            slider2.updateVPos(vPos)
            line.y = vPos + verticalLineOffset
        }
        onDragUpdate: {
            viewPointsChanged(vPos, slider.hPos, slider2.hPos)
        }
        onDoneDrag: {
            viewPointsChanged(vPos, slider.hPos, slider2.hPos)
        }
    }
    Rectangle {
        id: line
        x: slider.hPos
        width: slider2.hPos - slider.hPos + 1
        property int saved_width: 10
        height: 5
        color: m_root.highlightAll || lineArea.containsMouse || lineArea.drag.active ? slider.fillColorHighlight : slider.fillColor
        opacity: alpha
        MouseArea {
            id: lineArea
            enabled: m_root.enabled
            anchors.fill:  parent
            hoverEnabled: true
            drag {
                target: line
                axis: Drag.YAxis
                minimumX: -1
                maximumX: m_root.width - slider.width - 1
                minimumY: 0
                maximumY: m_root.height - lineArea.height
                threshold: Style.drag_threshold
            }
            onPositionChanged:  {
                if (drag.active)
                    updatePosition()
            }
            onPressed: {
                line.saved_width = line.width
            }

            onReleased: {
                updatePosition()
                viewPointsChanged(slider.vPos, slider.hPos, slider2.hPos)
            }
            function updatePosition() {
                slider.updateVPos(line.y - verticalLineOffset)
                slider2.updateVPos(line.y - verticalLineOffset)
            }
        }
    }
    MScaleHandle {
        id: slider2
        increment: 0.0
        gripSize: 20
        alpha: m_root.alpha
        rotationAngle: 90
        startY: 0.75 * m_root.height
        fillColorHighlight: slider.fillColorHighlight
        fillColor: slider.fillColor
        outsideHighlight: m_root.highlightAll
        dragSpecs.minimumX: slider.hPos + minLineHeight + 1
        dragSpecs.maximumX: parent.width
        endMarkMax: vPos + verticalLineOffset
        onVPosChanged: {
            slider.updateVPos(vPos)
        }
        onDragUpdate: {
            viewPointsChanged(vPos, slider.hPos, slider2.hPos)
        }
        onDoneDrag: {
            viewPointsChanged(vPos, slider.hPos, slider2.hPos)
        }
    }
}
