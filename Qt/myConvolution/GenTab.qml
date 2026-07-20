import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {

    Rectangle {
        id: postListField
        width: parent.width/4 - 30
        height: parent.height - generationToggleButton.height - 90
        // height: 100
        border.color: main.borderColor
        color: main.backgroundColor
        radius: 5

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: 20
        anchors.leftMargin: 20

        Text {
            visible: postListView.currentIndex < 0
            color: main.textColorInactive
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.margins: 10
            text: "Время добавить новый пост..."
        }

        Button {
            id: postAddButton
            text: "+"
            width: parent.width/2 - 5
            height: 25
            background: ButtonBackground {}

            anchors.top: parent.bottom
            anchors.left: parent.left
            anchors.topMargin: 5

            onClicked: {
                let newIndex = generatorBackend.postListModel.addPost()
                if (newIndex >= 0) {
                    postListView.currentIndex = newIndex
                    generatorBackend.postListModel.postUpdate()
                }
            }
        }
        Button {
            id: postDeleteButton
            text: "-"
            width: parent.width/2 - 5
            height: 25
            background: ButtonBackground {}

            anchors.top: parent.bottom
            anchors.left: postAddButton.right
            anchors.topMargin: 5
            anchors.leftMargin: 10

            onClicked: {
                let newIndex = generatorBackend.postListModel.removePost(postListView.currentIndex)
                postListView.currentIndex = newIndex
                generatorBackend.postListModel.postUpdate()
            }
        }

        ListView {
            id: postListView
            anchors.fill: parent
            clip: true
            spacing: 10
            anchors.margins: 5

            // 1. Модель данных
            model: generatorBackend.postListModel

            // 2. Делегат (шаблон отображения)
            delegate: ItemDelegate {
                width: postListView.width
                height: 60
                background: ListViewItemBackground {}
                highlighted: ListView.isCurrentItem

                onClicked: {
                    postListView.currentIndex = index
                    generatorBackend.postListModel.postUpdate()
                    generatorBackend.postListModel.setPostIndex(index)
                }

                Text {
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.margins: 10

                    text: model.postName // Обращение к роли postName из модели
                    font.bold: true
                    font.pixelSize: 16
                    color: model.enabled ? "black" : main.textColorInactive2
                }

                Text {
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.margins: 10
                    color: model.enabled ? "black" : main.textColorInactive2

                    text: model.enabled ? "Активен" : "Неактивен"
                }

                Text {
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    anchors.margins: 10
                    color: model.enabled ? "black" : main.textColorInactive2

                    text: "Период: " + model.minPeriod + " - " + model.maxPeriod + " мс"
                }

            }
        }
    }

    Rectangle {
        id: postEditField
        // width: parent.width - postListField.width - 60
        height: parent.height - generationToggleButton.height - 60
        // height: 100
        border.color: main.borderColor
        color: main.backgroundColor
        radius: 5

        anchors.top: parent.top
        anchors.left:  postListField.right
        anchors.right: parent.right
        anchors.topMargin: 20
        anchors.leftMargin: 10
        anchors.rightMargin: 20

        ScrollView {
            id: postEditScrollView
            anchors.fill: parent
            anchors.margins: 5
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            clip: true
            contentWidth: availableWidth

            ColumnLayout {
                width: postEditScrollView.availableWidth
                spacing: 40

                Switch {
                    id: postEnabledButton
                    text: "Пост активен"
                }

                TextField {
                    id: postNameTextField
                    background: TextFieldBackground {}
                    placeholderText: "Мой пост"
                    text: generatorBackend.postListModel.currentPostName

                    onEditingFinished: {
                        generatorBackend.postListModel.currentPostName = text
                    }

                    Label {
                        text: "Имя поста"
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 5
                    }
                }

                TextField {
                    id: latitudeTextField
                    background: TextFieldBackground {}
                    placeholderText: "60"

                    Label {
                        text: "Широта (град.)"
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 5
                    }

                    TextField {
                        id: longitudeTextField
                        background: TextFieldBackground {}
                        placeholderText: "30"
                        anchors.left: parent.right
                        anchors.leftMargin: 20
                        Label {
                            text: "Долгота (град.)"
                            anchors.bottom: parent.top
                            anchors.bottomMargin: 5
                        }
                    }
                }

                TextField {
                    id: frequencyTextField
                    background: TextFieldBackground {}
                    placeholderText: "100000000"

                    Label {
                        text: "Частота (Гц)"
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 5
                    }
                }

                TextField {
                    id: levelTextField
                    background: TextFieldBackground {}
                    placeholderText: "10"

                    Label {
                        text: "Уровень (дБ)"
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 5
                    }

                    TextField {
                        id: levelSigmaTextField
                        background: TextFieldBackground {}
                        placeholderText: "5"
                        anchors.left: parent.right
                        anchors.leftMargin: 20
                        Label {
                            text: "СКО уровня"
                            anchors.bottom: parent.top
                            anchors.bottomMargin: 5
                        }
                    }
                }

                TextField {
                    id: minAngleHTextField
                    background: TextFieldBackground {}
                    placeholderText: "10"
                    Label {
                        text: "Мин. гор. угол (град.)"
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 5
                    }

                    TextField {
                        id: maxAngleHTextField
                        background: TextFieldBackground {}
                        placeholderText: "5"
                        anchors.left: parent.right
                        anchors.leftMargin: 20
                        Label {
                            text: "Макс. гор. угол (град.)"
                            anchors.bottom: parent.top
                            anchors.bottomMargin: 5
                        }

                        ComboBox {
                            id: stepHComboBox
                            background: TextFieldBackground {}
                            // placeholderText: "5"
                            anchors.left: parent.right
                            anchors.leftMargin: 20
                            Label {
                                text: "Шаг гор. угла (град.)"
                                anchors.bottom: parent.top
                                anchors.bottomMargin: 5
                            }
                        }
                    }
                }

                TextField {
                    id: minAngleVTextField
                    background: TextFieldBackground {}
                    placeholderText: "10"
                    Label {
                        text: "Мин. верт. угол (град.)"
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 5
                    }

                    TextField {
                        id: maxAngleVTextField
                        background: TextFieldBackground {}
                        placeholderText: "5"
                        anchors.left: parent.right
                        anchors.leftMargin: 20
                        Label {
                            text: "Макс. верт. угол (град.)"
                            anchors.bottom: parent.top
                            anchors.bottomMargin: 5
                        }

                        ComboBox {
                            id: stepVComboBox
                            background: TextFieldBackground {}
                            // placeholderText: "5"
                            anchors.left: parent.right
                            anchors.leftMargin: 20
                            Label {
                                text: "Шаг верт. угла (град.)"
                                anchors.bottom: parent.top
                                anchors.bottomMargin: 5
                            }
                        }
                    }
                }

                TextField {
                    id: minPeriodField
                    background: TextFieldBackground {}
                    placeholderText: "10"

                    Label {
                        text: "Мин. период генерации (мс)"
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 5
                    }

                    TextField {
                        id: maxPeriodTextField
                        background: TextFieldBackground {}
                        placeholderText: "5"
                        anchors.left: parent.right
                        anchors.leftMargin: 20
                        Label {
                            text: "Макс. период генерации (мс)"
                            anchors.bottom: parent.top
                            anchors.bottomMargin: 5
                        }
                    }
                }

                ComboBox {
                    id: noiseTypeComboBox
                    background: TextFieldBackground {}
                    // placeholderText: "5"
                    Label {
                        text: "Тип шума"
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 5
                    }
                }
            }
        }
    }

    Button {
        id: generationToggleButton
        width: 250
        anchors.right: postEditField.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        background: ButtonBackground {}

        text : generatorBackend.generatorEnabled ? "Остановить генератор нагрузки" : "Запустить генератор нагрузки"

        onClicked: generatorBackend.onGeneratorEnabledButtonClicked()
    }

    Button {
        id: postConfigSaveButton
        width: 100
        anchors.right: generationToggleButton.left
        anchors.rightMargin: 20
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        background: ButtonBackground {}

        text : "Сохранить"

        onClicked: generatorBackend.postListModel.postUpdate()
    }

}
