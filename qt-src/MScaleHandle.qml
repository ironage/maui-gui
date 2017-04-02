import QtQuick 2.4
import "."

Item {
    id: m_root
    property int startY: 10
    property int startX: 10
    property alias hPos: grip.x
    property alias vPos: grip.y
    property color fillColor: Style.ui_color_dark_lblue
    property color fillColorHighlight: Style.ui_color_bright_lblue
    property int gripSize: 22
    property real increment: 0.1
    property bool enabled: true
    property real boundWidth: parent.width
    property real boundHeight: parent.height
    property alias alpha: grip.alpha
    property int endMarkWidth: 10
    property int endMarkMax: end_line.x
    property alias dragSpecs: mouseArea.drag
    property int rotationAngle: 0
    property alias activeHover: grip.activeHover
    property bool outsideHighlight: false

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
        width: endMarkWidth
        y: grip.y + (grip.height / 2)
        color: scaleColor
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
        triangleWidth: m_root.gripSize
        triangleHeight: triangleWidth
        curColor: (activeHover || outsideHighlight) ? m_root.fillColorHighlight : m_root.fillColor
        transform: Rotation {
            origin.x: 0
            origin.y: (height / 2);
            angle: rotationAngle
        }

        MouseArea {
            id: mouseArea
            enabled: m_root.enabled
            anchors.fill:  parent
            drag {
                target: grip
                axis: Drag.XAndYAxis
                minimumX: 0
                maximumX: m_root.boundWidth - (grip.width)
                minimumY: -(grip.height/2)
                maximumY: m_root.boundHeight - (grip.height/2)
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
            PropertyChanges { target: end_line; width: endMarkWidth; }
        },
        State {
            name: "expanded"
            PropertyChanges { target: end_line; width: endMarkMax }
            when: mouseArea.pressed
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
