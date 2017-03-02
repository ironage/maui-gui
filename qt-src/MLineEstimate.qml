import QtQuick 2.5
import "."

Canvas {
    id: line
    antialiasing: true

    property color strokeColor:  Style.ui_color_light_orange
    property color fillColor: Style.ui_color_light_red
    property color curFillColor: fillColor
    property color curStrokeColor: strokeColor
    property int lineWidth: 20
    property bool fill: true
    property bool stroke: true
    property real alpha: 0.6
    property var pointList
    x:0
    y:0
    width: parent.width
    height: parent.height

    onPointListChanged: {
        requestPaint()
    }

    onCurFillColorChanged: requestPaint()
    onCurStrokeColorChanged: requestPaint()
    onLineWidthChanged:requestPaint();
    onXChanged: requestPaint()
    onYChanged: requestPaint()
    onWidthChanged: requestPaint()
    onHeightChanged: requestPaint()

    onPaint: {
        var ctx = getContext("2d");
        ctx.save();
        ctx.clearRect(0, 0, line.width, line.height);
//        if (pointList.length >= 2) {
//            ctx.strokeStyle = line.curStrokeColor;
//            ctx.globalAlpha = line.alpha
//            ctx.lineWidth = line.lineWidth
//            ctx.lineJoin = "round"
//            ctx.lineCap = "round"
//            ctx.beginPath();
//            var i
//            for (i = 0; i < pointList.length - 1; i++) {
//                ctx.moveTo(pointList[i].x, pointList[i].y);
//                ctx.lineTo(pointList[i+1].x, pointList[i+1].y);
//            }
//            ctx.closePath();
//            ctx.stroke();
//        }

        if (pointList.length >= 3) {
            ctx.strokeStyle = line.curStrokeColor;
            ctx.globalAlpha = line.alpha
            ctx.lineWidth = line.lineWidth
            ctx.lineJoin = "round"
            ctx.lineCap = "round"
            ctx.beginPath();
            ctx.moveTo(pointList[0].x, pointList[0].y);
            var middleNdx = pointList.length >> 1
            ctx.quadraticCurveTo(pointList[middleNdx].x, pointList[middleNdx].y, pointList[pointList.length-1].x, pointList[pointList.length-1].y)
            //ctx.closePath();
            ctx.stroke();
        }
        ctx.restore();
    }
}
