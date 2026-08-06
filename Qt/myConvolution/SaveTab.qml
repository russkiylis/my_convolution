import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {

    Rectangle {
        id: saveParamsField
        width: parent.width - 40
        // height: connectionGrid.height + 80
        height: parent.height - saveToggleButton.height - 90
        border.color: main.borderColor
        color: main.backgroundColor
        radius: 5

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: 20
        anchors.leftMargin: 20

        ColumnLayout {
            spacing: 20
            width: 225
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: 20
            anchors.topMargin: 40

            ComboBox {
                id: compressComboBox
                background: TextFieldBackground {
                }
                Layout.fillWidth: true
                currentIndex: saveBackend.currentDataType

                onActivated: {
                    saveBackend.currentDataType = currentIndex
                }

                model: [
                    "double precision (8 байт)",
                    "real (4 байта)",
                    "smallint (2 байта)"
                ]

                Label {
                    text: "Тип данных свёрток"
                    anchors.bottom: parent.top
                    anchors.bottomMargin: 5
                    color: main.textColor
                }
            }

            Item {
                implicitHeight: byteOrderLabel.height + byteOrderComboBox.height
                ComboBox {
                    id: byteOrderComboBox
                    background: TextFieldBackground {
                    }
                    Layout.fillWidth: true
                    currentIndex: saveBackend.currentByteOrder

                    onActivated: {
                        saveBackend.currentByteOrder = currentIndex
                    }

                    model: [
                        "Little endian",
                        "Big endian"
                    ]

                    anchors.bottom: parent.bottom
                }

                Label {
                    id: byteOrderLabel
                    text: "Порядок байтов"
                    anchors.bottom: byteOrderComboBox.top
                    anchors.bottomMargin: 5
                    color: main.textColor
                }
            }

            Button {
                id: tableClearButton
                Layout.fillWidth: true
                background: ButtonBackground {
                }

                text: "Очистить таблицы"

                onClicked: eraseDataDialog.open()

                enabled: connectionBackend.dbStatus !== 0
            }

            Button {
                id: tableRecreateButton
                Layout.fillWidth: true
                background: ButtonBackground {
                }
                text: "Пересоздать таблицы"

                onClicked: recreateTableDialog.open()

                enabled: connectionBackend.dbStatus !== 0
            }

            Button {
                id: tableDeleteButton
                Layout.fillWidth: true
                background: ButtonBackground {
                }

                text: "Удалить таблицы"

                onClicked: deleteTableDialog.open()

                enabled: connectionBackend.dbStatus !== 0
            }

            Label {
                Layout.fillWidth: true
                text: "Подсказка: если появляются ошибки, связанные с отстутствием таблицы/поля, попробуйте пересоздать таблицы."
            }

        }
    }

    Button {
        id: saveToggleButton
        width: 200
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.rightMargin: 20
        background: ButtonBackground {
        }

        text: saveBackend.saveEnabled ? "Выключить сохранение в БД" : "Включить сохранение в БД"

        onClicked: saveBackend.onSaveEnableButtonClicked()

        enabled: connectionBackend.dbStatus !== 0

        onEnabledChanged: {
            if (!enabled) {
                saveBackend.saveEnabled = false
            }
        }
    }

    TextEdit {
        id: dbErrorText
        width: saveParamsField.width - saveToggleButton.width - 10

        anchors.top: saveParamsField.bottom
        anchors.left: saveParamsField.left
        anchors.topMargin: 10

        text: connectionBackend.lastError

        readOnly: true
        selectByMouse: true
        selectByKeyboard: true

        wrapMode: TextEdit.Wrap

        color: main.textColor
    }

    Dialog {
        id: eraseDataDialog
        title: "Потенциально опасное действие"
        modal: true
        anchors.centerIn: parent

        background: Rectangle {
            color: main.backgroundColor
            border.color: main.borderColor
        }

        footer: DialogButtonBox {
            background: Item {
            }

            delegate: Button {
                id: btn
                background: ButtonBackground {}
            }
        }

        standardButtons: Dialog.Ok | Dialog.Cancel
        Label {
            text: "Это действие сотрёт все записанные данные. \nУверены, что хотите продолжить?"
            // wrapMode: Text.WordWrap
        }

        onAccepted: {
            saveBackend.onClearTableButtonClicked()
        }
    }

    Dialog {
        id: recreateTableDialog
        title: "Потенциально опасное действие"
        modal: true
        anchors.centerIn: parent

        background: Rectangle {
            color: main.backgroundColor
            border.color: main.borderColor
        }

        footer: DialogButtonBox {
            background: Item {
            }

            delegate: Button {
                id: btn
                background: ButtonBackground {}
            }
        }

        standardButtons: Dialog.Ok | Dialog.Cancel
        Label {
            text: "Пересоздание таблиц удалит все записанные в них данные. \nУверены, что хотите продолжить?"
            // wrapMode: Text.WordWrap
        }

        onAccepted: {
            saveBackend.onRecreateTableButtonClicked()
        }
    }

    Dialog {
        id: deleteTableDialog
        title: "Потенциально опасное действие"
        modal: true
        anchors.centerIn: parent

        background: Rectangle {
            color: main.backgroundColor
            border.color: main.borderColor
        }

        footer: DialogButtonBox {
            background: Item {
            }

            delegate: Button {
                id: btn
                background: ButtonBackground {}
            }
        }

        standardButtons: Dialog.Ok | Dialog.Cancel
        Label {
            text: "Удаление таблиц уничтожит все записанные в них данные.\nК тому же, пока таблицы не будут созданы вновь, сохранение будет приводить к ошибке.\nУверены, что хотите продолжить?"
            // wrapMode: Text.WordWrap
        }

        onAccepted: {
            saveBackend.onDeleteTableButtonClicked()
        }
    }
}


