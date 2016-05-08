import QtQuick 2.0
import "."

Item {
    id: m_root
    property alias text: m_text.text

    signal clicked();

    width: m_text.implicitWidth + (2 * Style.h_padding)
    height: m_text.implicitHeight + (2 * Style.v_padding)

    Rectangle {
        id: m_rect
        radius: 1
        border.width: 0
        anchors.fill: parent
        color: Style.ui_component_bg

        MText {
            id: m_text
        }
        MouseArea {
            id: m_area
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                m_root.clicked()
            }
        }
    }
    states: [
        State {
            name: ""    // default
            PropertyChanges { target: m_rect; color: Style.ui_component_bg}
        },
        State {
            name: "pressed"
            PropertyChanges { target: m_rect; color: Style.ui_component_selected}
            when: m_area.containsPress
        },
        State {
            name: "hover"
            PropertyChanges { target: m_rect; color: Style.ui_component_highlight}
            when: m_area.containsMouse && !m_area.containsPress
        }
    ]
}
