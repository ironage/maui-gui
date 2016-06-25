import QtQuick 2.5

Canvas {
    id: line
    antialiasing: true

    property int x1: 0
    property int x2: 0
    property int y1: 0
    property int y2: 0

    property color strokeColor:  "#ffffff"
    property color fillColor: "#ffffff"
    property color curFillColor: fillColor
    property color curStrokeColor: strokeColor
    property int lineWidth: 2
    property bool fill: true
    property bool stroke: true
    property real alpha: 0.5
    x: Math.min(x1, x2) - lineWidth
    y: Math.min(y1, y2) - lineWidth
    width: Math.abs(x2 - x1) + lineWidth
    height: Math.abs(y2 - y1) + lineWidth

    onCurFillColorChanged: requestPaint()
    onCurStrokeColorChanged: requestPaint()
    onLineWidthChanged:requestPaint();
    onXChanged: requestPaint()
    onYChanged: requestPaint()
    onWidthChanged: requestPaint()
    onHeightChanged: requestPaint()
    onX1Changed: requestPaint()
    onY1Changed: requestPaint()
    onX2Changed: requestPaint()
    onY2Changed: requestPaint()

    onPaint: {
        var ctx = getContext("2d");
        ctx.save();
        ctx.clearRect(0, 0, line.width, line.height);
        ctx.strokeStyle = line.curStrokeColor;
        ctx.lineWidth = line.lineWidth
        ctx.lineJoin = "round";
        ctx.beginPath();

        ctx.moveTo(x1 - x, y1 - y);
        ctx.lineTo(x2 - x, y2 - y);

        ctx.stroke();
        ctx.restore();
    }
}
