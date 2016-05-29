import QtQuick 2.5
import "."

Rectangle {
    id: m_root
    anchors.fill: parent
    color: "transparent"
    property alias h_value: slider.h_value
    property alias top_v_value: slider.v_value
    property alias bottom_v_value: slider2.v_value
    property alias lineWidth: line.width
    property alias gripSize: slider.gripSize
    property real alpha: 0.70
    property int end_mark_width: 5
    property int min_line_height: 1
    property alias text: m_text.text

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
        gripSize: 19
        alpha: m_root.alpha
        v_value: 0.25
        h_value: 0.80
        onH_valueChanged: if (slider2 !== null) slider2.h_value = slider.h_value
        fill_color_highlight: Style.ui_color_light_red
        stroke_color_highlight: Style.ui_color_light_red
        fill_color: Style.ui_color_dark_red
        stroke_color: Style.ui_color_dark_red
        drag_specs.maximumY: (slider2.v_value * parent.height) - min_line_height - (height / 2)
    }
    Rectangle {
        id: line
        x: (parent.h_value * parent.width) - 1
        y: parent.top_v_value * parent.height
        height: line_area.drag.active ? saved_height : (parent.bottom_v_value - parent.top_v_value) * parent.height + 1
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
                axis: Drag.XAndYAxis
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
            }
            function updatePosition() {
                h_value = (line.x + 1) / m_root.width
                top_v_value = (line.y / m_root.height)
                bottom_v_value = (line.saved_height - 1) / m_root.height + m_root.top_v_value
            }
        }
    }
    MScaleHandle {
        id: slider2
        increment: 0.0
        gripSize: 19
        alpha: m_root.alpha
        v_value: 0.75
        h_value: slider.h_value
        onH_valueChanged: slider.h_value = slider2.h_value
        fill_color_highlight: slider.fill_color_highlight
        stroke_color_highlight: slider.stroke_color_highlight
        fill_color: slider.fill_color
        stroke_color: slider.stroke_color
        drag_specs.minimumY: (slider.v_value * parent.height) + min_line_height - (height / 2)
    }
}
