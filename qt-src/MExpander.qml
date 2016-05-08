import QtQuick 2.0
import QtQuick.Layouts 1.1
import "."

Item {
    id: m_root

    property alias title: m_header_text.text

    width: m_header.width
    height: m_header.height

    ColumnLayout {
        Rectangle {
            id: m_header
            color: Style.ui_component_bg
            border.width: Style.ui_border_width
            border.color: Style.ui_border_color
            width: m_header_text.implicitWidth + (2 * Style.h_padding)
            height: m_header_text.implicitHeight + (2 * Style.v_padding)
            MText {
                id: m_header_text
            }
        }
        Rectangle {
            id: m_body
            color: Style.ui_form_bg

        }
    }
}
