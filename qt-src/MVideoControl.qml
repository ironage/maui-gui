import QtQuick 2.5
import QtQuick.Layouts 1.1
import "."

Item {
    id: m_root
    width: 10
    height: 10
    property double progress: 0

    signal setProgress(double percent)

    Rectangle {
        width: m_root.width
        height: m_root.height
        color: Style.ui_color_light_grey
        MouseArea {
            anchors.fill: parent
            onPositionChanged:  {
                if (containsPress)
                    updatePosition()
            }
            onReleased: {
                updatePosition()
            }
            function updatePosition() {
                setProgress(mouseX / m_root.width)
            }
        }
    }
    Rectangle {
        width: m_root.progress * m_root.width
        height: m_root.height
        color: Style.ui_color_light_green
    }
}
