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

    onClosing: {
        validationTimer.stop()
        remoteInterface.finishSession();
    }

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
    }
    MMessageWindow {
        id: videoFinishedError
        title: "Video Processing Error!"
    }

    MUpgradeWindow {
        id: updateWindow
        currentVersion: remoteInterface.getDisplayVersion()
        availableVersion: remoteInterface.softwareVersion
        changelog: remoteInterface.releaseNotes
        onStartUpdate: remoteInterface.doUpdate()
    }

    MLoginWindow {
        id: loginWindow
        onVerifyAccount: remoteInterface.validateRequest(username, password)
        onChangeAccount: remoteInterface.changeExistingCredentials(username, password)
    }

    MRemoteInterface {
        id: remoteInterface
        property bool doneInitialVerify: false
        property bool requiresVerifyOnContinue: false
        onNoExistingCredentials: {
            if (loginWindow.active === false) {
                loginWindow.preset(username, password)
            }
            loginWindow.setMessage("Please login to continue.")
            loginWindow.show()
        }
        onValidationNoConnection: {
            if (loginWindow.active === false) {
                loginWindow.preset(username, password)
            }
            loginWindow.setMessage("Connection failure.\nPlease check your internet connection and try again.")
            loginWindow.show()
            summaryPane.doStop()
        }
        onValidationFailed: {
            if (loginWindow.active === false) {
                loginWindow.preset(username, password)
            }
            loginWindow.setMessage("Login failed.\n" + failureReason)
            loginWindow.show()
            summaryPane.doStop()
        }
        onValidationBadCredentials: {
            if (loginWindow.active === false) {
                loginWindow.preset(username, password)
            }
            loginWindow.setMessage("Your username and password did not match.\nPlease try again.")
            loginWindow.show()
            summaryPane.doStop()
        }
        onValidationAccountExpired: {
            if (loginWindow.active === false) {
                loginWindow.preset(username, password)
            }
            loginWindow.setMessage("Your account has expired.\nPlease renew your account at hedgehogmedical.com")
            loginWindow.show()
            summaryPane.doStop()
        }
        onMultipleSessionsDetected: {
            summaryPane.pauseIfPlaying()
            window.controlsEnabled = true
            requiresVerifyOnContinue = true

            loginWindow.preset("", "")
            loginWindow.setMessage("Multiple user sessions have been detected.\nOnly one active session is allowed per account.")
            loginWindow.show()
        }
        onChangelogChanged: {
            settingsPane.shouldRequestChangeset = false
        }
        onValidationSuccess: {
            if (!doneInitialVerify) {
                doneInitialVerify = true
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
            settingsPane.updateAvailable()
        }
    }
    Timer {
        id: validationTimer
        triggeredOnStart: false
        interval: 30000
        repeat: false
        running: true
        onTriggered: {
            remoteInterface.validateWithExistingCredentials()
            validationTimer.restart()
        }
    }

    MText {
        id: disclaimer
        width: leftPanel.headerWidth
        anchors.left: parent.left
        anchors.leftMargin: Style.h_padding
        anchors.bottom: settingsPane.top
        anchors.bottomMargin: Style.v_padding
        text: "Please reference the Measurements from Arterial Ultrasound Imaging (MAUI) software from Hedgehog Medical Inc. in your publications."
        style: Text.Normal
        color: Style.ui_color_dark_dblue
        font.pixelSize: 11
        wrapMode: Text.WordWrap
    }

    MSettingsPane {
        id: settingsPane
        version: remoteInterface.getDisplayVersion()
        anchors.leftMargin: Style.h_padding
        anchors.bottomMargin: Style.v_padding
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        width: leftPanel.headerWidth
        property bool shouldRequestChangeset: true

        onUserClicked: {
            loginWindow.preset(remoteInterface.username, remoteInterface.password)
            loginWindow.setMessage("Current user account details:")
            loginWindow.showForChange()
        }
        onVersionClicked: {
            if (shouldRequestChangeset) {
                remoteInterface.requestChangelog()
            }
            updateWindow.show()
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 1
        Column {
            id: leftPanel
            property int headerWidth: 240
            Layout.alignment: Qt.AlignTop
            Layout.topMargin: Style.h_padding
            Layout.leftMargin: Style.h_padding
            spacing: Style.v_padding * 2

            MPaneInput {
                id: inputPane
                Layout.leftMargin: Style.h_padding
                Layout.bottomMargin: Style.v_padding * 2
                width: leftPanel.headerWidth
                height: 180
                enabled: window.controlsEnabled
                onVideoSelected: {
                    m_video.addVideo(path)
                    remoteInterface.setLocalSetting("directory_in", folder)
                }
                onOpeningInputDialog: {
                    var previousFolder = remoteInterface.getLocalSetting("directory_in")
                    inputPane.setFolder(previousFolder)
                }
                onVideoRemoved: {
                    m_video.removeVideo(path)
                }
                onDisplayVideo: {
                    m_video.changeToVideo(path)
                }
                onClearVideo: {
                    m_video.changeToVideo("")
                }
            }

            MCheckbox {
                id: syncVideoSettingsCheckbox
                text: "Remember Initialization Settings"
                checked: false
                textFontSizeMode: Text.Fit
                textMinPixelSize: 8
                textBoxWidth: leftPanel.headerWidth - checkboxWidth - middlePadding - Style.h_padding
                middlePadding: Style.h_padding / 2
                checkboxWidth: 18
                checkboxHeight: checkboxWidth
                Layout.leftMargin: Style.h_padding
                Layout.bottomMargin: Style.v_padding * 2
                anchors.horizontalCenter: leftPanel.horizontalCenter
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
                function doStop() {
                    window.controlsEnabled = true
                    m_video.pause()
                    setStartState("ready")
                }

                onPlayClicked: {
                    window.controlsEnabled = false
                    m_video.play()
                }
                onContinueClicked: {
                    if (remoteInterface.requiresVerifyOnContinue) {
                        loginWindow.preset(remoteInterface.username, remoteInterface.password)
                        loginWindow.setMessage("Please login to continue.")
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
                recomputeROIMode: roi.visible
                conversionUnits: wallDetectionPane.conversionUnits
                diameterConversion: wallDetectionPane.scale
                outputDir: outputPane.outputDirectory
                velocityConversionUnits: velocityDetectionPane.conversionUnits
                velocityConversion: velocityDetectionPane.scale
                velocityTime: velocityDetectionPane.time
                // See CameraTask::SetupState for interpretation of these values
                setupState: (wallDetectionPane.checked ? 1 : 0) * 1 + (velocityDetectionPane.checked ? 1 : 0) * 2
                doProcessOutputVideo: outputPane.processOutputVideo
                setupForAllVideos: syncVideoSettingsCheckbox.checked

                onProgressChanged: {
                    if (summaryPane.isPlaying) {
                        m_video_control.moveTo(progress)
                        inputPane.setCurrentProgress(progress)
                    }
                }
                onWidthChanged: {
                    roi.parentLayoutChanged()
                    velocityROI.parentLayoutChanged()
                    scale.parentLayoutChanged()
                }
                onHeightChanged: {
                    roi.parentLayoutChanged()
                    velocityROI.parentLayoutChanged()
                    scale.parentLayoutChanged()
                }
                onSourceChanged: {
                    summaryPane.setStartState("ready")
                    m_video_control.end_percent = 1
                    m_video_control.start_percent = 0 // do this last so cur frame is start
                }
                function updateOverlaysFromStoredData() {
                    roi.mappedXY = Qt.point(roiMapping.x, roiMapping.y)
                    roi.mappedWH = Qt.point(roiMapping.width, roiMapping.height)
                    roi.mappedBR = Qt.point(roi.mappedXY.x + roi.mappedWH.x, roi.mappedXY.y + roi.mappedWH.y)
                    roi.parentLayoutChanged()
                    velocityROI.mappedXY  = Qt.point(m_video.velocityROIMapping.x, m_video.velocityROIMapping.y)
                    velocityROI.mappedWH = Qt.point(m_video.velocityROIMapping.width, m_video.velocityROIMapping.height)
                    velocityROI.mappedBR = Qt.point(m_video.velocityROIMapping.x + m_video.velocityROIMapping.width, m_video.velocityROIMapping.y + m_video.velocityROIMapping.height)
                    velocityROI.parentLayoutChanged()

                    scale.changeMappedPoints(m_video.diameterScale.x, m_video.diameterScale.y,  m_video.diameterScale.y + m_video.diameterScale.height)
                    scale.parentLayoutChanged()
                    velocityVerticalScale.changeMappedPoints(m_video.velocityScaleVertical.x, m_video.velocityScaleVertical.y, m_video.velocityScaleVertical.y + m_video.velocityScaleVertical.height)
                    velocityVerticalScale.parentLayoutChanged()
                    velocityHorizontalScale.changeMappedPoints(m_video.velocityScaleHorizontal.y, m_video.velocityScaleHorizontal.x, m_video.velocityScaleHorizontal.x + m_video.velocityScaleHorizontal.width)
                    velocityHorizontalScale.parentLayoutChanged()
                }
                onContentRectChanged: updateOverlaysFromStoredData()
                onVideoRectChanged: {
                    updateOverlaysFromStoredData()
                }
                onVideoControlInfoChanged: {
                    updateOverlaysFromStoredData()

                    m_video_control.moveTo(m_video.progress)
                    wallDetectionPane.changeToUnits(m_video.conversionUnits)
                    wallDetectionPane.changeScale(m_video.diameterConversion)
                    velocityDetectionPane.changeToUnits(m_video.velocityConversionUnits)
                    velocityDetectionPane.changeScale(m_video.velocityConversion)
                    velocityDetectionPane.changeTime(m_video.velocityTime)
                    outputPane.processOutputVideo = m_video.doProcessOutputVideo

                    wallDetectionPane.checked = ((m_video.setupState & 1) > 0)
                    velocityDetectionPane.checked = ((m_video.setupState & 2) > 0)
                }
                onVideoLoaded: {
                    if (inputPane.isLoadingNewVideos) {
                        var readableName = name + "." + extension
                        summaryPane.fileName = readableName
                        inputPane.addFile(success, fullName, dir, readableName)
                    }
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
                    } else if (state === 2) { // MCVPlayer.VELOCITY_INIT_FAILED
                        summaryPane.setStartState("paused")
                        videoFinishedError.text = "Could not initialize the velocity!"
                        videoFinishedError.informativeText = "Try adjusting the selected velocity area."
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
//                onRoiMappingChanged: {
//                    roi.mappedXY = Qt.point(roiMapping.x, roiMapping.y)
//                    roi.mappedWH = Qt.point(roiMapping.width, roiMapping.height)
//                    roi.mappedBR = Qt.point(roi.mappedXY.x + roi.mappedWH.x, roi.mappedXY.y + roi.mappedWH.y)
//                    roi.parentLayoutChanged()
//                }

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

                    onMovedFromDrag: {
                        recomputeMappedPoints()
                        updateLines()
                    }

                    function recomputeMappedPoints() {
                        mappedXY = m_video.viewPointToVideoPoint(Qt.point(roi.roiX,roi.roiY))
                        mappedBR = m_video.viewPointToVideoPoint(Qt.point(roi.roiX + roi.roiWidth,roi.roiY + roi.roiHeight))
                        mappedWH = Qt.point(mappedBR.x - mappedXY.x, mappedBR.y - mappedXY.y)
                        m_video.roiMapping = Qt.rect(roi.mappedXY.x, roi.mappedXY.y, roi.mappedWH.x, roi.mappedWH.y)
                    }

                    function updateLines() {
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
                    minX: roi.roiX
                    minY: roi.roiY
                    maxX: roi.roiX + roi.roiWidth
                    maxY: roi.roiY + roi.roiHeight
                    onManualChange: {
                        var newTopList = []
                        for (var topNdx = 0; topNdx < pointList.length; ++topNdx) {
                            var topPoint = Qt.point(pointList[topNdx].x, pointList[topNdx].y)
                            topPoint = m_video.viewPointToVideoPoint(topPoint)
                            newTopList.push(topPoint);
                        }
                        m_video.setNewTopPoints(newTopList)
                    }
                }
                MPointLine {
                    id: line2
                    visible: roi.visible && roi.adjustable
                    minX: roi.roiX
                    minY: roi.roiY
                    maxX: roi.roiX + roi.roiWidth
                    maxY: roi.roiY + roi.roiHeight
                    onManualChange: {
                        var newTopList = []
                        for (var topNdx = 0; topNdx < pointList.length; ++topNdx) {
                            var topPoint = Qt.point(pointList[topNdx].x, pointList[topNdx].y)
                            topPoint = m_video.viewPointToVideoPoint(topPoint)
                            newTopList.push(topPoint);
                        }
                        m_video.setNewBottomPoints(newTopList)
                    }
                }
                MScaleAdjuster {
                    id: scale
                    visible: wallDetectionPane.checked && (m_video.source !== "") && !summaryPane.isPlaying
                    text: "" + (m_video.videoHeight <= 0 ? "" : (scale.mappedBottomValue - scale.mappedTopValue) + " pixels = ") + wallDetectionPane.scale + " " + wallDetectionPane.conversionUnits
                    onViewPointsChanged: {
                        var scaleTop = m_video.viewPointToVideoPoint(Qt.point(hValue, topValue))
                        var scaleBottom = m_video.viewPointToVideoPoint(Qt.point(hValue, bottomValue))
                        scale.changeMappedPoints(scaleTop.x, scaleTop.y, scaleBottom.y)
                        m_video.diameterScale = Qt.rect(scaleTop.x, scaleTop.y, 0, scaleBottom.y - scaleTop.y)
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
                    roiRestColor: Style.ui_color_dark_lblue
                    roiHoverColor: Style.ui_color_bright_lblue

                    property point mappedXY: Qt.point(1,1)
                    property point mappedBR: Qt.point(1,1)
                    property point mappedWH: Qt.point(1,1)

                    function parentLayoutChanged() {
                        var newXY = m_video.videoPointToViewPoint(mappedXY)
                        var newBR = m_video.videoPointToViewPoint(mappedBR)
                        roiX = newXY.x
                        roiY = newXY.y
                        roiWidth = newBR.x - newXY.x
                        roiHeight = newBR.y - newXY.y
                    }

                    onRoiXChanged: recomputeMappedPoints()
                    onRoiYChanged: recomputeMappedPoints()
                    onRoiWidthChanged: recomputeMappedPoints()
                    onRoiHeightChanged: recomputeMappedPoints()

                    function recomputeMappedPoints() {
                        mappedXY = m_video.viewPointToVideoPoint(Qt.point(velocityROI.roiX,velocityROI.roiY))
                        mappedBR = m_video.viewPointToVideoPoint(Qt.point(velocityROI.roiX + velocityROI.roiWidth,velocityROI.roiY + velocityROI.roiHeight))
                        mappedWH = Qt.point(mappedBR.x - mappedXY.x, mappedBR.y - mappedXY.y)
                        m_video.velocityROIMapping = Qt.rect(velocityROI.mappedXY.x, velocityROI.mappedXY.y, velocityROI.mappedWH.x, velocityROI.mappedWH.y)
                    }
                }
                MScaleAdjuster {
                    id: velocityVerticalScale
                    visible: velocityDetectionPane.checked && (m_video.source !== "") && !summaryPane.isPlaying
                    text: "" + (m_video.videoHeight <= 0 ? "" : (velocityVerticalScale.mappedBottomValue - velocityVerticalScale.mappedTopValue) + " pixels = ") + velocityDetectionPane.scale + " " + velocityDetectionPane.conversionUnits
                    scaleColor: Style.ui_color_dark_lblue
                    scaleHighlightColor: Style.ui_color_bright_lblue
                    onViewPointsChanged: {
                        var scaleTop = m_video.viewPointToVideoPoint(Qt.point(hValue, topValue))
                        var scaleBottom = m_video.viewPointToVideoPoint(Qt.point(hValue, bottomValue))
                        velocityVerticalScale.changeMappedPoints(scaleTop.x, scaleTop.y, scaleBottom.y)
                        m_video.velocityScaleVertical = Qt.rect(scaleTop.x, scaleTop.y, 0, scaleBottom.y - scaleTop.y)
                    }
                    function parentLayoutChanged() {
                        var newTop = m_video.videoPointToViewPoint(Qt.point(velocityVerticalScale.mappedHValue, velocityVerticalScale.mappedTopValue))
                        var newBottom = m_video.videoPointToViewPoint(Qt.point(velocityVerticalScale.mappedHValue, velocityVerticalScale.mappedBottomValue))
                        velocityVerticalScale.updateViewPoints(newTop.x, newTop.y, newBottom.y)
                    }
                }
                MScaleAdjusterHorizontal {
                    id: velocityHorizontalScale
                    //visible: velocityDetectionPane.checked && (m_video.source !== "") && !summaryPane.isPlaying
                    visible: false // disabled for now
                    text: "" + (m_video.videoHeight <= 0 ? "" : (velocityHorizontalScale.mappedRightValue - velocityHorizontalScale.mappedLeftValue) + " pixels = ") + velocityDetectionPane.time + (velocityDetectionPane.time === "1" ? " second" : " seconds")
                    scaleColor: Style.ui_color_dark_lblue
                    scaleHighlightColor: Style.ui_color_bright_lblue
                    onViewPointsChanged: {
                        var scaleLeft = m_video.viewPointToVideoPoint(Qt.point(leftValue, vValue))
                        var scaleRight = m_video.viewPointToVideoPoint(Qt.point(rightValue, vValue))
                        velocityHorizontalScale.changeMappedPoints(scaleLeft.y, scaleLeft.x, scaleRight.x)
                        m_video.velocityScaleHorizontal = Qt.rect(scaleLeft.x, scaleLeft.y, scaleRight.x - scaleLeft.x, 0)
                    }
                    function parentLayoutChanged() {
                        var newLeft = m_video.videoPointToViewPoint(Qt.point(velocityHorizontalScale.mappedLeftValue, velocityHorizontalScale.mappedVValue))
                        var newRight = m_video.videoPointToViewPoint(Qt.point(velocityHorizontalScale.mappedRightValue, velocityHorizontalScale.mappedVValue))
                        velocityHorizontalScale.updateViewPoints(newLeft.y, newLeft.x, newRight.x)
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
