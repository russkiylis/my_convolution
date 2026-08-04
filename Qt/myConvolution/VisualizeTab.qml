import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "sparkline.js" as SparklinePainter

// TODO: Добавить scrollbar и количество выведенных штук
Item {
    Button {
        id: readButton
        text: "Прочитать"
        width: 125
        anchors.right: tableField.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        background: ButtonBackground {
        }
        onClicked: visualizeBackend.readDb()
    }

    Rectangle {
        id: tableField
        border.color: main.borderColor2
        width: parent.width - 40
        height: parent.height - readButton.height - 90

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: 20
        anchors.leftMargin: 20

        HorizontalHeaderView {
            id: horizontalHeader
            height: 25
            anchors.left: tableView.left
            anchors.bottom: tableView.top
            syncView: tableView
            clip: true

            delegate: Rectangle {
                implicitHeight: 25
                implicitWidth: {
                    switch (column) {
                        case 0: return 100  // ID
                        case 1: return 175  // Временная метка
                        case 2: return 175  // Имя поста
                        case 3: return 100  // Азимут
                        case 4: return 100 // Угол места
                        case 5: return 100 // Мощность
                        case 6: return 100 // Частота
                        case 7: return 100 // Широта
                        case 8: return 100 // Долгота
                        case 9: return 175 // Качество (азимут)
                        case 11: return 200 // Свёртка (азимут)
                        case 10: return 175 // Качество (угол места)
                        case 12: return 200 // Свёртка (угол места)
                        default: return 300
                    }
                }
                border.color: main.borderColor2
                color: main.backgroundColor2

                Text {
                    text: model.display
                }
            }
        }

        TableView {
            id: tableView
            height: 500
            anchors.fill: parent
            clip: true
            anchors.topMargin: horizontalHeader.height
            anchors.leftMargin: 1
            anchors.rightMargin: 1
            anchors.bottomMargin: 1
            ScrollBar.vertical: ScrollBar {}

            model: visualizeBackend.visualizationTableModel
            onWidthChanged: tableView.forceLayout()

            delegate: Rectangle {
                id: tableDelegate
                implicitHeight: 25
                implicitWidth: {
                    switch (column) {
                        case 0: return 100  // ID
                        case 1: return 175  // Временная метка
                        case 2: return 175  // Имя поста
                        case 3: return 100  // Азимут
                        case 4: return 100 // Угол места
                        case 5: return 100 // Мощность
                        case 6: return 100 // Частота
                        case 7: return 100 // Широта
                        case 8: return 100 // Долгота
                        case 9: return 150 // Качество (азимут)
                        case 11: return 150 // Свёртка (азимут)
                        case 10: return 150 // Качество (угол места)
                        case 12: return 150 // Свёртка (угол места)
                        default: return 300
                    }
                }

                property var sparklineData: model.isSparkline ? model.sparkline : []
                Loader {
                    anchors.fill: parent
                    sourceComponent: model.isSparkline ? sparklineComponent : textComponent
                }
                Component {
                    id: textComponent
                    Text {
                        text: model.display
                    }
                }
                Component {
                    id: sparklineComponent
                    Canvas {
                        property var points: tableDelegate.sparklineData

                        onPointsChanged: requestPaint()

                        onPaint: {
                            let conv = model.sparkline;
                            let ctx = getContext("2d");
                            SparklinePainter.drawSparkline(ctx, width, height, conv);
                        }
                    }
                }

                border.color: main.borderColor2
            }
        }
    }

    TextEdit {
        id: amountText
        width: tableField - readButton.width - 10

        anchors.top: tableField.bottom
        anchors.left: tableField.left
        anchors.topMargin: 10

        text: "Отображено " + tableView.rows + " строк."

        readOnly: true
        selectByMouse: true
        selectByKeyboard: true

        wrapMode: TextEdit.Wrap

        color: main.textColor
    }
}
