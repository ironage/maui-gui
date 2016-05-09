import QtQuick 2.0
import "."

Text {
    color: Style.ui_text
    text: qsTr("Text")
    styleColor: Style.ui_text_style
    style: Text.Outline
    font.bold: true
    anchors.centerIn: parent
    font.family: "Arial"
    font.pixelSize: 14
}
