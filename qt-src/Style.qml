import QtQuick 2.0
pragma Singleton

QtObject {
    property int titleAlignment: Text.AlignHCenter
    property int titleFontSize: 50
    property color titleColor: "green"

    property color ui_form_bg: "#f1f1f5"
    property color ui_component_bg: "#010120"
    property color ui_component_selected: "#0101f0"
    property color ui_component_highlight: "#1010f0"
    property color ui_text: "#eeeeee"
}
