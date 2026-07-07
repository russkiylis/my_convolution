import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Window {
    property int defaultWidth: 640
    property int defaultHeight: 480

    width: defaultWidth
    height: defaultHeight
    minimumWidth: defaultWidth
    minimumHeight: defaultHeight
    visible: true
    title: qsTr("Hello World с помощью QML")

    ColumnLayout {
        id: rectangleColumn

        anchors.fill: parent
        anchors.margins: 15

        spacing: 20

        Rectangle {
            id: helloWorldBlock

            Layout.fillHeight: true
            Layout.fillWidth: true

            color: "#F0F0F0"
            border.color: "black"

            Text {
                id: helloWorldText
                text: "Hello Worldъ QML Editon"
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.margins: 5
            }

            Button {
                id: helloWorldButton
                text: "Нажми меня :("
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                anchors.margins: 5

                onClicked: {
                    helloWorldBackend.buttonClicked()
                }

            }

            TextField {
                id: helloWorldField

                width: parent.width - helloWorldButton.width - 15

                anchors.verticalCenter: helloWorldButton.verticalCenter
                anchors.right: parent.right
                anchors.margins: 5

                readOnly: true

                text: helloWorldBackend.helloWorldText
            }
        }

        Rectangle {
            id: calculatorBlock

            Layout.fillHeight: true
            Layout.fillWidth: true

            color: "#F0F0F0"
            border.color: "black"

            Text {
                id: calculatorBlockText
                text: "Калькуляторъ QML Editon"
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.margins: 5
            }

            SpinBox {
                id: calculatorValue1

                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.margins: 5

                from: -100
                to: 100

                editable: true

                onValueModified: calculatorBackend.firstSpinboxActivated(value)
            }

            ComboBox {
                id: calculatorOperationSelector

                anchors.bottom: parent.bottom
                anchors.verticalCenter: calculatorValue1.verticalCenter
                anchors.left: calculatorValue1.right
                anchors.margins: 5

                model: ["+", "-", "*", "/"]

                onActivated: calculatorBackend.comboboxActivated(currentIndex)
            }

            SpinBox {
                id: calculatorValue2

                anchors.bottom: parent.bottom
                anchors.verticalCenter: calculatorValue1.verticalCenter
                anchors.left: calculatorOperationSelector.right
                anchors.margins: 5

                from: -100
                to: 100

                editable: true

                onValueModified: calculatorBackend.secondSpinboxActivated(value)
            }

            TextField {
                id: calculatorResultField

                width: (
                    parent.width
                    - calculatorValue1.width
                    - calculatorOperationSelector.width
                    - calculatorValue2.width
                    - 25
                    )

                anchors.bottom: parent.bottom
                anchors.verticalCenter: calculatorValue1.verticalCenter
                anchors.left: calculatorValue2.right
                anchors.margins: 5

                readOnly: true

                text: calculatorBackend.result
            }
        }

        Rectangle {
            id: waveBlock

            Layout.fillHeight: true
            Layout.fillWidth: true

            color: "#F0F0F0"
            border.color: "black"

            Text {
                id: waveBlockText
                text: "Конвертеръ ''частота - период - длина волны'' QML Editon"
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.margins: 5
            }

            TextField {
                id: waveFrequencyField

                width: (parent.width - 20) / 3

                anchors.left: parent.left
                anchors.bottom: parent.bottom
                anchors.margins: 5

                placeholderText: "100000000"
            }

            Text {
                id: waveFrequencyText

                anchors.left: waveFrequencyField.left
                anchors.bottom: waveFrequencyField.top
                anchors.bottomMargin: 5

                text: "Частота, Гц"
            }

            TextField {
                id: wavePeriodField

                width: (parent.width - 20) / 3

                anchors.left: waveFrequencyField.right
                anchors.bottom: parent.bottom
                anchors.margins: 5

                placeholderText: "0.01"
            }

            Text {
                id: wavePeriodText

                anchors.left: wavePeriodField.left
                anchors.bottom: wavePeriodField.top
                anchors.bottomMargin: 5

                text: "Период, мкс"
            }

            TextField {
                id: waveLengthField

                width: (parent.width - 20) / 3

                anchors.left: wavePeriodField.right
                anchors.bottom: parent.bottom
                anchors.margins: 5

                placeholderText: "3"
            }

            Text {
                id: waveLengthText

                anchors.left: waveLengthField.left
                anchors.bottom: waveLengthField.top
                anchors.bottomMargin: 5

                text: "Длина волны, м"
            }
        }
    }
}
