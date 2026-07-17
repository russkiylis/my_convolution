import QtQuick 2.15
import QtQuick.Controls 2.15

Item {

    Rectangle {
        id: generationField
        width: parent.width - 40
        height: parent.height - generationToggleButton.height - 60
        // height: 100
        border.color: main.borderColor
        color: main.backgroundColor

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: 20
        anchors.leftMargin: 20
    }

    Button {
        id: generationToggleButton
        width: 250
        anchors.right: generationField.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20

        // text: {
        //     if (connectionBackend.dbStatus === 2) return "Отключиться"
        //     return "Подключиться"
        // }
        text : generatorBackend.generatorEnabled ? "Остановить генератор нагрузки" : "Запустить генератор нагрузки"

        onClicked: generatorBackend.onGeneratorEnabledButtonClicked()
    }

}
