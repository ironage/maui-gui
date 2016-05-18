import QtQuick 2.0
import QtQuick.Layouts 1.1
import "."

Item {
    id: m_root
    width: 10
    height: 5
    property double progress: 0


    Rectangle {
        width: m_root.width
        height: m_root.height
        color: Style.ui_color_light_grey
    }
    Rectangle {
        width: m_root.progress * m_root.width
        height: m_root.height
        color: Style.ui_color_light_green
    }
}
