import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2

ApplicationWindow {
    visible: true
    //visibility: "FullScreen"
    color: "#1c1c1e"

    FileDialog {
        id: videoSelectDialog
    }

    Rectangle {
        id: leftPanel
       // height: parent.height
        width: 200
        height: 300
        border.color: "#040409"
        color: "#1c1c1e"

        border.width: 2
        ColumnLayout {
            MButton {
                id: fileSelect
                text: "Open video"
                onClicked: videoSelectDialog.open()
            }

            MButton {
                id: start
                text: "Start"
                onClicked: Qt.quit();
            }
        }
    }

//    MainForm {
//        anchors.fill: parent
//        mouseArea.onClicked: {
//            Qt.quit();
//        }
//    }
//    ProgressBar {
//        value: 50
//        style: ProgressBarStyle {
//            background: Rectangle {
//                radius: 2
//                color: "lightgray"
//                border.color: "gray"
//                border.width: 1
//                implicitWidth: 200
//                implicitHeight: 24
//            }
//            progress: Rectangle {
//                color: "lightsteelblue"
//                border.color: "steelblue"
//            }
//        }
//    }
}
