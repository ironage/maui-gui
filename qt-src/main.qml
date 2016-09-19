import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2
import QtMultimedia 5.5
import QtQml 2.2
import "."
import com.maui.custom 1.0  // MPoint, MRemoteInterface

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
        title: "Select an input video"
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
            import_video.defaultOutputName = simpleFileName
            logMetaData.inputFileName = simpleFileName + fileExtension
            logMetaData.inputFilePath = directoryPath
            videoOutputDialog.folder = folder
        }
    }

    FileDialog {
        id: videoOutputDialog
        property string outputDirectory: ""
        selectFolder: true
        title: "Select an output directory"
        onAccepted: {
            var simpleName = fileUrl.toString();
            // unescape html codes like '%23' for '#'
            simpleName = decodeURIComponent(simpleName);
            var searchExpression = new RegExp("^((file:\\/{3})|(qrc:\\/{2})|(http:\\/{2}))(.*)", "g")
            var match = searchExpression.exec(simpleName)
            var directoryPath = match[5]
            outputDirectory = directoryPath
        }
    }
    MMessageWindow {
        id: videoFinishedSuccess
        title: "Video Processing Finished!"
        text: "The video has been processed successfully!"
        onAccepted: {
            validationTimer.stop()
            remoteInterface.finishSession();
        }
    }
    MMessageWindow {
        id: videoFinishedError
        title: "Video Processing Error!"
    }
    MMessageWindow {
        id: newVersionMessage
        title: "Update Available!"
        text: "A new version of MAUI is available for download!\nWould you like to perform the update now?";
        onAccepted: {
            remoteInterface.doUpdate()
        }
    }

    MLogMetaData {
        id: logMetaData
        conversionUnits: calibration.units
        conversionPixels: m_video.video_height <= 0 ? 1 : (scale.mappedBottomValue - scale.mappedTopValue) / calibration.scale
        outputName: import_video.outputName === "" ? import_video.defaultOutputName : import_video.outputName
        outputDir: videoOutputDialog.outputDirectory
    }

    MLoginWindow {
        id: loginWindow
        onVerifyAccount: remoteInterface.validateRequest(username, password)
        onClosing: {
            summaryPane.cancelValidation()
        }
    }

    MRemoteInterface {
        id: remoteInterface
        property bool doneInitialVerify: false
        property bool requiresVerifyOnContinue: false
        onNoExistingCredentials: {
            loginWindow.preset(username, password)
            loginWindow.setMessage("Please login to continue.")
            loginWindow.show()
        }
        onValidationNoConnection: {
            loginWindow.preset(username, password)
            loginWindow.setMessage("Connection failure.\nPlease check your internet connection and try again.")
            loginWindow.show()
            summaryPane.setStartState("ready")
        }
        onValidationFailed: {
            loginWindow.preset(username, password)
            loginWindow.setMessage("Login failed.\n" + failureReason)
            loginWindow.show()
            summaryPane.setStartState("ready")
        }
        onValidationBadCredentials: {
            loginWindow.preset(username, password)
            loginWindow.setMessage("Your username and password did not match.\nPlease try again.")
            loginWindow.show()
            summaryPane.setStartState("ready")
        }
        onValidationAccountExpired: {
            loginWindow.preset(username, password)
            loginWindow.setMessage("Your account has expired.\nPlease renew your account at hedgehogmedical.com")
            loginWindow.show()
            summaryPane.setStartState("ready")
        }
        onMultipleSessionsDetected: {
            summaryPane.pauseIfPlaying()
            calibration.enable()
            import_video.enable()
            wall_detection.enable()
            requiresVerifyOnContinue = true

            loginWindow.preset("", "")
            loginWindow.setMessage("Multiple user sessions have been detected.\nOnly one active session is allowed per account.")
            loginWindow.show()
        }
        onValidationSuccess: {
            if (!doneInitialVerify) {
                doneInitialVerify = true

                calibration.disable()
                import_video.disable()
                wall_detection.disable()
                roi.visible = true
                roi.adjustable = false

                m_video.play()
                summaryPane.setStartState("playing")
            } else if (requiresVerifyOnContinue) {
                //summaryPane.doContinue()
            }
            requiresVerifyOnContinue = false

            validationTimer.restart()
        }
        onSessionFinished: {
            console.log("session complete")
        }
        onValidationNewVersionAvailable: {
            newVersionMessage.show()
            summaryPane.setStartState("ready")
        }
    }
    Timer {
        id: validationTimer
        triggeredOnStart: false
        interval: 30000
        repeat: false
        running: false
        onTriggered: {
            if (summaryPane.isPlaying()) {
                remoteInterface.validateWithExistingCredentials()
            } else {
                validationTimer.restart()
            }
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
                scalePixelValue: m_video.video_height <= 0 ? "" : (scale.mappedBottomValue - scale.mappedTopValue)
                scaleDistanceValue: calibration.scale
                scaleUnitString: calibration.units

                function doContinue() {
                    calibration.disable()
                    import_video.disable()
                    wall_detection.disable()
                    roi.visible = true
                    roi.adjustable = false

                    m_video.continueProcessing();
                }

                onPlayClicked: {
                    remoteInterface.doneInitialVerify = false
                    remoteInterface.validateWithExistingCredentials()
                }
                onContinueClicked: {
                    if (remoteInterface.requiresVerifyOnContinue) {
                        loginWindow.preset("", "")
                        loginWindow.setMessage("Multiple user sessions have been detected.\nOnly one active session is allowed per account.")
                        loginWindow.show()
                    } else {
                        doContinue()
                    }
                }
                onPauseClicked: {
                    calibration.enable()
                    import_video.enable()
                    wall_detection.enable()
                    roi.visible = false

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
                        property string outputName: loader_item.outputName
                        property string defaultOutputName: "output name"
                        onDefaultOutputNameChanged: {
                            loader_item.defaultName = defaultOutputName
                        }

                        override_width: leftPanel.header_width
                        onOpened: {
                            calibration.close()
                            wall_detection.close()
                        }
                        payload: Item {
                            width: childrenRect.width
                            height: childrenRect.height
                            property alias outputName: outputTextInput.text
                            property string defaultName: "output name"
                            ColumnLayout {
                                Item {
                                    width: parent.width
                                    height: leftPanel.body_v_padding
                                }
                                MButton {
                                    id: open_video
                                    text: "Input Video"
                                    Layout.alignment: Qt.AlignCenter
                                    onClicked: videoSelectDialog.open()
                                }
                                MButton {
                                    id: save_video
                                    text: "Output Directory"
                                    Layout.alignment: Qt.AlignCenter
                                    onClicked: videoOutputDialog.open()
                                }
                                MText {
                                    text: "Output Title:"
                                    style: Text.Normal
                                    color: Style.ui_color_dark_dblue
                                    Layout.alignment: Qt.AlignCenter
                                }
                                MTextInput {
                                    id: outputTextInput
                                    text: ""
                                    width: 128
                                    placeholderText: defaultName
                                    borderColor: Style.ui_component_highlight
                                    horizontalAlignment: TextInput.AlignHCenter
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
                                    text: "1"
                                    width: 128
                                    placeholderText: "Scale"
                                    borderColor: acceptableInput ? Style.ui_component_highlight : Style.ui_color_light_red
                                    validator: DoubleValidator{bottom: 0.0001; top: 999.0; decimals: 4; notation: DoubleValidator.StandardNotation}
                                    horizontalAlignment: TextInput.AlignHCenter
                                }
                                MCombobox {
                                    id: scale_units
                                    width: 50
                                    model: ListModel {
                                        id: cbItems
                                        ListElement { text: "cm"; }
                                        ListElement { text: "mm"; }
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
                            roi.adjustable = true
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
                recomputeROIMode: roi.visible
                logData: logMetaData
                onWidthChanged: {
                    roi.parentLayoutChanged()
                    scale.parentLayoutChanged()
                }
                onHeightChanged: {
                    roi.parentLayoutChanged()
                    scale.parentLayoutChanged()
                }
                onSourceChanged: {
                    summaryPane.setStartState("ready")
                    roi.reInitToCenter()
                }
                onVideoRectChanged: {
                    scale.initializeMappedPoints(m_video.width, m_video.height)
                }

                onVideoFinished: {
                    calibration.enable()
                    import_video.enable()
                    wall_detection.enable()
                    if (state === 0) { // MCVPlayer.SUCCESS
                        summaryPane.setStartState("ready")
                        videoFinishedSuccess.show()
                    } else if (state === 1) { // MCVPlayer.AUTO_INIT_FAILED
                        summaryPane.setStartState("paused")
                        videoFinishedError.text = "Could not find initial points on the start frame!"
                        videoFinishedError.informativeText = "Try adjusting the region of interest."
                        videoFinishedError.show()
                        wall_detection.open()
                    } else {
                        summaryPane.setStartState("ready")
                        console.log("unhandled state when video finished: " + state)
                    }
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
                    function reInitToCenter() {
                        roiX = ~~(parent.width/2) - (initialSize/2)
                        roiY = ~~(parent.height/2) - (initialSize/2)
                        roiWidth = initialSize
                        roiHeight = initialSize
                    }
                    function parentLayoutChanged() {
                        var newXY = m_video.videoPointToViewPoint(mappedXY)
                        var newBR = m_video.videoPointToViewPoint(mappedBR)
                        roiX = newXY.x
                        roiY = newXY.y
                        roiWidth = newBR.x - newXY.x
                        roiHeight = newBR.y - newXY.y
                        updateLines()
                    }

                    property point mappedXY: Qt.point(1,1)
                    property point mappedBR: Qt.point(1,1)
                    property point mappedWH: Qt.point(1,1)

                    onRoiXChanged: updateLines()
                    onRoiYChanged: updateLines()
                    onRoiWidthChanged: updateLines()
                    onRoiHeightChanged: updateLines()

                    function recomputeMappedPoints() {
                        mappedXY = m_video.viewPointToVideoPoint(Qt.point(roi.roiX,roi.roiY))
                        mappedBR = m_video.viewPointToVideoPoint(Qt.point(roi.roiX + roi.roiWidth,roi.roiY + roi.roiHeight))
                        mappedWH = Qt.point(mappedBR.x - mappedXY.x, mappedBR.y - mappedXY.y)
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
                    visible: roi.visible && roi.adjustable
                }
                MPointLine {
                    id: line2
                    visible: roi.visible && roi.adjustable
                }

                MScaleAdjuster {
                    id: scale
                    visible: false
                    text: calibration.scale + " " + calibration.units
                    Timer {
                        // This is because the hValue of each component in the scale
                        // depends on each other and by the time the window maximizes to the
                        // final height, the qml property binding has already been broken.
                        interval: 700
                        running: true
                        repeat: false
                        onTriggered: {
                            scale.updateViewPoints(0.7 * m_video.width, 0.25 * m_video.height, 0.75 * m_video.height)
                        }
                    }
                    onViewPointsChanged: {
                        var scaleTop = m_video.viewPointToVideoPoint(Qt.point(hValue, topValue))
                        var scaleBottom = m_video.viewPointToVideoPoint(Qt.point(hValue, bottomValue))
                        scale.changeMappedPoints(scaleTop.x, scaleTop.y, scaleBottom.y)
                    }
                    function parentLayoutChanged() {
                        var newTop = m_video.videoPointToViewPoint(Qt.point(scale.mappedHValue, scale.mappedTopValue))
                        var newBottom = m_video.videoPointToViewPoint(Qt.point(scale.mappedHValue, scale.mappedBottomValue))
                        scale.updateViewPoints(newTop.x, newTop.y, newBottom.y)
                    }
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
