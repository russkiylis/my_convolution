import QtQuick 2.15
import QtQuick.Controls 2.15

Item {

    Rectangle {
        id: saveParamsField
        width: parent.width - 40
        // height: connectionGrid.height + 80
        height: parent.height - saveToggleButton.height - 60
        border.color: main.borderColor
        color: main.backgroundColor
        radius: 5

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: 20
        anchors.leftMargin: 20

        ComboBox {
            id: compressComboBox
            background: TextFieldBackground {
            }
            width: 225
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: 10
            anchors.topMargin: 40
            currentIndex: saveBackend.currentDataType

            onActivated: {
                saveBackend.currentDataType = currentIndex
            }

            model: [
                "double precision (8 байт)",
                "smallint (2 байта)",
                "real (4 байта)"
            ]

            Label {
                text: "Тип данных свёрток"
                anchors.bottom: parent.top
                anchors.bottomMargin: 5
                color: main.textColor
            }
        }
    }

    Button {
        id: saveToggleButton
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.rightMargin: 20
        background: ButtonBackground {
        }

        // TODO: Добавить адекватный текст
        // text: generatorBackend.generatorEnabled ? "Остановить генератор нагрузки" : "Запустить генератор нагрузки"
        text: "мяу"

        // TODO: Добавить адекватную обработку кликов
        // onClicked: generatorBackend.onGeneratorEnabledButtonClicked()
    }
}
