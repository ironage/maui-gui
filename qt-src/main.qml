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
        outputDir: outputPane.outputDirectory
        velocityConversionUnits: velocityDetectionPane.conversionUnits
        velocityConversionPixels: m_video.video_height <= 0 ? 1 : (velocityVerticalScale.mappedBottomValue - velocityVerticalScale.mappedTopValue) / velocityDetectionPane.scale
        velocityTime: velocityDetectionPane.time
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

    MText {
        id: disclaimer
        width: leftPanel.headerWidth
        height: 80
        anchors.left: parent.left
        anchors.leftMargin: Style.h_padding
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Style.v_padding
        text: "Please reference the Measurements from Arterial Ultrasound Imaging (MAUI) software from Hedgehog Medical Inc. in your publications."
        style: Text.Normal
        color: Style.ui_color_dark_dblue
        font.pixelSize: 11
        wrapMode: Text.WordWrap
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
                height: 150
                enabled: window.controlsEnabled
                onVideoSelected: {
                    m_video.source = path
                    remoteInterface.setLocalSetting("directory_in", folder)
                }
                onDisplayVideo: {
                    m_video.source = path
                }
                onClearVideo: {
                    m_video.source = ""
                }
            }

            MPaneOutput {
                id: outputPane
                Layout.leftMargin: Style.h_padding
                Layout.bottomMargin: Style.v_padding * 2
                width: leftPanel.headerWidth
                enabled: window.controlsEnabled
                onTriggerSetInitialDirectory: {
                    var previousFolder = remoteInterface.getLocalSetting("directory_out")
                    setInitialDirectory(previousFolder)
                }
                onTriggerUpdateInitialDirectory: {
                    remoteInterface.setLocalSetting("directory_out", folder)
                }
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
                        inputPane.setCurrentProgress(progress)
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
                }
                onVideoRectChanged: {
                    scale.initializeMappedPoints(m_video.width, m_video.height, 0.75, 0.2, 0.6)
                    //velocityVerticalScale.initializeMappedPoints(m_video.width, m_video.height, 0.75, 0.2, 0.6)
                    console.log("source rect changed under ROI")
                }
                property bool firstLoad: true
                onVideoLoaded: {
                    if (inputPane.isLoadingNewVideos) {
                        var readableName = name + "." + extension
                        summaryPane.fileName = readableName
                        logMetaData.inputFileName = fullName
                        logMetaData.inputFilePath = dir
                        inputPane.addFile(success, fullName, dir, readableName)
                    }
                    if (firstLoad) {
                        console.log("first load, init ROI")
                        roi.reInitToCenter()
                    }
                    firstLoad = false
                    roi.recomputeMappedPoints()
                    roi.parentLayoutChanged()
                    //forceROIRefresh()
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
                    property int initialSize: 200
                    roiX: ~~(parent.width/2) - (initialSize/2)
                    roiY: ~~(parent.height/2) - (initialSize/2)
                    roiWidth: initialSize
                    roiHeight: initialSize
                    function reInitToCenter() {
                        roiX = ~~(parent.width/2) - (initialSize/2)
                        roiY = ~~(parent.height*.4) - (initialSize/2)
                        roiWidth = initialSize
                        roiHeight = initialSize
                        recomputeMappedPoints()
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
                    text: "" + (m_video.video_height <= 0 ? "" : (scale.mappedBottomValue - scale.mappedTopValue) + " pixels = ") + wallDetectionPane.scale + " " + wallDetectionPane.conversionUnits
                    onViewPointsChanged: {
                        var scaleTop = m_video.viewPointToVideoPoint(Qt.point(hValue, topValue))
                        var scaleBottom = m_video.viewPointToVideoPoint(Qt.point(hValue, bottomValue))
                        scale.changeMappedPoints(scaleTop.x, scaleTop.y, scaleBottom.y)
                    }
                    function initializeToParent() {
                        scale.updateViewPoints(0.7 * m_video.width, 0.4 * m_video.height, 0.6 * m_video.height)
                    }
                    function parentLayoutChanged() {
                        var newTop = m_video.videoPointToViewPoint(Qt.point(scale.mappedHValue, scale.mappedTopValue))
                        var newBottom = m_video.videoPointToViewPoint(Qt.point(scale.mappedHValue, scale.mappedBottomValue))
                        scale.updateViewPoints(newTop.x, newTop.y, newBottom.y)
                    }
                }
                MROI {
                    id: velocityROI
                    visible: velocityDetectionPane.checked && (m_video.source !== "")
                    adjustable: !summaryPane.isPlaying
                    property int initialWidth: 400
                    property int initialHeight: 100
                    roiX: ~~(parent.width/2) - (initialWidth/2)
                    roiY: ~~(parent.height * 0.7)
                    roiWidth: initialWidth
                    roiHeight: initialHeight
                    roiColor: Style.ui_color_light_lblue
                }
                MScaleAdjuster {
                    id: velocityVerticalScale
                    visible: velocityDetectionPane.checked && (m_video.source !== "")
                    text: "" + (m_video.video_height <= 0 ? "" : (velocityVerticalScale.mappedBottomValue - velocityVerticalScale.mappedTopValue) + " pixels = ") + velocityDetectionPane.scale + " " + velocityDetectionPane.conversionUnits
                    scaleColor: Style.ui_color_dark_lblue
                    scaleHighlightColor: Style.ui_color_light_lblue
                    onViewPointsChanged: {
                        var scaleTop = m_video.viewPointToVideoPoint(Qt.point(hValue, topValue))
                        var scaleBottom = m_video.viewPointToVideoPoint(Qt.point(hValue, bottomValue))
                        velocityVerticalScale.changeMappedPoints(scaleTop.x, scaleTop.y, scaleBottom.y)
                    }
                    function initializeToParent() {
                        velocityVerticalScale.updateViewPoints(velocityROI.roiX + velocityROI.roiWidth + 25, velocityROI.roiY + (velocityROI.roiHeight * 0.1) - (gripSize/2), velocityROI.roiY + velocityROI.roiHeight - (2 * velocityROI.roiHeight * 0.1) - (gripSize/2))
                    }
                    function parentLayoutChanged() {
                        var newTop = m_video.videoPointToViewPoint(Qt.point(velocityVerticalScale.mappedHValue, velocityVerticalScale.mappedTopValue))
                        var newBottom = m_video.videoPointToViewPoint(Qt.point(velocityVerticalScale.mappedHValue, velocityVerticalScale.mappedBottomValue))
                        velocityVerticalScale.updateViewPoints(newTop.x, newTop.y, newBottom.y)
                    }
                }
                MScaleAdjusterHorizontal {
                    id: velocityHorizontalScale
                    visible: velocityDetectionPane.checked && (m_video.source !== "")
                    text: "" + (m_video.video_height <= 0 ? "" : (velocityHorizontalScale.mappedRightValue - velocityHorizontalScale.mappedLeftValue) + " pixels = ") + velocityDetectionPane.time + (velocityDetectionPane.time === "1" ? " second" : " seconds")
                    scaleColor: Style.ui_color_dark_lblue
                    scaleHighlightColor: Style.ui_color_light_lblue
                    onViewPointsChanged: {
                        var scaleLeft = m_video.viewPointToVideoPoint(Qt.point(leftValue, vValue))
                        var scaleRight = m_video.viewPointToVideoPoint(Qt.point(rightValue, vValue))
                        velocityHorizontalScale.changeMappedPoints(scaleLeft.y, scaleLeft.x, scaleRight.x)
                    }
                    function parentLayoutChanged() {
                        var newLeft = m_video.videoPointToViewPoint(Qt.point(velocityHorizontalScale.mappedLeftValue, velocityHorizontalScale.mappedVValue))
                        var newRight = m_video.videoPointToViewPoint(Qt.point(velocityHorizontalScale.mappedRightValue, velocityHorizontalScale.mappedVValue))
                        velocityHorizontalScale.updateViewPoints(newLeft.x, newLeft.y, newRight.y)
                    }
                    function initializeToParent() {
                        velocityHorizontalScale.updateViewPoints(velocityROI.roiY + velocityROI.roiHeight + 20, velocityROI.roiX + (velocityROI.roiWidth * 0.1) - (gripSize/2), velocityROI.roiX + velocityROI.roiWidth - (velocityROI.roiWidth * 0.1) + (gripSize/2))
                    }
                }
                Timer {
                    // This is because the hValue of each component in the scale
                    // depends on each other and by the time the window maximizes to the
                    // final height, the qml property binding has already been broken.
                    interval: 700
                    running: true
                    repeat: false
                    onTriggered: {
                        scale.initializeToParent()
                        velocityHorizontalScale.initializeToParent()
                        velocityVerticalScale.initializeToParent()
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
}
