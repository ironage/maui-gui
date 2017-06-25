import QtQuick 2.5
import QtQuick.Layouts 1.3
import "."

Item {
    id: m_root
    property alias text: m_text.text
    property alias textColor: m_text.color
    property alias textStyleColor: m_text.styleColor
    property alias textStyle: m_text.style
    property alias textFont: m_text.font

    property int h_padding: Style.h_padding
    property int v_padding: Style.v_padding
    property alias border: m_rect.border
    property color color: Style.ui_component_bg

    property color selected_color: Style.ui_component_selected
    property color highlight_color: Style.ui_component_highlight
    property color disabled_color: Style.ui_color_dark_grey

    property alias imageSource: image.source
    property alias imageSourceSize: image.sourceSize

    signal clicked();

    width: m_text.implicitWidth + image.width + (2 * h_padding)
    height: (m_text.implicitHeight > image.height ? m_text.implicitHeight : image.height) + (2 * v_padding)

    function enable() {
        m_rect.state = ""
        enabled = true
    }
    function disable() {
        m_rect.state = "disabled"
        enabled = false
    }

    Rectangle {
        id: m_rect
        radius: 1
        border.width: 0
        anchors.fill: parent
        color: m_root.color

        RowLayout {
            anchors.centerIn: parent
            Image {
                id: image
                smooth: true
                width: sourceSize.width
                height: sourceSize.height
            }

            MText {
                id: m_text
            }
        }
        MouseArea {
            id: m_area
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                m_root.clicked()
            }
        }
        states: [
            State {
                name: ""    // default
                PropertyChanges { target: m_rect; color: m_root.color }
            },
            State {
                name: "pressed"
                PropertyChanges { target: m_rect; color: m_root.selected_color }
                when: m_area.containsPress
            },
            State {
                name: "hover"
                PropertyChanges { target: m_rect; color: m_root.highlight_color}
                when: m_area.containsMouse && !m_area.containsPress
            },
            State {
                name: "disabled"
                PropertyChanges { target: m_rect; color: m_root.disabled_color }
            }

        ]
        transitions: [
               Transition {
                   from: "*"; to: "*"
                   ColorAnimation { target: m_rect; properties: "color"; duration: 100 }
               }
           ]
    }
}
