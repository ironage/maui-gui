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
    property int min_line_height: 20
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
        fill_color_highlight: Style.ui_color_light_dblue
        stroke_color_highlight: Style.ui_color_light_dblue
        drag_specs.maximumY: (slider2.v_value * parent.height) - min_line_height - height
    }
    Rectangle {
        id: line
        x: (parent.h_value * parent.width) + 1
        y: parent.top_v_value * parent.height
        height: (parent.bottom_v_value - parent.top_v_value) * parent.height + 1
        width: 4
        border.width: 2
        border.color: Style.ui_color_dark_dblue
        color: Style.ui_color_light_dblue
        opacity: alpha
    }
    MScaleHandle {
        id: slider2
        increment: 0.0
        gripSize: 19
        alpha: m_root.alpha
        v_value: 0.75
        h_value: slider.h_value
        onH_valueChanged: slider.h_value = slider2.h_value
        fill_color_highlight: Style.ui_color_light_dblue
        stroke_color_highlight: Style.ui_color_light_dblue
        drag_specs.minimumY: (slider.v_value * parent.height) + min_line_height
    }
}
