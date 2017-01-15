import QtQuick 2.2
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import "." // Custom Style

Item {
    id: root
    property alias outputDirectory: videoOutputDialog.outputDirectory
    Rectangle {
        id: background
        border.width: Style.border_width
        border.color: Style.ui_border_color
        color: Style.ui_form_bg2
        anchors.fill: parent

        Column {
            MButton {
                id: save_video
                text: "Output Directory"
                Layout.alignment: Qt.AlignCenter
                onClicked: videoOutputDialog.open()
            }
            CheckBox {
                id: processVideoCheckBox
                checked: true
                text: "Output video results"
            }
            MCheckbox {
                id: test
            }
        }
    }
    MButton {
        id: header
        border.width: Style.border_width
        border.color: Style.ui_border_color
        enabled: false
        Layout.minimumHeight: 20
        Layout.minimumWidth: 30
        width: root.width
        h_padding: 5
        v_padding: 5
        text: "Data To Save"
    }

    FileDialog {
        id: videoOutputDialog
        property string outputDirectory: ""
        selectFolder: true
        title: "Select an output directory"
        onAccepted: {
//            remoteInterface.setLocalSetting("directory_out", folder)
//            var simpleName = fileUrl.toString();
//            // unescape html codes like '%23' for '#'
//            simpleName = decodeURIComponent(simpleName);
//            var searchExpression = new RegExp("^((file:\\/{3})|(qrc:\\/{2})|(http:\\/{2}))(.*)", "g")
//            var match = searchExpression.exec(simpleName)
//            var directoryPath = match[5]
//            outputDirectory = directoryPath
        }
        onVisibleChanged: {
            if (visible) {
//                var previousFolder = remoteInterface.getLocalSetting("directory_out")
//                folder = previousFolder
            }
        }
    }
}
