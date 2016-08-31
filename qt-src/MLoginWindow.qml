import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import "." // Style

Window {
    id: windowRoot

    color: Style.ui_form_bg
    width: layout.childrenRect.width + (Style.h_padding * 2)
    height: layout.height + (Style.v_padding * 2)
    minimumHeight: layout.height + (Style.v_padding * 2)
    minimumWidth: 300
    modality: Qt.WindowModal
    flags: Qt.Dialog | Qt.CustomizeWindowHint | Qt.WindowTitleHint | Qt.WindowCloseButtonHint & (~Qt.WindowContextHelpButtonHint)
    title: "Login"
    property int textSize: 11

    onVisibilityChanged: {
        if (visibility) {
            usernameInput.forceActiveFocus()
        }
    }

    signal verifyAccount(string username, string password)

    function setMessage(newMessage) {
        message.text = newMessage
    }

    function preset(username, password) {
        usernameInput.text = username
        passwordInput.text = password
        message.text = ""
    }
    function finish() {
        if (usernameInput.text.length <= 0) {
            message.text = "Email field cannot be blank!"
        } else if (passwordInput.text.length <= 0) {
            message.text = "Password field cannot be blank!"
        } else {
            verifyAccount(usernameInput.text, passwordInput.text)
            windowRoot.close()
        }
    }

    ColumnLayout {
        id: layout
        anchors.centerIn: parent
        spacing: 12

        Text {
            id: message
            Layout.alignment: Qt.AlignLeft
            text: ""
            font.pointSize: 12
        }
        Item {
            width: 2
            height: Style.v_padding
        }
        Text {
            Layout.alignment: Qt.AlignLeft
            text: "Email:"
            font.pointSize: textSize
        }
        MTextInput {
            id: usernameInput
            text: ""
            width: 250
            onAccepted: {
                finish()
            }
            placeholderText: "Email"
            borderColor: text.length > 0 ? Style.ui_component_highlight : Style.ui_color_light_red
            horizontalAlignment: TextInput.AlignLeft
            Layout.alignment: Qt.AlignLeft
        }
        Text {
            Layout.alignment: Qt.AlignLeft
            text: "Password:"
            font.pointSize: textSize
        }
        MTextInput {
            id: passwordInput
            text: ""
            width: 250
            onAccepted: {
                finish()
            }
            echoMode: TextInput.Password
            placeholderText: "Password"
            borderColor: text.length > 0 ? Style.ui_component_highlight : Style.ui_color_light_red
            horizontalAlignment: TextInput.AlignLeft
            Layout.alignment: Qt.AlignLeft
        }
        Button {
            text: "OK"
            Layout.alignment: Qt.AlignHCenter
            onClicked: {
                finish()
            }
        }
        Item {
            width: 2
            height: Style.v_padding
        }
    }
}
