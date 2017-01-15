import QtQuick 2.5
import "." // Style

Canvas {
    id: root
    antialiasing: true

    property color checkedBgColor: Style.ui_color_light_turquoise
    property color checkedFgColor: Style.ui_color_light
    property color uncheckedBgColor: Style.ui_color_dark_grey
    property color uncheckedFgColor: Style.ui_color_light
    property color disabledBgColor: Style.ui_color_light_grey
    property color disabledFgColor: Style.ui_color_light
    property color checkedHighlightBgColor: Style.ui_color_dark_turquoise
    property color checkedHighlightFgColor: Style.ui_color_light
    property color curBgColor: checkedBgColor
    property color curFgColor: checkedFgColor
    property bool checked: true
    property bool enabled: true

    property int lineWidth: 5
    property bool fill: true
    property bool stroke: false
    property bool drawCheck: true
    property real alpha: 1
    property int xOffset: 0
    property int yOffset: 0
    property real radius: 5
    state: "checked"

    width: 20
    height: width

    states: [
        State {
            name: "checked"    // default (checked)
            PropertyChanges { target: root; curBgColor: root.checkedBgColor }
            PropertyChanges { target: root; curFgColor: root.checkedFgColor }
            PropertyChanges { target: root; drawCheck: true }
        },
        State {
            name: "unchecked"; when: !root.checked && !root.enabled
            PropertyChanges { target: root; curBgColor: root.uncheckedBgColor }
            PropertyChanges { target: root; curFgColor: root.uncheckedFgColor }
            PropertyChanges { target: root; drawCheck: false }
        },
        State {
            name: "disabled";
            when: !root.enabled
            PropertyChanges { target: root; curBgColor: root.disabledBgColor }
            PropertyChanges { target: root; curFgColor: root.disabledFgColor }
        },
        State {
            name: "hoverChecked"
            when: mouseArea.containsMouse && root.checked
            PropertyChanges { target: root; curBgColor: root.checkedHighlightBgColor }
            PropertyChanges { target: root; curFgColor: root.checkedHighlightFgColor }
            PropertyChanges { target: root; drawCheck: true }
        },
        State {
            name: "hoverUnchecked"
            when: mouseArea.containsMouse && !root.checked
            PropertyChanges { target: root; curBgColor: root.checkedHighlightBgColor }
            PropertyChanges { target: root; curFgColor: root.checkedHighlightFgColor }
            PropertyChanges { target: root; drawCheck: true }
        }
    ]
    transitions: [
           Transition {
               from: "*"; to: "*"
               ColorAnimation { target: root; properties: "curBgColor"; duration: 500 }
               ColorAnimation { target: root; properties: "curFgColor"; duration: 500 }
           }
       ]

    onCurBgColorChanged: requestPaint()
    onCurFgColorChanged: requestPaint()

    signal clicked()

    onPaint: {
        var ctx = getContext("2d");
        ctx.save();
        ctx.clearRect(xOffset, yOffset, root.width, root.height);
        ctx.strokeStyle = root.curFgColor;
        ctx.lineWidth = root.lineWidth
        ctx.fillStyle = root.curBgColor
        ctx.globalAlpha = root.alpha
        //ctx.lineJoin = "round";

        ctx.roundedRect(xOffset, yOffset, root.width, root.height, root.radius, root.radius);
        ctx.fill();
        // put rectangle in the middle
        ctx.translate( (0.5 * width - 0.5 * root.width),
                       (0.5 * height - 0.5 * root.height))

        if (root.drawCheck) {
            ctx.beginPath()
            ctx.moveTo(xOffset + (root.width / 3), yOffset + (root.height / 2) );
            ctx.lineTo(xOffset + (root.width / 2), yOffset + (root.height * 2 / 3));
            ctx.lineTo(xOffset + (root.width * 2 / 3), yOffset + (root.height / 3));
            ctx.stroke()
        }

        //ctx.fill();
//        ctx.stroke();
        ctx.restore();
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            parent.clicked()
            root.checked = !root.checked
        }
    }
}
