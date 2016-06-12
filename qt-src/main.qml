import QtQuick 2.5
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
                        process.close()
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
                    property string scale: loader_item.scale
                    property string units: loader_item.units
                    onOpened: {
                        wall_detection.close()
                        import_video.close()
                        process.close()
                        scale.visible = true
                    }
                    onClosed: {
                        scale.visible = false
                    }
                    MouseArea { anchors.fill: parent; onClicked: { loader_item.focused = false } }

                    payload: Item {
                        width: childrenRect.width
                        height: childrenRect.height
                        property string scale: scale_input.acceptableInput ? scale_input.text : 1
                        property alias units: scale_units.currentText

                        ColumnLayout {
                            Item {
                                width: parent.width
                                height: leftPanel.body_v_padding
                            }
                            MTextInput {
                                id: scale_input
                                text: "10"
                                width: 20
                                placeholderText: "Scale"
                                borderColor: acceptableInput ? Style.ui_component_highlight : Style.ui_color_light_red
                                validator: DoubleValidator{bottom: 0.01; top: 999.0; decimals: 4; notation: DoubleValidator.StandardNotation}
                                horizontalAlignment: TextInput.AlignHCenter
                            }
                            MCombobox {
                                id: scale_units
                                width: 50
                                model: ListModel {
                                        id: cbItems
                                        ListElement { text: "mm"; color: "Yellow" }
                                        ListElement { text: "cm"; color: "Green" }
                                        ListElement { text: "in"; color: "Brown" }
                                    }
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
                        process.close()
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
                MExpander {
                    id: process
                    title: "Step 4: Process"
                    override_width: leftPanel.header_width
                    anchors.top: wall_detection.bottom
                    property string start_state: ""
                    onStart_stateChanged: {
                        loader_item.start_state = start_state
                    }

                    onOpened: {
                        calibration.close()
                        import_video.close()
                        wall_detection.close()
                    }
                    payload: Item {
                        width: childrenRect.width
                        height: childrenRect.height
                        property alias start_state: start.state

                        ColumnLayout {
                            Item {
                                width: parent.width
                                height: leftPanel.body_v_padding
                            }
                            MButton {
                                id: start
                                text: "Start"
                                Layout.leftMargin: Style.h_padding
                                Layout.rightMargin: Style.h_padding
                                //Layout.bottomMargin: Style.v_padding
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
                            Item {
                                width: parent.width
                                height: leftPanel.body_v_padding
                            }
                        }
                    }
                }
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
                    process.start_state = "ready"
                }
                onPlayback_stateChanged: {
                    if (playback_state === MediaPlayer.PausedState
                            || playback_state === MediaPlayer.StoppedState) {
                        process.start_state = "ready"
                    }
                }
                MScaleAdjuster {
                    id: scale
                    visible: false
                    text: calibration.scale + " " + calibration.units

                }
            }
            RowLayout {
                MVideoControl {
                    id: m_video_control
                    progress: m_video.progress
                    totalFrames: m_video.duration
                    onSetProgress: m_video.seek(percent * m_video.duration)
                    Layout.fillWidth: true
                    Layout.bottomMargin: Style.v_padding * 2
                    Layout.rightMargin: (2 * Style.h_padding)
                    Layout.leftMargin: (2 * Style.h_padding)
                    Layout.topMargin: Style.v_padding * 2
                }
            }
        }
    }
}
