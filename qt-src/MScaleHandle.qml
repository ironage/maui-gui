import QtQuick 2.2
import "."

Rectangle {
    id: m_root
    color: "transparent"
    radius: 5
    property alias h_value: grip.h_value
    property alias v_value: grip.v_value
    property color fill_color: Style.ui_color_dark_dblue
    property color stroke_color: Style.ui_color_dark_dblue
    property color fill_color_highlight: Style.ui_color_light_dblue
    property color stroke_color_highlight: Style.ui_color_light_dblue
    property real gripSize: 20
    property real increment: 0.1
    property bool enabled: true
    property real bound_width: parent.width
    property real bound_height: parent.height
    property alias alpha: grip.alpha
    property int end_mark_width: 10
    property alias drag_specs: mouse_area.drag

    width: grip.width
    height: grip.height

    Rectangle {
        id: end_line
        x: grip.x - 1
        width: end_mark_width
        y: grip.y + (grip.height/2) + 1
        color: stroke_color
        opacity: grip.alpha
        height: 1
        transform: Rotation { origin.x: 0; origin.y: 0; angle: 180 }
    }
    MTriangle {
        id: grip
        property real h_value: 0.5
        property real v_value: 0.5
        x: (h_value * m_root.bound_width)
        y: (v_value * m_root.bound_height) - (height/2)
        triangle_width: m_root.gripSize
        triangle_height: triangle_width
        stroke_color: m_root.stroke_color
        fill_color: m_root.fill_color
        stroke_color_highlight: m_root.stroke_color_highlight
        fill_color_highlight: m_root.fill_color_highlight

        MouseArea {
            id: mouse_area
            enabled: m_root.enabled
            anchors.fill:  parent
            drag {
                target: grip
                axis: Drag.XAndYAxis
                minimumX: 0
                maximumX: m_root.bound_width - (grip.width)
                minimumY: -(grip.height/2)
                maximumY: m_root.bound_height - (grip.height/2)
                threshold: Style.drag_threshold
            }
            onPositionChanged:  {
                if (drag.active)
                    updatePosition()
            }
            onReleased: {
                updatePosition()
            }
            function updatePosition() {
                h_value = (grip.x) / m_root.bound_width
                v_value = (grip.y + (grip.height/2)) / m_root.bound_height
            }
        }
    }
    states: [
        State {
            name: ""
            PropertyChanges { target: end_line; width: end_mark_width; }
        },
        State {
            name: "expanded"
            PropertyChanges { target: end_line; width: end_line.x }
            when: mouse_area.pressed
        }
    ]
    transitions: [
        Transition {
            from: "expanded"
            to: ""
            PropertyAnimation { target: end_line; property: "width"; duration: 400; easing.type: Easing.OutCubic }
        },
        Transition {
            from: ""
            to: "expanded"
            PropertyAnimation { target: end_line; property: "width"; duration: 400; easing.type: Easing.OutCubic }
        }
    ]

}
