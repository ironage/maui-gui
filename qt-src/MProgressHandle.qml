import QtQuick 2.5
// Modified from http://clogwog.net/wp/2015/02/a-simple-qml-triangle/ with full permission
// This is very similar to MTriangle (but different pattern of triangle)
// This may look like a code clone, but the benefit of this separation is that
// it gives flexibility to customize in the future.

Canvas {
    id: triangle
    antialiasing: true

    property int triangle_width: 60
    property int triangle_height: 60
    property color curColor:  "#ffffff"
    property int line_width: 3
    property bool fill: true
    property bool stroke: false
    property real alpha: 0.5
    property int xOffset: 0
    property int yOffset: 0
    width: triangle_width + (2 * line_width)
    height: triangle_height + (2 * line_width)

    onCurColorChanged: requestPaint()
    onLine_widthChanged:requestPaint();
    onFillChanged:requestPaint();
    onStrokeChanged:requestPaint();

    signal clicked()

    onPaint: {
        var ctx = getContext("2d");
        ctx.save();
        ctx.clearRect(xOffset, yOffset, triangle.width, triangle.height);
        ctx.strokeStyle = triangle.curColor;
        ctx.lineWidth = triangle.line_width
        ctx.fillStyle = triangle.curColor
        ctx.globalAlpha = triangle.alpha
        ctx.lineJoin = "round";
        ctx.beginPath();

        // put rectangle in the middle
        ctx.translate( (0.5 * width - 0.5 * triangle_width),
                       (0.5 * height - 0.5 * triangle_height))

        // draw the rectangle
        ctx.moveTo(xOffset + triangle_width / 2, yOffset); // top point of triangle
        ctx.lineTo(triangle_width, yOffset + triangle_height);
        ctx.lineTo(0, yOffset + triangle_height);

        ctx.closePath();
        if (triangle.fill)
            ctx.fill();
        if (triangle.stroke)
            ctx.stroke();
        ctx.restore();
    }
    MouseArea{
        id: ma1
        anchors.fill: parent
        hoverEnabled: true
        onClicked: parent.clicked()
    }
}
