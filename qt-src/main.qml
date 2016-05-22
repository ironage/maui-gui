import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2
import QtMultimedia 5.5
import "."

ApplicationWindow {
    visible: true
    //visibility: "FullScreen"
    color: Style.ui_form_bg

    width: 900
    height: 600
    minimumWidth: 400
    minimumHeight: 300

    FileDialog {
        id: videoSelectDialog
        onAccepted: {
            m_video.source = fileUrl
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 1
        Rectangle {
            id: leftPanel
            property int body_v_padding: Style.v_padding
            property int header_width: 200

            height: childrenRect.height
            color: Style.ui_form_bg
            Layout.minimumWidth: header_width
            Layout.leftMargin: Style.h_padding
            //anchors.verticalCenter: parent.verticalCenter
            ColumnLayout {

                MExpander {
                    id: import_video
                    title: "Step 1: Select Input"
                    override_width: leftPanel.header_width
                    onOpened: {
                        calibration.close()
                        wall_detection.close()
                    }

                    payload: Component {
                        ColumnLayout {
                            Item {
                                width: parent.width
                                height: leftPanel.body_v_padding
                            }
                            MButton {
                                id: open_video
                                text: "Open Video"
                                Layout.alignment: Qt.AlignCenter
                                onClicked: videoSelectDialog.open()
                            }
                            MButton {
                                id: test_temp
                                text: "Test"
                                Layout.alignment: Qt.AlignCenter
                            }
                            Item {
                                width: parent.width
                                height: leftPanel.body_v_padding
                            }
                        }
                    }
                }
                MExpander {
                    id: calibration
                    title: "Step 2: Calibration"
                    override_width: leftPanel.header_width
                    anchors.top: import_video.bottom
                    onOpened: {
                        wall_detection.close()
                        import_video.close()
                    }

                    payload: Component {
                        ColumnLayout {
                            Item {
                                width: parent.width
                                height: leftPanel.body_v_padding
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
                                height: leftPanel.body_v_padding
                            }
                        }
                    }
                }
                MExpander {
                    id: wall_detection
                    title: "Step 3: Wall Detection"
                    override_width: leftPanel.header_width
                    anchors.top: calibration.bottom
                    onOpened: {
                        calibration.close()
                        import_video.close()
                    }

                    payload: Component {
                        ColumnLayout {
                            Item {
                                width: parent.width
                                height: leftPanel.body_v_padding
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
                                height: leftPanel.body_v_padding
                            }
                        }
                    }
                }
//                MText {
//                    text: "height: " + wall_detection.height
//                    color: "#ff0000"
//                }
            }
        }
        ColumnLayout {
            MVideoDisplay {
                id: m_video
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumWidth: 100
                Layout.margins: Style.h_padding
                progress_min: m_video_control.start_percent
                progress_max: m_video_control.end_percent
                onSourceChanged: {
                    start.state = "ready"
                }
            }
            RowLayout {
                MButton {
                    id: start
                    text: "Start"
                    Layout.leftMargin: Style.h_padding
                    Layout.rightMargin: Style.h_padding
                    Layout.bottomMargin: Style.v_padding
                    state: "not_ready"
                    onClicked: {
                        if (state === "ready") {
                            m_video.play()
                            state = "playing"
                        } else if (state === "playing") {
                            m_video.pause()
                            state = "ready"
                        }
                    }

                    states: [
                        State {
                            name: "not_ready"
                            PropertyChanges { target: start; color: Style.ui_color_dark_red }
                            PropertyChanges { target: start; highlight_color: Style.ui_color_light_red }
                            PropertyChanges { target: start; selected_color: Style.ui_color_dark_red }
                            PropertyChanges { target: start; text: "Start" }
                        },
                        State {
                            name: "ready"
                            PropertyChanges { target: start; color: Style.ui_color_dark_green }
                            PropertyChanges { target: start; highlight_color: Style.ui_color_light_green }
                            PropertyChanges { target: start; selected_color: Style.ui_color_dark_green }
                            PropertyChanges { target: start; text: "Play" }
                        },
                        State {
                            name: "playing"
                            PropertyChanges { target: start; color: Style.ui_color_dark_grey }
                            PropertyChanges { target: start; highlight_color: Style.ui_color_light_grey }
                            PropertyChanges { target: start; selected_color: Style.ui_color_dark_grey }
                            PropertyChanges { target: start; text: "Pause" }
                        }

                    ]
                    transitions: [
                        Transition {
                            from: "*"; to: "*"
                            ColorAnimation { target: start; properties: "color"; duration: 100 }
                        }
                    ]
                }
                MVideoControl {
                    id: m_video_control
                    progress: m_video.progress
                    movable: m_video.playback_state !== MediaPlayer.PlayingState
                    onSetProgress: m_video.seek(percent * m_video.duration)
                    Layout.fillWidth: true
                    Layout.bottomMargin: Style.v_padding
                    Layout.rightMargin: (2 * Style.h_padding)
                    Layout.leftMargin: Style.h_padding
                    Layout.topMargin: 0
                }
            }
        }
    }
}
