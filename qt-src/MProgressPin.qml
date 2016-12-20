import QtQuick 2.0
import "."

MProgressHandle {
    id: grip
    property real value: 0.5

    property int width_bound: parent.width
    property bool enabled: true
    property color selected_color: Style.ui_color_light_red
    property color idle_color: Style.ui_color_dark_dblue
    property color disabled_color: Style.ui_color_dark_grey
    property string description: ""
    property alias minimumX: dragArea.minimumX
    property alias maximumX: dragArea.maximumX
    property alias color: grip.curColor
    property alias dragActive: dragArea.active
    property double padOffset: 0.0
    property bool externalActive: false

    function activate() {
        externalActive = true
    }
    function deactivate() {
        externalActive = false
    }
    function signalUpdate() {
        mouseArea.updatePosition()
    }
    signal requestNewValue(double newValue);

    x: (value * parent.width) - width/2
    anchors.top: parent.bottom
    width: triangle_width
    height: triangle_height
    color: enabled ? ((dragArea.active || externalActive) ? selected_color : idle_color) : disabled_color
    alpha: 1

    MouseArea {
        id: mouseArea
        enabled: grip.enabled
        anchors.fill:  parent
        drag {
            id: dragArea
            target: grip
            axis: Drag.XAxis
            minimumX: -parent.width/2
            maximumX: width_bound - parent.width/2
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
            //var newValue = (grip.x + grip.width/2 - padOffset) / width_bound
            var newValue = (grip.x + grip.width/2) / width_bound
            requestNewValue(newValue)
        }
    }
    Rectangle {
        id: m_vbar
        width: 2
        height: grip.parent.height
        color: parent.color
        anchors.bottom: grip.top
        anchors.horizontalCenter: parent.horizontalCenter
    }
}
