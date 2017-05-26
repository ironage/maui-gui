import QtQuick 2.5
import QtQuick.Layouts 1.1
import "." // Style

Item {
    id: root
    width: childrenRect.width
    height: childrenRect.height

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

    property int lineWidth: 3
    property bool fill: true
    property bool stroke: false
    property bool drawCheck: true
    property real alpha: 1
    property int xOffset: 0
    property int yOffset: 0
    property real radius: 5
    property alias checkboxWidth: canvas.width
    property alias checkboxHeight: canvas.height
    property alias middlePadding: middlePadding.width
    property alias text: mText.text
    property alias textBoxWidth: mText.width
    property alias textFont: mText.font
    property alias textFontSizeMode: mText.fontSizeMode
    property alias textMinPixelSize: mText.minimumPixelSize
    signal clicked()

    onCurBgColorChanged: canvas.requestPaint()
    onCurFgColorChanged: canvas.requestPaint()
    Row {
        Canvas {
            id: canvas
            antialiasing: true
            state: "checked"
            width: 20
            height: width

            states: [
                State {
                    name: "checked"    // default (checked)
                    when: root.checked && !mouseArea.containsMouse
                    PropertyChanges { target: root; curBgColor: root.checkedBgColor }
                    PropertyChanges { target: root; curFgColor: root.checkedFgColor }
                    PropertyChanges { target: root; drawCheck: true }
                },
                State {
                    name: "unchecked"; when: !root.checked && !mouseArea.containsMouse
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
                    when: root.checked && mouseArea.containsMouse
                    PropertyChanges { target: root; curBgColor: root.checkedHighlightBgColor }
                    PropertyChanges { target: root; curFgColor: root.checkedHighlightFgColor }
                    PropertyChanges { target: root; drawCheck: true }
                },
                State {
                    name: "hoverUnchecked"
                    when: !root.checked && mouseArea.containsMouse
                    PropertyChanges { target: root; curBgColor: root.uncheckedBgColor }
                    PropertyChanges { target: root; curFgColor: root.uncheckedFgColor }
                    PropertyChanges { target: root; drawCheck: true }
                }
            ]
            transitions: [
                Transition {
                    from: "*"; to: "*"
                    ColorAnimation { target: root; properties: "curBgColor"; duration: 250 }
                    ColorAnimation { target: root; properties: "curFgColor"; duration: 250 }
                }
            ]

            onStateChanged: requestPaint()

            onPaint: {
                var ctx = getContext("2d");
                ctx.save();
                ctx.clearRect(root.xOffset, root.yOffset, canvas.width, canvas.height);
                ctx.strokeStyle = root.curBgColor;
                ctx.lineWidth = root.lineWidth
                ctx.fillStyle = root.curBgColor
                ctx.globalAlpha = root.alpha
                ctx.lineJoin = "bevel";

                ctx.beginPath()
                ctx.roundedRect(root.xOffset, root.yOffset, canvas.width, canvas.height, root.radius, root.radius);
                ctx.fill();

                if (root.drawCheck) {
                    ctx.beginPath()
                    ctx.strokeStyle = root.curFgColor;
                    ctx.moveTo(root.xOffset + (canvas.width / 4), root.yOffset + (canvas.height / 2) );
                    ctx.lineTo(root.xOffset + (canvas.width * 3 / 7), root.yOffset + (canvas.height * 2 / 3));
                    ctx.lineTo(root.xOffset + (canvas.width * 3 / 4), root.yOffset + (canvas.height / 3));
                    ctx.stroke()
                }

                ctx.restore();
            }
        }
        Item {
            id: middlePadding
            height: 1
            width: Style.h_padding
        }
        MText {
            id: mText
            text: ""
            style: Text.Normal
            color: Style.ui_color_dark_dblue
            anchors.verticalCenter: canvas.verticalCenter
        }
    }
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            parent.clicked()
            root.checked = !root.checked
            root.focus = true
        }
    }
}
