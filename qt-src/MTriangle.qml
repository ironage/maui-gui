import QtQuick 2.5
// Modified from http://clogwog.net/wp/2015/02/a-simple-qml-triangle/ with full permission

Canvas {
    id: triangle
    antialiasing: true

    property int triangle_width: 60
    property int triangle_height: 60
    property color stroke_color:  "#ffffff"
    property color fill_color: "#ffffff"
    property color fill_color_highlight: fill_color
    property color stroke_color_highlight: stroke_color
    property color cur_fill_color: fill_color
    property color cur_stroke_color: stroke_color
    property int line_width: 3
    property bool fill: true
    property bool stroke: false
    property real alpha: 0.5
    property int xOffset: 0
    property int yOffset: 0
    width: triangle_width + (2 * line_width)
    height: width

    states: [
        State {
            name: ""    // default
            PropertyChanges { target: triangle; cur_stroke_color: triangle.stroke_color }
            PropertyChanges { target: triangle; cur_fill_color: triangle.fill_color }
        },
        State {
            name: "pressed"; when: m_area.containsMouse && !m_area.containsPress
            PropertyChanges { target: triangle; fill: true; }
        },
        State {
            name: "hover"
            PropertyChanges { target: triangle; cur_stroke_color: triangle.stroke_color_highlight }
            PropertyChanges { target: triangle; cur_fill_color: triangle.fill_color_highlight }
            when: ma1.containsMouse
        }
    ]
//    transitions: [
//           Transition {
//               from: "*"; to: "*"
//               ColorAnimation { target: triangle; properties: "stroke_color"; duration: 1000 }
//               ColorAnimation { target: triangle; properties: "fill_color"; duration: 1000 }
//           }
//       ]

    onCur_fill_colorChanged: requestPaint()
    onCur_stroke_colorChanged: requestPaint()
    onLine_widthChanged:requestPaint();
    onFillChanged:requestPaint();
    onStrokeChanged:requestPaint();

    signal clicked()

    onPaint: {
        var ctx = getContext("2d");
        ctx.save();
        ctx.clearRect(xOffset, yOffset, triangle.width, triangle.height);
        ctx.strokeStyle = triangle.cur_stroke_color;
        ctx.lineWidth = triangle.line_width
        ctx.fillStyle = triangle.cur_fill_color
        ctx.globalAlpha = triangle.alpha
        ctx.lineJoin = "round";
        ctx.beginPath();

        // put rectangle in the middle
        ctx.translate( (0.5 * width - 0.5 * triangle_width),
                       (0.5 * height - 0.5 * triangle_height))

        // draw the rectangle
        ctx.moveTo(xOffset,yOffset + (triangle_height/2) ); // left point of triangle
        ctx.lineTo(triangle_width, yOffset);
        ctx.lineTo(triangle_width, triangle_height + yOffset);

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
