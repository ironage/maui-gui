import QtQuick 2.5
import "."

Rectangle {
    id: m_root
    anchors.fill: parent
    color: "transparent"
    property alias lineWidth: line.width
    property alias gripSize: slider.gripSize
    property real alpha: 0.70
    property int end_mark_width: 5
    property int min_line_height: 2
    property alias text: m_text.text
    property int mappedHValue: 20
    property int mappedTopValue: 20
    property int mappedBottomValue: 40

    function changeMappedPoints(hValue, topValue, bottomValue) {
        mappedHValue = hValue
        mappedTopValue = topValue
        mappedBottomValue = bottomValue
        console.log("mapped points changed: " + hValue + " " + topValue + " " + bottomValue)
    }
    function updateViewPoints(hValue, topValue, bottomValue) {
        slider.updateHPos(hValue)
        slider.updateVPos(topValue)
        slider2.updateVPos(bottomValue)
    }
    function initializeMappedPoints(newWidth, newHeight) {
        updateViewPoints(0.7 * newWidth, 0.25 * newHeight, 0.75 * newHeight)
        viewPointsChanged(0.7 * newWidth, 0.25 * newHeight, 0.75 * newHeight)
    }

    signal viewPointsChanged(int hValue, int topValue, int bottomValue)

    MText {
        id: m_text
        text: "10 cm"
        x: line.x + slider.width
        y: line.y + (line.height/2) - (width/2)
        transform: Rotation { origin.x: 0; origin.y: 0; angle: 90}
    }
    MScaleHandle {
        id: slider
        increment: 0.0
        gripSize: 20 // keep this even, else off by one from rounding errors happen
        alpha: m_root.alpha
        startY: 0.25 * m_root.height
        fill_color_highlight: Style.ui_color_light_red
        stroke_color_highlight: Style.ui_color_light_red
        fill_color: Style.ui_color_dark_red
        stroke_color: Style.ui_color_dark_red
        drag_specs.maximumY: slider2.vPos - min_line_height - (height / 2)
        onHPosChanged: {
            line.x = hPos
            slider2.updateHPos(hPos)
        }
        onDoneDrag: {
            viewPointsChanged(hPos, slider.vPos, slider2.vPos)
        }
    }
    Rectangle {
        id: line
        y: slider.vPos - 1
        height: slider2.vPos - slider.vPos + 1
        property int saved_height: 10
        width: 5
        color: line_area.containsMouse || line_area.drag.active ? slider.fill_color_highlight : slider.fill_color
        opacity: alpha
        MouseArea {
            id: line_area
            enabled: m_root.enabled
            anchors.fill:  parent
            hoverEnabled: true
            drag {
                target: line
                axis: Drag.XAxis
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
                line.saved_height = line.height
            }

            onReleased: {
                updatePosition()
                viewPointsChanged(slider.hPos, slider.vPos, slider2.vPos)
            }
            function updatePosition() {
                slider.updateHPos(line.x)
                slider2.updateHPos(line.x)
            }
        }
    }
    MScaleHandle {
        id: slider2
        increment: 0.0
        gripSize: 20
        alpha: m_root.alpha
        startY: 0.75 * m_root.height
        fill_color_highlight: slider.fill_color_highlight
        stroke_color_highlight: slider.stroke_color_highlight
        fill_color: slider.fill_color
        stroke_color: slider.stroke_color
        drag_specs.minimumY: slider.vPos + min_line_height - (height / 2) + 1
        onHPosChanged: {
            slider.updateHPos(hPos)
            line.x = hPos
        }
        onDoneDrag: {
            viewPointsChanged(hPos, slider.vPos, slider2.vPos)
        }
    }
}
