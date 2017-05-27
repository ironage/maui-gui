import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import "." // Custom Style

Item {
    id: root
    clip: true
    property bool isLoadingNewVideos: false
    ListModel {
        id: listModel
    }
    property var errorFiles: []

    signal videoSelected(string path, string folder)
    signal videoRemoved(string path)
    signal displayVideo(string path)
    signal clearVideo()

    function addFile(success, fullUrl, folder, displayName) {
        if (success) {
            listModel.append({"name": displayName, "path": fullUrl, "folder": folder, "percentComplete": 0.0})
            listView.currentIndex = listModel.count - 1
        } else {
            errorFiles.push(displayName)
        }
    }
    function setFolder(folder) {
        videoSelectDialog.folder = folder
    }
    function setCurrentProgress(progress) {
        if (listView.currentIndex < listModel.count) {
            listModel.get(listView.currentIndex).percentComplete = progress
            //console.log("setting progress: " + progress + " new percent: " + listModel.get(listView.currentIndex).percentComplete)
        }
    }

    function removeFromList(index) {
        if (index < listModel.count) {
            var oldPath = listModel.get(listView.currentIndex).path
            root.videoRemoved(oldPath)
            if (listModel.count == 1) {
                listModel.remove(index)
                clearVideo()
            } else {
                var newSelection = listView.currentIndex
                if (listView.currentIndex > index) {
                    newSelection = listView.currentIndex - 1
                }
                if (newSelection >= listModel.count - 1) {  // can happen when last == selected and remove last
                    newSelection = newSelection - 1
                }
                listModel.remove(index)
                listView.currentIndex = newSelection
                if (oldPath !== listModel.get(listView.currentIndex).path) {
                    displayVideo(listModel.get(listView.currentIndex).path)
                }
            }
        } else {
            console.log("removing invalid index: " + index + " from list of size " + listModel.count)
        }
    }

    Rectangle {
        id: background
        border.width: Style.border_width
        border.color: Style.ui_border_color
        color: Style.ui_form_bg2
        anchors.fill: parent
        ListView {
            id: listView
            anchors.top: header.bottom
            anchors.left: header.left
            anchors.right: header.right
            anchors.bottom: add.top
            anchors.leftMargin: Style.border_width
            anchors.rightMargin: Style.border_width
            spacing: 2
            Component {
                id: contactsDelegate
                Rectangle {
                    id: wrapper
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: contactInfo.height + Style.v_padding
                    color: Style.ui_color_silver
                    clip: true
                    border.width: ListView.isCurrentItem ? Style.border_width : 0
                    border.color: Style.ui_border_color

                    Rectangle {
                        id: progress
                        anchors.left: wrapper.left
                        anchors.top: wrapper.top
                        anchors.bottom: wrapper.bottom
                        color: Style.ui_color_dark_green
                        width: percentComplete * wrapper.width
                    }
                    MText {
                        id: contactInfo
                        text: name
                        style: Text.Normal
                        color: Style.ui_color_dark_dblue
                        anchors.left: wrapper.left
                        anchors.leftMargin: Style.h_padding
                        anchors.verticalCenter: wrapper.verticalCenter
                    }
                    MouseArea {
                        id: itemArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            if (listView.currentIndex !== index) {
                                listView.currentIndex = index
                                console.log("clicked item: " + path)
                                root.displayVideo(path)
                            }
                        }
                    }
                    MExitButton {
                        id: remove
                        height: parent.height
                        width: parent.height
                        anchors.right: parent.right
                        anchors.top: parent.top
                        presentable: itemArea.containsMouse
                        onClicked: {
                            removeFromList(index)
                        }
                    }
                }
            }
            model: listModel
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
                for (var c = 0; c < listModel.count; c++) {
                    // use indexOf to find duplicate entries since the
                    // "file://" prefix has been removed from loaded files
                    if (fileUrls[i].toString().indexOf(listModel.get(c).path) !== -1) {
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
