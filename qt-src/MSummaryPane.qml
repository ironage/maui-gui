import QtQuick 2.0
import QtQuick.Layouts 1.1
import "."

Item {
    id: m_root

    property string fileName: ""
    property string startFrame: ""
    property string endFrame: ""
    property string scaleString: ""
    property int scalePixelValue: 0
    property double scaleDistanceValue: 1
    property double scaleComputedValue: scalePixelValue / scaleDistanceValue
    property string scaleUnitString: "cm"
    property int leftMarginPadding: Style.h_padding
    property bool isPlaying: start.state === "playing"

    signal playClicked()
    signal continueClicked()
    signal pauseClicked()

    function pauseIfPlaying() {
        if (start.state === "playing") {
            start.state = "paused"
            pauseClicked()
        }
    }

    function setStartState(newState) {
        start.state = newState
    }

    function cancelValidation() {
        if (start.state === "validating") {
            start.state = "ready"
        }
    }

    MButton {
        id: start
        text: "Start"
        anchors.fill: parent
        state: "not_ready"
        onClicked: {
            if (state === "ready") {
                m_root.playClicked()
                state = "validating"
            } else if (state === "playing") {
                m_root.pauseClicked()
                state = "paused"
            } else if (state === "paused") {
                m_root.continueClicked()
                state = "playing"
            } else if (state === "validating") {

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
                PropertyChanges { target: start; text: "Start" }
            },
            State {
                name: "playing"
                PropertyChanges { target: start; color: Style.ui_color_dark_turquoise }
                PropertyChanges { target: start; highlight_color: Style.ui_color_light_turquoise }
                PropertyChanges { target: start; selected_color: Style.ui_color_dark_turquoise }
                PropertyChanges { target: start; text: "Pause" }
            },
            State {
                name: "paused"
                PropertyChanges { target: start; color: Style.ui_color_dark_lblue }
                PropertyChanges { target: start; highlight_color: Style.ui_color_light_lblue }
                PropertyChanges { target: start; selected_color: Style.ui_color_dark_grey }
                PropertyChanges { target: start; text: "Continue" }
            },
            State {
                name: "validating"
                PropertyChanges { target: start; color: Style.ui_color_light_orange }
                PropertyChanges { target: start; highlight_color: Style.ui_color_light_orange }
                PropertyChanges { target: start; selected_color: Style.ui_color_light_orange }
                PropertyChanges { target: start; text: "Validating..." }
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
