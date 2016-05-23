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

    MScaleHandle {
        id: slider
        increment: 0.0
        gripSize: 19
        alpha: m_root.alpha
        v_value: 0.25
        h_value: 0.80
        fill_color_highlight: Style.ui_color_light_dblue
        stroke_color_highlight: Style.ui_color_light_dblue
    }
    MText {
        text: "10 cm"
        x: line.x + slider.width
        y: line.y + (line.height/2) - (width/2)
        transform: Rotation { origin.x: 0; origin.y: 0; angle: 90}
    }
    Rectangle {
        x: line.x - end_mark_width
        y: line.y
        width: end_mark_width
        color: line.color
        opacity: alpha
        height: 1
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
    Rectangle {
        x: line.x - end_mark_width
        y: line.y + line.height - 1
        width: end_mark_width
        color: line.color
        opacity: alpha
        height: 1
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
    }
}
