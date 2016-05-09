import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2
import "."

ApplicationWindow {
    visible: true
    //visibility: "FullScreen"
    color: Style.ui_form_bg

    FileDialog {
        id: videoSelectDialog
    }

    Rectangle {
        id: leftPanel
        width: 200
        height: 300
        color: Style.ui_form_bg
        border.color: "#040409"
        border.width: 0

        ColumnLayout {
            anchors.centerIn: parent
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

            MExpander {
                id: import_video
                title: "Import Video"
                payload: load_video
            }

            Component {
                id: load_video
                ColumnLayout {

                    Layout.topMargin: Style.v_padding
                    Layout.bottomMargin: Style.v_padding
                    Layout.leftMargin: Style.h_padding
                    Layout.rightMargin: Style.h_padding
                    MButton {
                        id: open_video
                        text: "Open Video"
                        Layout.alignment: Qt.AlignCenter
                    }
                    MButton {
                        id: test_temp
                        text: "Test"
                        Layout.alignment: Qt.AlignCenter
                    }
                }
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
