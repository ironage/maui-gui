import QtQuick 2.5
import QtQuick.Layouts 1.1
import "." // Style

Item {
    id: root
    width: 20   // users set these
    height: 20

    property color normalBgColor: Style.ui_color_light_red
    property color normalFgColor: Style.ui_color_light
    property color disabledBgColor: Style.ui_color_light_grey
    property color disabledFgColor: Style.ui_color_light
    property color highlightBgColor: Style.ui_color_dark_red
    property color highlightFgColor: Style.ui_color_light
    property color curBgColor: normalBgColor
    property color curFgColor: normalFgColor
    property bool presentable: false
    property bool enabled: true

    property int lineWidth: 3
    property bool fill: false
    property bool stroke: true
    property real alpha: 1
    property int outsidePadding: 3
    property int xOffset: 0
    property int yOffset: 0
    property real radius: 5
    signal clicked()

    onCurBgColorChanged: canvas.requestPaint()
    onCurFgColorChanged: canvas.requestPaint()

    Canvas {
        id: canvas
        antialiasing: true
        state: "hiding"
        width: root.width
        height: root.height
        property int realX: root.xOffset + outsidePadding
        property int realY: root.yOffset + outsidePadding
        property int realWidth: width - (2 * outsidePadding)
        property int realHeight: height - (2 * outsidePadding)

        states: [
            State {
                name: "hiding"    // default
                when: !mouseArea.containsMouse && !root.presentable
                PropertyChanges { target: root; curBgColor: root.normalBgColor }
                PropertyChanges { target: root; curFgColor: root.normalFgColor }
                PropertyChanges { target: root; fill: false }
                PropertyChanges { target: root; stroke: false }
            },
            State {
                name: "present";
                when: root.presentable && !mouseArea.containsMouse
                PropertyChanges { target: root; curBgColor: root.normalBgColor }
                PropertyChanges { target: root; curFgColor: root.normalFgColor }
                PropertyChanges { target: root; fill: false }
                PropertyChanges { target: root; stroke: true }
            },
            State {
                name: "disabledPresent";
                when: !root.enabled && root.presentable
                PropertyChanges { target: root; curBgColor: root.disabledBgColor }
                PropertyChanges { target: root; curFgColor: root.disabledFgColor }
                PropertyChanges { target: root; fill: true }
                PropertyChanges { target: root; stroke: true }
            },
            State {
                name: "disabledHiding";
                when: !root.enabled && !root.presentable
                PropertyChanges { target: root; curBgColor: root.disabledBgColor }
                PropertyChanges { target: root; curFgColor: root.disabledFgColor }
                PropertyChanges { target: root; fill: false }
                PropertyChanges { target: root; stroke: false }
            },
            State {
                name: "highlight"
                when: mouseArea.containsMouse
                PropertyChanges { target: root; curBgColor: root.highlightBgColor }
                PropertyChanges { target: root; curFgColor: root.highlightFgColor }
                PropertyChanges { target: root; fill: true }
                PropertyChanges { target: root; stroke: true }
            }
        ]
        transitions: [
            Transition {
                from: "*"; to: "*"
                ColorAnimation { target: root; properties: "curBgColor"; duration: 250 }
                ColorAnimation { target: root; properties: "curFgColor"; duration: 250 }
            }
        ]

        onStateChanged: {
            requestPaint()
        }

        onPaint: {
            var ctx = getContext("2d");
            ctx.save();
            ctx.clearRect(0, 0, width, height);
            ctx.strokeStyle = root.curBgColor;
            ctx.lineWidth = root.lineWidth
            ctx.fillStyle = root.curBgColor
            ctx.globalAlpha = root.alpha
            ctx.lineJoin = "bevel";

            if (root.fill) {
                ctx.beginPath()
                ctx.strokeStyle = root.curBgColor;
                ctx.fillStyle = root.curBgColor
                ctx.roundedRect(realX, realY, realWidth, realHeight, root.radius, root.radius);
                ctx.fill();
            }

            if (root.stroke) {
                ctx.beginPath()
                ctx.strokeStyle = root.curFgColor;
                ctx.moveTo(realX + (realWidth / 4), realY + (realHeight / 4) );
                ctx.lineTo(realX + (realWidth * 3 / 4), realY + (realHeight * 3 / 4));
                ctx.stroke()
                ctx.moveTo(realX + (realWidth / 4), realY + (realHeight * 3 / 4));
                ctx.lineTo(realX + (realWidth * 3 / 4), realY + (realHeight / 4));
                ctx.stroke()
            }

            ctx.restore();
        }
    }
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            parent.clicked()
            root.focus = true
        }
    }
}
