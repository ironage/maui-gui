import QtQuick 2.4

Rectangle {
    property alias mouseArea: mouseArea

    anchors.fill: parent
    color: "#1c1c1e"

    MouseArea {
        id: mouseArea
        anchors.fill: parent
    }

    Text {
        anchors.centerIn: parent
        text: "Hello World"
    }
}
