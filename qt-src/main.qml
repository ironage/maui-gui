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
    id: window
    visible: true
    //visibility: "FullScreen"
    color: Style.ui_form_bg
    width: 900
    height: 600
    visibility: ApplicationWindow.Maximized
    minimumWidth: 400
    minimumHeight: 300
    title: "Measurements from Arterial Ultrasound Imaging (MAUI)"
    property bool controlsEnabled: true

    MFinishWindow {
        id: videoOutputProgress
        onCanceled: {
            m_video.doProcessOutputVideo = false
        }
    }
    MMessageWindow {
        id: videoFinishedSuccess
        title: "Video Processing Finished!"
        text: "The video has been processed successfully!"
        onVisibleChanged: {
            if (visible) {
                videoOutputProgress.close()
            }
        }

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
        conversionUnits: wallDetectionPane.conversionUnits
        conversionPixels: m_video.video_height <= 0 ? 1 : (scale.mappedBottomValue - scale.mappedTopValue) / wallDetectionPane.scale
        //outputName: import_video.outputName === "" ? import_video.defaultOutputName : import_video.outputName
        outputDir: outputPane.outputDirectory
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
            window.controlsEnabled = true
            requiresVerifyOnContinue = true

            loginWindow.preset("", "")
            loginWindow.setMessage("Multiple user sessions have been detected.\nOnly one active session is allowed per account.")
            loginWindow.show()
        }
        onValidationSuccess: {
            if (!doneInitialVerify) {
                doneInitialVerify = true

                window.controlsEnabled = false

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
        Component.onCompleted: {
            var previousFolder = remoteInterface.getLocalSetting("directory_in")
            inputPane.setFolder(previousFolder)
        }
    }
    Timer {
        id: validationTimer
        triggeredOnStart: false
        interval: 30000
        repeat: false
        running: false
        onTriggered: {
            if (summaryPane.isPlaying) {
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
            id: leftPanel
            property int headerWidth: 220
            Layout.alignment: Qt.AlignTop
            Layout.topMargin: Style.h_padding
            Layout.leftMargin: Style.h_padding
            spacing: Style.v_padding * 2

            MPaneInput {
                id: inputPane
                Layout.leftMargin: Style.h_padding
                Layout.bottomMargin: Style.v_padding * 2
                width: leftPanel.headerWidth
                height: 100
                enabled: window.controlsEnabled
                onVideoSelected: {
                    m_video.source = path
                    remoteInterface.setLocalSetting("directory_in", folder)

                    var fullName = m_video.readSrcName + "." + m_video.readSrcExtension
                    summaryPane.fileName = fullName
                    logMetaData.inputFileName = fullName
                    logMetaData.inputFilePath = m_video.readSrcDir
                    addFile(path, folder, fullName)
                }
            }

            MPaneOutput {
                id: outputPane
                Layout.leftMargin: Style.h_padding
                Layout.bottomMargin: Style.v_padding * 2
                width: leftPanel.headerWidth
                enabled: window.controlsEnabled
            }

            MPaneWallDetect {
                id: wallDetectionPane
                Layout.leftMargin: Style.h_padding
                Layout.bottomMargin: Style.v_padding * 2
                width: leftPanel.headerWidth
                enabled: window.controlsEnabled
            }

            MPaneVelocityDetect {
                id: velocityDetectionPane
                Layout.leftMargin: Style.h_padding
                Layout.bottomMargin: Style.v_padding * 2
                width: leftPanel.headerWidth
                enabled: window.controlsEnabled
            }

            MSummaryPane {
                id: summaryPane
                Layout.leftMargin: Style.h_padding
                Layout.bottomMargin: Style.v_padding * 2

                width: leftPanel.headerWidth
                height: 35

                function doContinue() {
                    window.controlsEnabled = false

                    m_video.continueProcessing();
                }

                onPlayClicked: {
                    remoteInterface.doneInitialVerify = false
                    remoteInterface.validateWithExistingCredentials()
                    m_video.doProcessOutputVideo = outputPane.processOutputVideo
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
                    window.controlsEnabled = true
                    m_video.pause()
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
                onProgressChanged: {
                    if (summaryPane.isPlaying) {
                        m_video_control.moveTo(progress)
                    }
                }
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
                    m_video_control.end_percent = 1
                    m_video_control.start_percent = 0 // do this last so cur frame is start
                    roi.reInitToCenter()
                }
                onVideoRectChanged: {
                    scale.initializeMappedPoints(m_video.width, m_video.height)
                }

                onVideoFinished: {
                    window.controlsEnabled = true
                    if (state === 0) { // MCVPlayer.SUCCESS
                        summaryPane.setStartState("ready")
                        videoFinishedSuccess.show()
                    } else if (state === 1) { // MCVPlayer.AUTO_INIT_FAILED
                        summaryPane.setStartState("paused")
                        videoFinishedError.text = "Could not find initial points on the start frame!"
                        videoFinishedError.informativeText = "Try adjusting the region of interest."
                        videoFinishedError.show()
                    } else {
                        summaryPane.setStartState("ready")
                        console.log("unhandled state when video finished: " + state)
                    }
                }
                onOutputProgress: {
                    videoOutputProgress.progress = progress
                    if (videoOutputProgress.visible == false) {
                        videoOutputProgress.show()
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
                    visible: wallDetectionPane.checked && (m_video.source !== "")
                    adjustable: !summaryPane.isPlaying
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
                    visible: wallDetectionPane.checked && (m_video.source !== "")
                    text: wallDetectionPane.scale + " " + wallDetectionPane.conversionUnits
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
                    totalFrames: m_video.duration > 0 ? m_video.duration - 1 : 0
                    onSetProgress: m_video.seek(percent * m_video.duration)
                    Layout.fillWidth: true
                    Layout.bottomMargin: Style.v_padding * 3
                    Layout.rightMargin: (2 * Style.h_padding)
                    Layout.leftMargin: (2 * Style.h_padding)
                    Layout.topMargin: Style.v_padding * 2
                    enabled: summaryPane.isPlaying === false
                }
            }
        }
    }
    MText {
        id: versionString
        text: " v" + remoteInterface.getDisplayVersion() + "  "
        style: Text.Normal
        color: Style.ui_color_dark_dblue
        font.pixelSize: 12
        anchors.bottom: parent.bottom
        anchors.left: parent.left
    }
}
