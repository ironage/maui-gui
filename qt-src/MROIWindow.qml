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
    title: "ROI"

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
            text: "Region of Interest settings";
            maximumLineCount: 7
            font.pixelSize: 14
            Layout.alignment: Qt.AlignHCenter
            anchors.margins: Style.v_padding
            width: parent.width - (2 * Style.h_padding)
            wrapMode: Text.Wrap
            horizontalAlignment: Text.AlignHCenter
        }
        RowLayout {
            Button {
                id: copyButton
                text: " Copy to clipboard "
                state: ""
                Layout.alignment: Qt.AlignHCenter
                onClicked: {
                }
            }
            Button {
                id: pasteButton
                text: " Paste from clipboard "
                state: ""
                Layout.alignment: Qt.AlignHCenter
                onClicked: {
                }
            }
        }

        MText {
            id: labelX
            text: "X: ";
            font.pixelSize: 14
            Layout.alignment: Qt.AlignHCenter
            anchors.margins: Style.v_padding
            horizontalAlignment: Text.AlignHCenter
        }

        MTextInput {
            id: inputX

        }
    }
}
