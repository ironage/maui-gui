import QtQuick 2.4
// Modified from http://clogwog.net/wp/2015/02/a-simple-qml-triangle/ with full permission

Canvas {
    id: triangle
    antialiasing: true

    property int triangle_width: 60
    property int triangle_height: 60
    property color stroke_color:  "#ffffff"
    property color fill_color: "#ffffff"
    property int line_width: 3
    property bool fill: true
    property bool stroke: true
    property real alpha: 0.5
    width: triangle_width + (2 * line_width)
    height: width + (2 * line_width)
    states: [
        State {
            name: "pressed"; when: ma1.pressed
            PropertyChanges { target: triangle; fill: true; }
        }
    ]

    onLine_widthChanged:requestPaint();
    onFillChanged:requestPaint();
    onStrokeChanged:requestPaint();

    signal clicked()

    onPaint: {
        var ctx = getContext("2d");
        ctx.save();
        ctx.clearRect(0,0,triangle.width, triangle.height);
        ctx.strokeStyle = triangle.stroke_color;
        ctx.lineWidth = triangle.line_width
        ctx.fillStyle = triangle.fill_color
        ctx.globalAlpha = triangle.alpha
        ctx.lineJoin = "round";
        ctx.beginPath();

        // put rectangle in the middle
        ctx.translate( (0.5 *width - 0.5*triangle_width),
                      (0.5 * height - 0.5 * triangle_height))

        // draw the rectangle
        ctx.moveTo(0,triangle_height/2 ); // left point of triangle
        ctx.lineTo(triangle_width, 0);
        ctx.lineTo(triangle_width,triangle_height);

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
        onClicked: parent.clicked()
    }
}
