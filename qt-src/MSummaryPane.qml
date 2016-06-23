import QtQuick 2.0
import QtQuick.Layouts 1.1
import "."

Rectangle {
    id: m_root
    border.width: Style.border_width
    border.color: Style.ui_border_color
    color: Style.ui_form_bg2

    // ColumnLayout doesn't work with dynamic eliding text
    height: childrenRect.height + 2 * Style.v_padding

    property string fileName: ""
    property string startFrame: ""
    property string endFrame: ""
    property string scaleString: ""
    property int scalePixelValue: 0
    property int scaleDistanceValue: 1
    property int scaleComputedValue: scalePixelValue / scaleDistanceValue
    property string scaleUnitString: "mm"
    property int leftMarginPadding: Style.h_padding

    signal playClicked()
    signal pauseClicked()

    onFileNameChanged: {
        height: childrenRect.height
    }

    function setStartState(newState) {
        start.state = newState
    }

    MText {
        id: nameText
        x: leftMarginPadding
        y: Style.v_padding
        width: m_root.width - 2 * leftMarginPadding
        text: "File: " + fileName
        elide: Text.ElideRight
        wrapMode: Text.WrapAnywhere
        maximumLineCount: 6
        style: Text.Normal
        color: Style.ui_color_dark_dblue
    }

    MText {
        id: startFrameText
        text: "Start Frame: " + startFrame
        Layout.leftMargin: leftMarginPadding
        style: Text.Normal
        color: Style.ui_color_dark_dblue
        anchors.top: nameText.bottom
        anchors.topMargin: Style.v_padding
        anchors.left: nameText.left
    }
    MText {
        id: endFrameText
        text: "End Frame: " + endFrame
        style: Text.Normal
        color: Style.ui_color_dark_dblue
        anchors.top: startFrameText.bottom
        anchors.topMargin: Style.v_padding
        anchors.left: startFrameText.left
    }
    MText {
        id: scaleText
        text: scalePixelValue === 0 ? "Scale: " : "Scale: " + scalePixelValue + "/" + scaleDistanceValue + " = " + scaleComputedValue + " pixels/" + scaleUnitString
        style: Text.Normal
        color: Style.ui_color_dark_dblue
        anchors.top: endFrameText.bottom
        anchors.topMargin: Style.v_padding
        anchors.left: endFrameText.left
    }

    MButton {
        id: start
        text: "Start"
        anchors.top: scaleText.bottom
        anchors.topMargin: Style.v_padding
        anchors.horizontalCenter: m_root.horizontalCenter
        state: "not_ready"
        onClicked: {
            if (state === "ready") {
                m_root.playClicked()
                state = "playing"
            } else if (state === "playing") {
                m_root.pauseClicked()
                state = "ready"
            }
        }

        states: [
            State {
                name: "not_ready"
                PropertyChanges { target: start; color: Style.ui_color_dark_red }
                PropertyChanges { target: start; highlight_color: Style.ui_color_light_red }
                PropertyChanges { target: start; selected_color: Style.ui_color_dark_red }
                PropertyChanges { target: start; text: "Start" }
            },
            State {
                name: "ready"
                PropertyChanges { target: start; color: Style.ui_color_dark_green }
                PropertyChanges { target: start; highlight_color: Style.ui_color_light_green }
                PropertyChanges { target: start; selected_color: Style.ui_color_dark_green }
                PropertyChanges { target: start; text: "Play" }
            },
            State {
                name: "playing"
                PropertyChanges { target: start; color: Style.ui_color_dark_grey }
                PropertyChanges { target: start; highlight_color: Style.ui_color_light_grey }
                PropertyChanges { target: start; selected_color: Style.ui_color_dark_grey }
                PropertyChanges { target: start; text: "Pause" }
            }
        ]
        transitions: [
            Transition {
                from: "*"; to: "*"
                ColorAnimation { target: start; properties: "color"; duration: 100 }
            }
        ]
    }
}
