import QtQuick 2.5
// Modified from http://clogwog.net/wp/2015/02/a-simple-qml-triangle/ with full permission

Canvas {
    id: triangle
    antialiasing: true

    property int triangleWidth: 60
    property int triangleHeight: 60
    property color curColor: "#ffffff"
    property int lineWidth: 3
    property bool fill: true
    property bool stroke: false
    property real alpha: 0.5
    property int xOffset: 0
    property int yOffset: 0
    property bool activeHover: ma1.containsMouse

    width: triangleWidth + (2 * lineWidth)
    height: width

    onCurColorChanged: requestPaint()
    onLineWidthChanged: requestPaint();
    onFillChanged: requestPaint();
    onStrokeChanged: requestPaint();

    signal clicked()

    onPaint: {
        var ctx = getContext("2d");
        ctx.save();
        ctx.clearRect(xOffset, yOffset, triangle.width, triangle.height);
        ctx.strokeStyle = triangle.curColor;
        ctx.lineWidth = triangle.lineWidth
        ctx.fillStyle = triangle.curColor
        ctx.globalAlpha = triangle.alpha
        ctx.lineJoin = "round";
        ctx.beginPath();

        // put rectangle in the middle
        ctx.translate( (0.5 * width - 0.5 * triangleWidth),
                       (0.5 * height - 0.5 * triangleHeight))

        // draw the rectangle
        ctx.moveTo(xOffset,yOffset + (triangleHeight/2) ); // left point of triangle
        ctx.lineTo(triangleWidth, yOffset);
        ctx.lineTo(triangleWidth, triangleHeight + yOffset);

        ctx.closePath();
        if (triangle.fill)
            ctx.fill();
        if (triangle.stroke)
            ctx.stroke();
        ctx.restore();
    }
    MouseArea {
        id: ma1
        anchors.fill: parent
        hoverEnabled: true
        onClicked: parent.clicked()
    }
}
