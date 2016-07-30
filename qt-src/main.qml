import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2
import QtMultimedia 5.5
import "."
import com.maui.custom 1.0  // MPoint

ApplicationWindow {
    visible: true
    //visibility: "FullScreen"
    color: Style.ui_form_bg
    width: 900
    height: 600
    visibility: ApplicationWindow.Maximized
    minimumWidth: 400
    minimumHeight: 300

    FileDialog {
        id: videoSelectDialog
        onAccepted: {
            m_video.source = fileUrl

            var simpleName = fileUrl.toString();
            // unescape html codes like '%23' for '#'
            simpleName = decodeURIComponent(simpleName);
            var searchExpression = new RegExp("^((file:\\/{3})|(qrc:\\/{2})|(http:\\/{2}))(.*)((\\\\)|(\\/))(.*)(\\..+)", "g")
            var match = searchExpression.exec(simpleName)
            var directoryPath = match[5]
            var simpleFileName = match[9]
            var fileExtension = match[10]
            summaryPane.fileName = simpleFileName + fileExtension
            m_video.logFileName = simpleFileName + fileExtension
            m_video.logFilePath = directoryPath
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 1
        Column {
            Layout.alignment: Qt.AlignTop
            Layout.topMargin: Style.h_padding
            Layout.leftMargin: Style.h_padding
            spacing: Style.v_padding * 4
            MSummaryPane {
                id: summaryPane
                Layout.leftMargin: Style.h_padding
                Layout.bottomMargin: Style.v_padding * 2

                width: leftPanel.header_width
                startFrame: m_video_control.totalFrames === 0 ? "" : "" + (~~(m_video_control.start_percent * m_video_control.totalFrames) + 1)
                endFrame: m_video_control.totalFrames === 0 ? "" : "" + (~~(m_video_control.end_percent * m_video_control.totalFrames) + 1)
                scalePixelValue: m_video.video_height <= 0 ? "" : (scale.bottom_v_value - scale.top_v_value) * m_video.video_height
                scaleDistanceValue: calibration.scale
                scaleUnitString: calibration.units

                onPlayClicked: {
                    calibration.close()
                    import_video.close()
                    wall_detection.close()

                    m_video.play()
                }
                onContinueClicked: {
                    m_video.continueProcessing();
                }
                onPauseClicked: {
                    m_video.pause()
                }
            }
            Rectangle {
                id: leftPanel
                property int body_v_padding: Style.v_padding
                property int header_width: 220

                height: childrenRect.height
                width: header_width
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
                                        ListElement { text: "mm"; }
                                        ListElement { text: "cm"; }
                                        ListElement { text: "in"; }
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
                            roi.updateLines()
                            roi.visible = true
                        }
                        onClosed: {
                            roi.visible = false
                        }

                        payload: Item {
                            width: childrenRect.width
                            height: childrenRect.height
                            ColumnLayout {
                                Item {
                                    width: parent.width
                                    height: leftPanel.body_v_padding
                                }
                                MText {
                                    text: "ROI x: " + Math.round(roi.mappedXY.x)
                                    style: Text.Normal
                                    color: Style.ui_color_dark_dblue
                                }
                                MText {
                                    text: "ROI y: " + Math.round(roi.mappedXY.y)
                                    style: Text.Normal
                                    color: Style.ui_color_dark_dblue
                                }
                                MText {
                                    text: "ROI width: " + Math.round(roi.mappedWH.x)
                                    style: Text.Normal
                                    color: Style.ui_color_dark_dblue
                                }
                                MText {
                                    text: "ROI height: " + Math.round(roi.mappedWH.y)
                                    style: Text.Normal
                                    color: Style.ui_color_dark_dblue
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
                roi: Qt.rect(roi.mappedXY.x, roi.mappedXY.y, roi.mappedWH.x, roi.mappedWH.y)
                logUnits: calibration.units
                logPixels: video_height <= 0 ? 1 : ((scale.bottom_v_value - scale.top_v_value) * video_height) / calibration.scale
                onSourceChanged: {
                    summaryPane.setStartState("ready")
                }
                onVideoFinished: {
                    summaryPane.setStartState("ready")
                }
                onTopPointsChanged: {
                    roi.updateLines()
                }
                onBottomPointsChanged: {
                    roi.updateLines()   //FIXME: split to not recompute both each time
                }

                //onVideoRectChanged: roi.recomputeMappedPoints()
                onPlayback_stateChanged: {
                    if (playback_state === MediaPlayer.PausedState
                            || playback_state === MediaPlayer.StoppedState) {
                        summaryPane.setStartState("ready")
                    }
                }
                MROI {
                    id: roi
                    visible: false
                    // Note: ROI will be reinitialzed to center when parent
                    // changes size, not sure if this is desirable or not
                    property int initialSize: 200
                    roiX: ~~(parent.width/2) - (initialSize/2)
                    roiY: ~~(parent.height/2) - (initialSize/2)
                    roiWidth: initialSize
                    roiHeight: initialSize
                    property point mappedXY: Qt.point(1,1)
                    property point mappedBL: Qt.point(1,1)
                    property point mappedWH: Qt.point(1,1)

                    onRoiXChanged: updateLines()
                    onRoiYChanged: updateLines()
                    onRoiWidthChanged: updateLines()
                    onRoiHeightChanged: updateLines()

                    function recomputeMappedPoints() {
                        mappedXY = m_video.viewPointToVideoPoint(Qt.point(roi.roiX,roi.roiY))
                        mappedBL = m_video.viewPointToVideoPoint(Qt.point(roi.roiX + roi.roiWidth,roi.roiY + roi.roiHeight))
                        mappedWH = Qt.point(mappedBL.x - mappedXY.x, mappedBL.y - mappedXY.y)
                    }

                    function updateLines() {

                        recomputeMappedPoints()

                        var newTopList = []
                        for (var topNdx = 0; topNdx < m_video.topPoints.length; ++topNdx) {
                            var topPoint = Qt.point(m_video.topPoints[topNdx].x, m_video.topPoints[topNdx].y)
                            topPoint = m_video.videoPointToViewPoint(topPoint)
                            newTopList.push(topPoint);
                        }
                        line1.pointList = newTopList

                        var newBottomList = []
                        for (var bottomNdx = 0; bottomNdx < m_video.bottomPoints.length; ++bottomNdx) {
                            var bottomPoint = Qt.point(m_video.bottomPoints[bottomNdx].x, m_video.bottomPoints[bottomNdx].y)
                            bottomPoint = m_video.videoPointToViewPoint(bottomPoint)
                            newBottomList.push(bottomPoint);
                        }
                        line2.pointList = newBottomList
                    }
                }
                MPointLine {
                    id: line1
                    visible: roi.visible
                }
                MPointLine {
                    id: line2
                    visible: roi.visible
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
                    totalFrames: m_video.duration > 0 ? m_video.duration - 1 : 0
                    onSetProgress: m_video.seek(percent * m_video.duration)
                    Layout.fillWidth: true
                    Layout.bottomMargin: Style.v_padding * 3
                    Layout.rightMargin: (2 * Style.h_padding)
                    Layout.leftMargin: (2 * Style.h_padding)
                    Layout.topMargin: Style.v_padding * 2
                }
            }
        }
    }
}
