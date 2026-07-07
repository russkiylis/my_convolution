import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15 // Button

// Основное окно программы
Window {
    width: 640
    height: 480
    visible: true
    title: "My Convolution"

    // Для большей красоты и удобства можно сделать контейнер - прямоугольник
    Rectangle {

        // Блок размещения
        anchors.fill: parent
        anchors.margins: 20

        // Блок внешнего вида
        color: "#F0F0F0"
        border.color: "black"

        // Кнопка запуска генерации нагрузки
        Button {

            // Блок размещения
            anchors.centerIn: parent

            // Блок внешнего вида
            text: "Сгенерировать нагрузку"

            // Блок логики
            onClicked: backend.onGenerationButtonClicked() // QML видит Q_INVOKABLE метод подключённого класса
        }
    }

}
