import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import "."

TextField {
    id: textField
    height: 40
    width: 200

    property string borderColor: focus ? Style.ui_component_highlight : Style.ui_component_bg;
    property color backgroundColor: disabled ? "#f7f9f9" : "white";
    property int pointSize: 11;
    property bool disabled: false;
    property bool error: false;
    property bool success: false;

    MouseArea {
        anchors.fill: parent;
        enabled: textField.disabled;
    }

    placeholderText: "Input";
    style: TextFieldStyle {
        padding.left: 12;

        background: Rectangle {
            height: control.height;
            implicitWidth: control.width;
            color: control.backgroundColor;
            border {
                width: 2;
                color: control.borderColor;
            }
            radius: 4;
        }
    }
}
