import QtQuick 2.0
import QtQuick.Layouts 1.1
import "."

Item {
    id: root

    signal versionClicked()
    signal userClicked()
    property double version: 0
    property size imageSize: Qt.size(16, 16)
    width: childrenRect.width
    height: versionButton.height

    function updateAvailable() {
        versionButton.state = "update_avaliable"
    }

    Row {
        anchors.fill: parent
        MButton {
            id: versionButton
            text: ""
            textStyle: Text.Normal
            textColor: Style.ui_color_black
            textFont.pixelSize: 12
            v_padding: 4
            state: "up_to_date"
            onClicked: {
                root.versionClicked()
            }
            imageSourceSize: root.imageSize

            states: [
                State {
                    name: "up_to_date"
                    PropertyChanges { target: versionButton; color: Style.ui_color_dark_grey }
                    PropertyChanges { target: versionButton; highlight_color: Style.ui_color_light_grey }
                    PropertyChanges { target: versionButton; selected_color: Style.ui_color_dark_grey }
                    PropertyChanges { target: versionButton; text: "Version " + root.version }
                    PropertyChanges { target: versionButton; imageSource: "qrc:///icons/check.png" }
                },
                State {
                    name: "update_avaliable"
                    PropertyChanges { target: versionButton; color: Style.ui_color_dark_yellow }
                    PropertyChanges { target: versionButton; highlight_color: Style.ui_color_light_yellow }
                    PropertyChanges { target: versionButton; selected_color: Style.ui_color_dark_yellow }
                    PropertyChanges { target: versionButton; text: "Update Available" }
                    PropertyChanges { target: versionButton; imageSource: "qrc:///icons/download.png" }
                }
            ]
            transitions: [
                Transition {
                    from: "*"; to: "*"
                    ColorAnimation { target: versionButton; properties: "color"; duration: 100 }
                }
            ]
        }
        Item {
            id: padding
            width: Style.h_padding
            height: 1
        }

        MButton {
            id: userButton
            text: ""
            textStyle: Text.Normal
            textColor: Style.ui_color_black
            textFont.pixelSize: 12
            v_padding: 4
            state: "idle"
            imageSource: "qrc:///icons/user.png"
            imageSourceSize: root.imageSize
            onClicked: {
                root.userClicked()
            }
            states: [
                State {
                    name: "idle"
                    PropertyChanges { target: userButton; color: Style.ui_color_dark_grey }
                    PropertyChanges { target: userButton; highlight_color: Style.ui_color_light_grey }
                    PropertyChanges { target: userButton; selected_color: Style.ui_color_dark_grey }
                    PropertyChanges { target: userButton; text: "Account" }
                }
            ]
            transitions: [
                Transition {
                    from: "*"; to: "*"
                    ColorAnimation { target: userButton; properties: "color"; duration: 100 }
                }
            ]
        }
    }
}
