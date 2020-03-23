import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import "." // Custom Style

Item {
    id: root
    height: outputVideoCheckbox.height + saveVideo.height + header.height + (3 * Style.v_padding)
    property alias outputDirectory: videoOutputDialog.outputDirectory
    property alias processOutputVideo: outputVideoCheckbox.checked

    signal triggerUpdateInitialDirectory(string folder)
    signal triggerSetInitialDirectory()
    function setInitialDirectory(folder) {
        videoOutputDialog.folder = folder
    }

    Rectangle {
        id: background
        border.width: Style.border_width
        border.color: Style.ui_border_color
        color: Style.ui_form_bg2
        anchors.fill: parent

        MCheckbox {
            id: outputVideoCheckbox
            text: "Output visual results"
            checkboxWidth: 18
            checkboxHeight: 18
            middlePadding: (Style.h_padding / 2)
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: header.bottom
            anchors.topMargin: Style.v_padding
        }
        MButton {
            id: saveVideo
            text: "Output Directory"
            onClicked: videoOutputDialog.open()
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: outputVideoCheckbox.bottom
            anchors.topMargin: Style.v_padding
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
    }
    Rectangle {
        id: enabledCover
        anchors.fill: parent
        color: Style.ui_color_light_grey
        opacity: 0.8
        visible: !parent.enabled
    }

    FileDialog {
        id: videoOutputDialog
        property string outputDirectory: ""
        selectFolder: true
        title: "Select an output directory"
        onAccepted: {
            triggerUpdateInitialDirectory(folder)
            // FIXME: the following regex breaks on remote mounted server locations starting with "//"
            var simpleName = fileUrl.toString();
            console.log('file simple name: ' + simpleName)
            // unescape html codes like '%23' for '#'
            simpleName = decodeURIComponent(simpleName);
            var searchExpression = new RegExp("^((file:\\/{3})|(qrc:\\/{2})|(http:\\/{2}))(.*)", "g")
            var match = searchExpression.exec(simpleName)
            var directoryPath = match[5]
            outputDirectory = directoryPath
        }
        onVisibleChanged: {
            if (visible) {
                triggerSetInitialDirectory()
            }
        }
    }
}
