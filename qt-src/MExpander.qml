import QtQuick 2.0
import QtQuick.Layouts 1.1
import "."

Item {
    id: m_root

    property alias title: m_header.text
    property alias payload: m_loader.sourceComponent
    property alias override_width: m_header.width
    //width: m_header.width
    height: m_header.height + m_body.height

    onHeightChanged: {
        //console.log("Expander " + title + " height: " + height + " header: " + m_header.height + " body: " + m_body.height + " state: " + m_root.state)
    }

    function close() {
        m_root.state = "collapsed"
    }
    function open() {
        m_root.state = "expanded"
    }

    signal opened()
    signal closed()

    ColumnLayout {
        MButton {
            id: m_header
            border.width: 2
            border.color: Style.ui_border_color
            Layout.minimumHeight: 20
            Layout.minimumWidth: 30
            h_padding: 5
            v_padding: 5
            onClicked: {
                if (m_root.state == "collapsed") {
                    m_root.state = "expanded"
                    opened()
                } else {
                    m_root.state = "collapsed"
                    closed()
                }
            }
        }

        Rectangle {
            id: m_body
            color: Style.ui_form_bg2
            width: m_header.width
            height: 0   // collapsed
            anchors.top: m_header.bottom
            clip: true
            Loader {
                id: m_loader
                //anchors.centerIn: parent
                anchors.bottom: m_body.bottom
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }

    state: "collapsed"
    states: [
        State {
            name: "collapsed"
            PropertyChanges { target: m_body; height: 0; }
        },
        State {
            name: "expanded"
            PropertyChanges { target: m_body; height: m_loader.height; }
        }
    ]
    transitions: [
        Transition {
            from: "expanded"
            to: "collapsed"
            PropertyAnimation { property: "height"; duration: 400; }
        },
        Transition {
            from: "collapsed"
            to: "expanded"
            PropertyAnimation { property: "height"; duration: 400; }
        }
    ]
}
