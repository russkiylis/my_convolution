import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

// Окошко с интерфейсом подключения к БД
Item {

    Rectangle {
        id: dbParamsField
        width: parent.width - 40
        // height: connectionGrid.height + 80
        height: parent.height - dbConnectionButton.height - 60
        border.color: main.borderColor
        color: main.backgroundColor
        radius: 5

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: 20
        anchors.leftMargin: 20

        GridLayout {
            id: connectionGrid
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: 10
            anchors.topMargin: 40
            width: parent.width - 20

            columns: 2
            rowSpacing: 40
            columnSpacing: 100

            TextField {
                id: dbHostName
                Layout.fillWidth: true

                placeholderText: "127.0.0.1"
                placeholderTextColor: main.textColorFieldInactive
                text: connectionBackend.hostName
                background: TextFieldBackground {}
                selectByMouse: true

                Label {
                    anchors.bottom: parent.top
                    anchors.bottomMargin: 5
                    text: "Адрес хоста:"
                    color: main.textColor
                }
            }
            TextField {
                id: dbPort
                Layout.fillWidth: true

                placeholderText: "5432"
                placeholderTextColor: main.textColorFieldInactive
                text: connectionBackend.port
                background: TextFieldBackground {}
                selectByMouse: true

                Label {
                    anchors.bottom: parent.top
                    anchors.bottomMargin: 5
                    text: "Порт:"
                    color: main.textColor
                }
            }
            TextField {
                id: dbUsername
                Layout.fillWidth: true

                placeholderText: "russkiylis"
                placeholderTextColor: main.textColorFieldInactive
                text: connectionBackend.userName
                background: TextFieldBackground {}
                selectByMouse: true

                Label {
                    anchors.bottom: parent.top
                    anchors.bottomMargin: 5
                    text: "Имя пользователя:"
                    color: main.textColor
                }
            }
            TextField {
                id: dbPassword
                Layout.fillWidth: true

                placeholderText: "Введите пароль"
                placeholderTextColor: main.textColorFieldInactive
                echoMode: TextInput.Password
                passwordCharacter: "*"
                passwordMaskDelay: 1000
                text: connectionBackend.password
                background: TextFieldBackground {}
                selectByMouse: true

                Label {
                    anchors.bottom: parent.top
                    anchors.bottomMargin: 5
                    text: "Пароль:"
                    color: main.textColor

                }
            }
            TextField {
                id: dbName
                Layout.fillWidth: true

                placeholderText: "my_convolution"
                placeholderTextColor: main.textColorFieldInactive
                text: connectionBackend.databaseName
                background: TextFieldBackground {}
                selectByMouse: true

                Label {
                    anchors.bottom: parent.top
                    anchors.bottomMargin: 5
                    text: "Имя базы данных:"
                    color: main.textColor
                }
            }
            TextField {
                id: dbConnectOptions
                Layout.fillWidth: true

                placeholderText: "connect_timeout = 3"
                placeholderTextColor: main.textColorFieldInactive
                text: connectionBackend.connectOptions
                background: TextFieldBackground {}
                selectByMouse: true

                Label {
                    anchors.bottom: parent.top
                    anchors.bottomMargin: 5
                    text: "Настройки подключения:"
                    color: main.textColor
                }
            }
        }
    }

    Button {
        id: dbConnectionButton
        width: 125
        anchors.right: dbParamsField.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20

        enabled: {
            return connectionBackend.dbStatus !== 1;
        }
        background: ButtonBackground {}

        text: {
            if (connectionBackend.dbStatus === 2) return "Отключиться"
            return "Подключиться"
        }
        onClicked: connectionBackend.onDbConnectionButtonClicked(
            dbHostName.text,
            dbPort.text,
            dbUsername.text,
            dbPassword.text,
            dbName.text,
            dbConnectOptions.text
        ) // QML видит Q_INVOKABLE метод подключённого класса
    }

    TextEdit {
        id: dbErrorText
        width: dbParamsField.width - dbConnectionButton.width - 10

        anchors.top: dbParamsField.bottom
        anchors.left: dbParamsField.left
        anchors.topMargin: 10

        text: connectionBackend.lastError

        readOnly: true
        selectByMouse: true
        selectByKeyboard: true

        wrapMode: TextEdit.Wrap

        color: main.textColor
    }

}
