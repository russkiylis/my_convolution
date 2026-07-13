import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15 // Button
import QtQuick.Layouts 1.15

// Классные цвета
// #FFFDFA
// #FFEACA
// #FFD79A
// #FFC36A
// #FFB03A
// #FF9D0A
// #D98200
// #A96600
// #794900
// #492C00
// #1A0F00

// Основное окно программы
Window {
    id: main

    // Блок с переменными
    property string backgroundColor: "#FFFCFA"
    property string activeColor: "#FFEACA"
    property string borderColor: "#FFD79A"
    property string textColorInactive: "#FFB03A"
    property string textColor: "#1A0F00"
    property string textColorFieldInactive: "#B1B0AF"
    property string pressColor: "#FFF3E2"

    width: 640
    height: 480
    minimumWidth: 640
    minimumHeight: 480
    visible: true
    title: "My Convolution"

    // Для большей красоты и удобства можно сделать контейнер - прямоугольник
    Rectangle {

        // Блок размещения
        anchors.fill: parent
        anchors.margins: 20
        anchors.bottomMargin: 40
        anchors.topMargin: bar.height + 20

        // Блок внешнего вида
        color: backgroundColor
        border.color: main.borderColor

        // Табы сверху окошка (переключаемся между вкладками)
        TabBar {
            id: bar
            width: 500
            anchors.bottom: parent.top
            anchors.bottomMargin: -1

            TabButton {
                id: tabButtonConnect
                text: "Соединение"

                // Внешний вид кнопки
                contentItem: Text {
                    text: tabButtonConnect.text
                    font: tabButtonConnect.font
                    color: tabButtonConnect.checked ? main.textColor : main.textColorInactive
                    horizontalAlignment: Text.AlignHCenter
                }
                background: Rectangle {
                    color: {
                        if (tabButtonConnect.pressed) return main.pressColor
                        if (tabButtonConnect.checked) return main.activeColor
                        return main.backgroundColor
                    }
                    border.color: main.borderColor
                }
            }
            TabButton {
                id: tabButtonGen
                text: "Нагрузка"

                // Внешний вид кнопки
                contentItem: Text {
                    text: tabButtonGen.text
                    font: tabButtonGen.font
                    color: tabButtonGen.checked ? main.textColor : main.textColorInactive
                    horizontalAlignment: Text.AlignHCenter
                }
                background: Rectangle {
                    color: {
                        if (tabButtonGen.pressed) return main.pressColor
                        if (tabButtonGen.checked) return main.activeColor
                        return main.backgroundColor
                    }
                    border.color: main.borderColor
                }
            }
            TabButton {
                id: tabButtonSave
                text: "Сохранение"

                // Внешний вид кнопки
                contentItem: Text {
                    text: tabButtonSave.text
                    font: tabButtonSave.font
                    color: tabButtonSave.checked ? main.textColor : main.textColorInactive
                    horizontalAlignment: Text.AlignHCenter
                }
                background: Rectangle {
                    color: {
                        if (tabButtonSave.pressed) return main.pressColor
                        if (tabButtonSave.checked) return main.activeColor
                        return main.backgroundColor
                    }
                    border.color: main.borderColor
                }
            }
            TabButton {
                id: tabButtonVisualize
                text: "Отображение"

                // Внешний вид кнопки
                contentItem: Text {
                    text: tabButtonVisualize.text
                    font: tabButtonVisualize.font
                    color: tabButtonVisualize.checked ? main.textColor : main.textColorInactive
                    horizontalAlignment: Text.AlignHCenter
                }
                background: Rectangle {
                    color: {
                        if (tabButtonVisualize.pressed) return main.pressColor
                        if (tabButtonVisualize.checked) return main.activeColor
                        return main.backgroundColor
                    }
                    border.color: main.borderColor
                }
            }
        }

        // В одном окошке у нас будет разный интерфейс, в зависимости от выбранной вкладки
        StackLayout {
            anchors.fill: parent
            currentIndex: bar.currentIndex
            ConnectTab {
                id: connectTab
            }
            GenTab {
                id: genTab
            }
            SaveTab {
                id: saveTab
            }
            VisualizeTab {
                id: visualizeTab
            }
        }

        // Текст в нижнем правом углу. Показывает статус базы данных
        Label {
            padding: 10
            text: {
                if (backend.dbStatus === 0) return "Соединение с БД отсутствует";
                else if (backend.dbStatus === 1) return "БД занята";
                else return "БД готова";
            }

            color: {
                if (backend.dbStatus === 0) return "red";
                else if (backend.dbStatus === 1) return "#FF9900";
                else return "green";
            }

            anchors.top: parent.bottom
            anchors.right: parent.right
        }
    }
}
