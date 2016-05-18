import QtQuick 2.0
pragma Singleton

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
    property color ui_red_color: "#C02E1D"

    property int h_padding: 10
    property int v_padding: 10
}
