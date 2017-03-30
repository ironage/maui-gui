import QtQuick 2.0
pragma Singleton

// Color swatches: http://designmodo.github.io/Flat-UI/

QtObject {
    property int titleAlignment: Text.AlignHCenter
    property int titleFontSize: 50
    property color titleColor: "green"

    property color ui_form_bg: "#EAEAEA"
    property color ui_form_bg2: "#AEAEAE"
    property color ui_component_bg: "#54606D"
    property color ui_component_selected: "#343942"
    property color ui_component_highlight: "#24B0B2"
    property color ui_text: "#eeeeee"
    property color ui_text_style: "#343434"
    property color ui_border_color: "#181818"
    property int ui_border_width: 2
    property color ui_color_bright_red: "#f75c4c"
    property color ui_color_light_red: "#e74c3c"
    property color ui_color_dark_red: "#c0392b"
    property color ui_color_light_grey: "#95a5a6"
    property color ui_color_dark_grey: "#7f8c8d"
    property color ui_color_light_green: "#2ecc71"
    property color ui_color_dark_green: "#27ae60"
    property color ui_color_light_dblue: "#34495e"
    property color ui_color_dark_dblue: "#2c3e50"
    property color ui_color_bright_lblue: "#54b8fb"
    property color ui_color_light_lblue: "#3498db"
    property color ui_color_dark_lblue: "#2980b9"
    property color ui_color_light_turquoise: "#1abc9c"
    property color ui_color_dark_turquoise: "#16a085"
    property color ui_color_light_orange: "#e67e22"
    property color ui_color_dark_orange: "#d35400"
    property color ui_color_light: "#eeeeee"
    property color ui_color_silver: "#bdc3c7"

    property int h_padding: 10
    property int v_padding: 10
    property int drag_threshold: 1
    property int border_width: 2
}
