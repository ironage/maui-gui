import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import "." // Style

Window {
    id: windowRoot

    color: Style.ui_form_bg
    width: 600
    height: 500
    minimumHeight: 200
    minimumWidth: 300
    modality: Qt.WindowModal
    flags: Qt.Dialog | Qt.CustomizeWindowHint | Qt.WindowTitleHint | Qt.WindowCloseButtonHint & (~Qt.WindowContextHelpButtonHint)
    title: "Update"

    signal startUpdate()

    property string currentVersion
    property string availableVersion
    property string changelog

    function setUpdateAvailable() {
        actionButton.state = "update"
    }

    ColumnLayout {
        id: layout
        anchors.top: parent.top
        anchors.topMargin: (2 * Style.v_padding)
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: Style.h_padding
        anchors.rightMargin: Style.h_padding
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Style.v_padding
        spacing: 12

        Text {
            id: message
            text: "You are running MAUI version: [" + currentVersion + "]\nThe latest version available is: [" + availableVersion + "]";
            maximumLineCount: 7
            font.pixelSize: 14
            Layout.alignment: Qt.AlignHCenter
            anchors.margins: Style.v_padding
            width: parent.width - (2 * Style.h_padding)
            wrapMode: Text.Wrap
            horizontalAlignment: Text.AlignHCenter
        }
        Button {
            id: updateButton
            text: " Run Maintenenace Tool to Update "
            state: ""
            Layout.alignment: Qt.AlignHCenter
            onClicked: {
                windowRoot.startUpdate()
                windowRoot.close()
            }
        }
        Rectangle {
            color: Style.ui_color_light
            border.color: Style.ui_border_color
            border.width: 1
            Layout.fillWidth: true
            Layout.fillHeight: true

            Flickable {
                id: flick
                width: parent.width
                height: parent.height
                contentWidth: changesText.paintedWidth
                contentHeight: changesText.paintedHeight
                clip: true

                TextEdit {
                    id: changesText
                    focus: true
                    padding: Style.h_padding
                    wrapMode: TextEdit.Wrap
                    text: windowRoot.changelog
                    color: Style.ui_color_black
                    selectionColor: Style.ui_color_light_grey
                    selectedTextColor: Style.ui_color_silver
                    font.pixelSize: 12
                    readOnly: true
                }
            }
        }
    }
}
