import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import "." // Style

Window {
    id: windowRoot

    property alias text: contentText.text
    property alias informativeText: contentDetails.text
    width: layout.childrenRect.width + (Style.h_padding * 2)
    height: layout.height + (Style.v_padding * 2)
    minimumHeight: layout.height + (Style.v_padding * 2)
    minimumWidth: layout.childrenRect.width + (Style.h_padding * 2)
    modality: Qt.WindowModal
    flags: Qt.Dialog | Qt.CustomizeWindowHint | Qt.WindowTitleHint | Qt.WindowCloseButtonHint & (~Qt.WindowContextHelpButtonHint)

    signal accepted()

    ColumnLayout {
        id: layout
        anchors.centerIn: parent
        spacing: 10

        Text {
            id: contentText
            font.pointSize: contentDetails.font.pointSize + 1
            Layout.alignment: Qt.AlignHCenter
        }
        Text {
            id: contentDetails
            Layout.alignment: Qt.AlignHCenter
        }
        Button {
            text: "OK"
            onClicked: {
                accepted()
                windowRoot.close()
            }
            Layout.alignment: Qt.AlignHCenter
        }
    }
}
