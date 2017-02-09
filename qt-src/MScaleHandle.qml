import QtQuick 2.4
import "."

Item {
    id: m_root
    property int startY: 10
    property int startX: 10
    property alias hPos: grip.x
    property alias vPos: grip.y
    property color fill_color: Style.ui_color_dark_dblue
    property color stroke_color: Style.ui_color_dark_dblue
    property color fill_color_highlight: Style.ui_color_light_dblue
    property color stroke_color_highlight: Style.ui_color_light_dblue
    property int gripSize: 22
    property real increment: 0.1
    property bool enabled: true
    property real bound_width: parent.width
    property real bound_height: parent.height
    property alias alpha: grip.alpha
    property int end_mark_width: 10
    property int end_mark_max: end_line.x
    property alias drag_specs: mouse_area.drag
    property int rotationAngle: 0

    function updateHPos(newHPos) {
        grip.x = newHPos
    }
    function updateVPos(newVPos) {
        grip.y = newVPos
    }
    signal doneDrag()
    signal dragUpdate()

    width: grip.width
    height: grip.height

    Rectangle {
        id: end_line
        x: grip.x
        width: end_mark_width
        y: grip.y + (grip.height / 2)
        color: stroke_color
        opacity: grip.alpha
        height: 1
        transform: [
            Rotation { origin.x: 0; origin.y: 0; angle: 180 },
            Rotation { origin.x: 0; origin.y: 0; angle: rotationAngle }
        ]
    }
    MTriangle {
        id: grip
        x: startX
        y: startY
        xOffset: 2
        triangle_width: m_root.gripSize
        triangle_height: triangle_width
        stroke_color: m_root.stroke_color
        fill_color: m_root.fill_color
        stroke_color_highlight: m_root.stroke_color_highlight
        fill_color_highlight: m_root.fill_color_highlight
        transform: Rotation {
            origin.x: 0
            origin.y: (height / 2);
            angle: rotationAngle
        }

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
            onPositionChanged: {
                dragUpdate()
            }
            onReleased: {
                doneDrag()
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
            PropertyChanges { target: end_line; width: end_mark_max }
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
