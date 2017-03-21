import QtQuick 2.5
import "."

Rectangle {
    id: m_root
    anchors.fill: parent
    color: "transparent"
    property alias lineWidth: line.height
    property alias gripSize: slider.gripSize
    property real alpha: 0.70
    property int end_mark_width: 5
    property int min_line_height: 2
    property alias text: m_text.text
    property int mappedVValue: 20
    property int mappedLeftValue: 20
    property int mappedRightValue: 40
    property color scaleColor: Style.ui_color_dark_red
    property color scaleHighlightColor: Style.ui_color_light_red
    property int vertical_line_offset: ((slider.gripSize / 2) + 2)

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
        fill_color_highlight: scaleHighlightColor
        stroke_color_highlight: scaleHighlightColor
        fill_color: scaleColor
        stroke_color: scaleColor
        end_mark_max: vPos + vertical_line_offset
        drag_specs.maximumX: slider2.hPos - min_line_height
        onVPosChanged: {
            slider2.updateVPos(vPos)
            line.y = vPos + vertical_line_offset
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
        color: line_area.containsMouse || line_area.drag.active ? slider.fill_color_highlight : slider.fill_color
        opacity: alpha
        MouseArea {
            id: line_area
            enabled: m_root.enabled
            anchors.fill:  parent
            hoverEnabled: true
            drag {
                target: line
                axis: Drag.YAxis
                minimumX: -1
                maximumX: m_root.width - slider.width - 1
                minimumY: 0
                maximumY: m_root.height - line_area.height
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
                slider.updateVPos(line.y - vertical_line_offset)
                slider2.updateVPos(line.y - vertical_line_offset)
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
        fill_color_highlight: slider.fill_color_highlight
        stroke_color_highlight: slider.stroke_color_highlight
        fill_color: slider.fill_color
        stroke_color: slider.stroke_color
        drag_specs.minimumX: slider.hPos + min_line_height + 1
        drag_specs.maximumX: parent.width
        end_mark_max: vPos + vertical_line_offset
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
