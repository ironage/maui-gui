import QtQuick 2.5
import QtQuick.Layouts 1.1
import "."

Item {
    id: m_root
    width: 10
    height: 15
    property alias progress: progress_pin.value
    property alias start_percent: m_start_pin.value
    property alias end_percent: m_end_pin.value
    property bool movable: dragArea.active || progress_pin.dragActive
    property int totalFrames: 0
    signal setProgress(double percent)

    function moveTo(newProgress) {
        progress = newProgress
    }

    onProgressChanged: {
//        if (movable) {
//            if (progress - progress_pin.localPercentOffset < start_percent) {
//                start_percent = progress - progress_pin.localPercentOffset
//            }
//            if (progress + progress_pin.localPercentOffset > end_percent) {
//                end_percent = progress + progress_pin.localPercentOffset
//            }
//        }
    }

    Rectangle {
        width: m_root.width
        height: m_root.height
        color: Style.ui_color_light_green
        MouseArea {
            anchors.fill: parent
            drag {
                id: dragArea
                axis: Drag.XAxis
                target: progress_pin
                minimumX: (-progress_pin.width / 2)
                maximumX: m_root.width - (progress_pin.width / 2)
                threshold: Style.drag_threshold
            }
            onPressed: {
                if (m_root.width > 0) {
                    progress_pin.value = mouseX / m_root.width
                    progress_pin.signalUpdate()
                }
                progress_pin.activate()
            }
            onPositionChanged: {
                if (drag.active) {
                    progress_pin.signalUpdate()
                }
            }
            onReleased: {
                progress_pin.deactivate()
            }
            onWheel: {
                var newValue = progress
                if (wheel.angleDelta.y != 0 && totalFrames > 0) {
                    newValue = progress + (wheel.angleDelta.y / Math.abs(wheel.angleDelta.y)) / (totalFrames + 1)
                    console.log("scroll to " + newValue)
                    progress_pin.scrollTo(newValue)
                }
            }
        }
        Rectangle {
            color: Style.ui_color_dark_grey
            height: parent.height
            anchors.left: parent.left
            anchors.right: m_start_pin.horizontalCenter
        }
        Rectangle {
            color: Style.ui_color_dark_grey
            height: parent.height
            anchors.left: m_end_pin.horizontalCenter
            anchors.right: parent.right
        }
        MProgressPin {
            id: progress_pin
            property int localOffset: totalFrames > 0 ? (m_root.width / totalFrames) : 0
            padOffset: localOffset
            triangle_width: 20
            triangle_height: 20
            yOffset: -2
            value: 0.0
            enabled: parent.enabled
            onRequestNewValue: scrollTo(newValue)
            function scrollTo(newValue) {
                if (newValue >= 1) {
                    newValue = 1
                } else if (newValue <= 0) {
                    newValue = 0
                }
                value = newValue
                setProgress(newValue)
            }
        }
        MPin {
            id: m_start_pin
            value: 0.0
            description: totalFrames === 0 ? "" : ~~(value * totalFrames) + 1
            maximumX: m_end_pin.x - 3
            enabled: parent.enabled
        }
        MPin {
            id: m_end_pin
            value: 1.0
            description: totalFrames === 0 ? "" : ~~(value * totalFrames) + 1
            minimumX: m_start_pin.x + 3
            enabled: parent.enabled
        }
    }
}
