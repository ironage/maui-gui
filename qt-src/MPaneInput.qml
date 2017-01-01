import QtQuick 2.2
import QtQuick.Layouts 1.1
import "." // Custom Style

Item {
    id: root
    ListModel {
        id: contacts
        ListElement {
            name: "name1.avi"
            path: "path/to/video/"
        }
        ListElement {
            name: "name2.avi"
            path: "path/to/video2/"
        }
    }
    Rectangle {
        id: background
        border.width: Style.border_width
        border.color: Style.ui_border_color
        color: Style.ui_form_bg2
        anchors.fill: parent
        ListView {
            anchors.top: header.bottom
            anchors.left: header.left
            anchors.right: header.right
            anchors.bottom: add.top
            anchors.leftMargin: Style.border_width
            anchors.rightMargin: Style.border_width
            Component {
                id: contactsDelegate
                Rectangle {
                    id: wrapper
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: contactInfo.height
                    border.width: Style.border_width
                    border.color: Style.ui_border_color
                    color: Style.ui_color_light_grey
                    //color: ListView.isCurrentItem ? "black" : "red"
                    MText {
                        id: contactInfo
                        text: name
                        //color: wrapper.ListView.isCurrentItem ? "red" : "black"
                    }
                }
            }
            model: contacts
            delegate: contactsDelegate
            focus: true
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
            text: "Data To Process"
        }
        MButton {
            id: add
            border.width: Style.border_width
            border.color: Style.ui_border_color
            Layout.minimumHeight: 20
            Layout.minimumWidth: 30
            width: root.width
            height: 20
            anchors.bottom: parent.bottom
            h_padding: 5
            v_padding: 0
            text: "+"
        }
    }
}
