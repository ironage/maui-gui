import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import "." // Custom Style

Item {
    id: root
    property bool isLoadingNewVideos: false
    ListModel {
        id: contacts
    }
    property var errorFiles: []

    signal videoSelected(string path, string folder);

    function addFile(success, fullUrl, folder, displayName) {
        if (success) {
            contacts.append({"name": displayName, "path": fullUrl, "folder": folder})
        } else {
            errorFiles.push(displayName)
        }
    }
    function setFolder(folder) {
        videoSelectDialog.folder = folder
    }

    Rectangle {
        id: background
        border.width: Style.border_width
        border.color: Style.ui_border_color
        color: Style.ui_form_bg2
        anchors.fill: parent
        ListView {
            anchors.top: header.bottom
            anchors.left: header.left
            anchors.right: header.right
            anchors.bottom: add.top
            anchors.leftMargin: Style.border_width
            anchors.rightMargin: Style.border_width
            Component {
                id: contactsDelegate
                Rectangle {
                    id: wrapper
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: contactInfo.height
                    border.width: Style.border_width
                    border.color: Style.ui_border_color
                    color: Style.ui_color_light_grey
                    //color: ListView.isCurrentItem ? "black" : "red"
                    MText {
                        id: contactInfo
                        text: name
                        //color: wrapper.ListView.isCurrentItem ? "red" : "black"
                    }
                }
            }
            model: contacts
            delegate: contactsDelegate
            focus: true
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
            text: "Data To Process"
        }
        MButton {
            id: add
            border.width: Style.border_width
            border.color: Style.ui_border_color
            Layout.minimumHeight: 20
            Layout.minimumWidth: 30
            width: root.width
            height: 20
            anchors.bottom: parent.bottom
            h_padding: 5
            v_padding: 0
            text: "+"
            onClicked: {
                videoSelectDialog.open()
            }
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
        id: videoSelectDialog
        title: "Select an input video"
        selectMultiple: true
        onAccepted: {
            root.isLoadingNewVideos = true
            for (var i = 0; i < fileUrls.length; i++) {
                var found = false
                for (var c = 0; c < contacts.count; c++) {
                    // use indexOf to find duplicate entries since the
                    // "file://" prefix has been removed from loaded files
                    if (fileUrls[i].toString().indexOf(contacts.get(c).path) !== -1) {
                        found = true
                        console.log("found duplicate entry at position " + c + " name: " + fileUrls[i])
                        break;
                    }
                }
                if (!found) {
                    videoSelected(fileUrls[i], folder)
                }
            }
            root.isLoadingNewVideos = false

            if (errorFiles.length > 0) {
                if (errorFiles.length == 1) {
                    loadErrorWindow.text = "Unable to load the following video:"
                } else {
                    loadErrorWindow.text = "Unable to load the following " + errorFiles.length + " videos:"
                }
                loadErrorWindow.informativeText = ""
                for (var v = 0; v < errorFiles.length; v++) {
                    loadErrorWindow.informativeText += errorFiles[v] + "\n";
                }
                loadErrorWindow.show()
            }
            errorFiles = []
        }
    }
    MMessageWindow {
        id: loadErrorWindow
        title: "Error Loading Videos"
    }
}
