import QtQuick 2.0
pragma Singleton

QtObject {
    property int titleAlignment: Text.AlignHCenter
    property int titleFontSize: 50
    property color titleColor: "green"

    property color ui_form_bg: "#EAEAEA"
    property color ui_component_bg: "#54606D"
    property color ui_component_selected: "#343942"
    property color ui_component_highlight: "#24B0B2"
    property color ui_text: "#eeeeee"
    property color ui_text_style: "#343434"
}
