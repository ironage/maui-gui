import QtQuick 2.0
import QtQuick.Layouts 1.1
import "."

Item {
    id: m_root

    property alias title: m_header_text.text
    property alias payload: m_loader.sourceComponent

    width: m_header.width
    height: m_header.height

    ColumnLayout {
        Rectangle {
            id: m_header
            color: Style.ui_component_bg
            border.width: Style.ui_border_width
            border.color: Style.ui_border_color
            Layout.minimumHeight: 20
            Layout.minimumWidth: 30
            width: m_header_text.implicitWidth + (2 * Style.h_padding)
            height: m_header_text.implicitHeight + (2 * Style.v_padding)
            MText {
                id: m_header_text
            }
        }
        Rectangle {
            id: m_body
            color: Style.ui_form_bg2
            width: m_header.width
            height: m_loader.height
            anchors.top: m_header.bottom
            Loader {
                id: m_loader
//                anchors.centerIn: parent
                anchors.bottom: m_body.bottom
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
}
