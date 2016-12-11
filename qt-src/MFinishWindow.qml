import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import "." // Style

Window {
    id: windowRoot

    property alias text: contentText.text
    property alias progress: progressBar.value
    property alias progressVisible: progressBar.visible

    width: layout.childrenRect.width + (Style.h_padding * 2)
    height: layout.height + (Style.v_padding * 2)
    minimumHeight: layout.height + (Style.v_padding * 2)
    minimumWidth: layout.childrenRect.width + (Style.h_padding * 2)
    modality: Qt.WindowModal
    flags: Qt.Dialog | Qt.CustomizeWindowHint | Qt.WindowTitleHint & (~Qt.WindowContextHelpButtonHint)
    title: "Writing Output Video"

    signal accepted()
    signal canceled()

    ColumnLayout {
        id: layout
        anchors.centerIn: parent
        spacing: 10

        Text {
            id: contentText
            Layout.alignment: Qt.AlignHCenter
            text: "Rendering output video..."
        }
        ProgressBar {
            id: progressBar
            value: 0
            minimumValue: 0
            maximumValue: 100
        }
        Button {
            text: "Cancel"
            onClicked: {
                canceled()
                //windowRoot.close()
            }
            Layout.alignment: Qt.AlignHCenter
        }
    }
}
