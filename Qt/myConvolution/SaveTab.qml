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
    }
}
