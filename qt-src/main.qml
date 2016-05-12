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
        width: 400
        height: 500
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

            Item {
                id: m_ex_group
                property int body_v_padding: Style.v_padding
                property int header_width: 200

            }

            MExpander {
                id: import_video
                title: "Step 1: Select Input"
                override_width: m_ex_group.header_width
                onOpened: {
                    calibration.close()
                    wall_detection.close()
                }

                payload: Component {
                    ColumnLayout {
                        Item {
                            width: parent.width
                            height: m_ex_group.body_v_padding
                        }
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
                        Item {
                            width: parent.width
                            height: m_ex_group.body_v_padding
                        }
                    }
                }
            }
            MExpander {
                id: calibration
                title: "Step 2: Calibration"
                override_width: m_ex_group.header_width
                anchors.top: import_video.bottom
                onOpened: {
                    wall_detection.close()
                    import_video.close()
                }

                payload: Component {
                    ColumnLayout {
                        Item {
                            width: parent.width
                            height: m_ex_group.body_v_padding
                        }
                        MButton {
                            id: cal_crop
                            text: "Crop ROI"
                            Layout.alignment: Qt.AlignCenter
                        }
                        MButton {
                            id: cal_select
                            text: "Select Pixel Boundaries"
                            Layout.alignment: Qt.AlignCenter
                        }
                        Item {
                            width: parent.width
                            height: m_ex_group.body_v_padding
                        }
                    }
                }
            }
            MExpander {
                id: wall_detection
                title: "Step 3: Wall Detection"
                override_width: m_ex_group.header_width
                anchors.top: calibration.bottom
                onOpened: {
                    calibration.close()
                    import_video.close()
                }

                payload: Component {
                    ColumnLayout {
                        Item {
                            width: parent.width
                            height: m_ex_group.body_v_padding
                        }
                        MButton {
                            id: wall_crop
                            text: "Crop ROI"
                            Layout.alignment: Qt.AlignCenter
                        }
                        MButton {
                            id: wall_select_top
                            text: "Select Wall Top"
                            Layout.alignment: Qt.AlignCenter
                        }
                        MButton {
                            id: wall_select_bottom
                            text: "Select Wall Bottom"
                            Layout.alignment: Qt.AlignCenter
                        }
                        Item {
                            width: parent.width
                            height: m_ex_group.body_v_padding
                        }
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
