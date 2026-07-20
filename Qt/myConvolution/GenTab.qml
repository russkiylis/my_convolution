import QtQuick 2.15
import QtQuick.Controls 2.15

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

}
