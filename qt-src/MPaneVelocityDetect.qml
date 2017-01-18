import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import "." // Custom Style

Item {
    id: root
    height: scaleInput.height + header.height + (2 * Style.v_padding)
    Rectangle {
        id: background
        border.width: Style.border_width
        border.color: Style.ui_border_color
        color: Style.ui_form_bg2
        anchors.fill: parent

        RowLayout {
            id: row
            anchors.top: header.bottom
            anchors.bottom: background.bottom
            anchors.left: background.left
            anchors.right: background.right
            anchors.leftMargin: Style.h_padding
            anchors.rightMargin: Style.h_padding

            MTextInput {
                id: scaleInput
                text: "1"
                width: 128
                height: implicitHeight
                placeholderText: "Scale"
                borderColor: acceptableInput ? Style.ui_component_highlight : Style.ui_color_light_red
                validator: DoubleValidator{bottom: 0.0001; top: 999.0; decimals: 4; notation: DoubleValidator.StandardNotation}
                horizontalAlignment: TextInput.AlignHCenter
            }
            MCombobox {
                id: scaleUnits
                width: 50
                model: ListModel {
                    id: cbItems
                    ListElement { text: "cm"; }
                    ListElement { text: "mm"; }
                    ListElement { text: "in"; }
                }
            }
        }
        MButton {
            id: header
            border.width: Style.border_width
            border.color: Style.ui_border_color
            enabled: false
            Layout.minimumHeight: 20
            Layout.minimumWidth: 30
            width: root.width
            h_padding: 5
            v_padding: 5
            text: "Velocity Detection"
        }
        MCheckbox {
            id: enabledCheckbox
            text: ""
            checkboxWidth: header.height - (2 * Style.border_width)
            checkboxHeight: checkboxWidth
            anchors.left: header.left
            anchors.leftMargin: Style.border_width
            anchors.verticalCenter: header.verticalCenter
        }
    }
    Rectangle {
        id: enabledCover
        anchors.fill: parent
        color: Style.ui_color_light_grey
        opacity: 0.8
        visible: !parent.enabled
    }
}
