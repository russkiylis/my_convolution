import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {

    Rectangle {
        id: postListField
        width: parent.width / 4 - 30
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
            width: parent.width / 2 - 5
            height: 25
            background: ButtonBackground {
            }

            anchors.top: parent.bottom
            anchors.left: parent.left
            anchors.topMargin: 5

            onClicked: {
                let newIndex = generatorBackend.postListModel.addPost()
                if (newIndex >= 0) {
                    postListView.currentIndex = newIndex
                    // generatorBackend.postListModel.postUpdate()
                }
            }
        }
        Button {
            id: postDeleteButton
            text: "-"
            width: parent.width / 2 - 5
            height: 25
            background: ButtonBackground {
            }

            anchors.top: parent.bottom
            anchors.left: postAddButton.right
            anchors.topMargin: 5
            anchors.leftMargin: 10

            onClicked: {
                let newIndex = generatorBackend.postListModel.removePost(postListView.currentIndex)
                postListView.currentIndex = newIndex
                // generatorBackend.postListModel.postUpdate()
            }

            enabled: postListView.count > 1
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
                background: ListViewItemBackground {
                }
                highlighted: ListView.isCurrentItem

                onClicked: {
                    postListView.currentIndex = index
                    // generatorBackend.postListModel.postUpdate()
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
        anchors.left: postListField.right
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

            // FIXME: строчки позволяют запихивать всё что угодно
            ColumnLayout {
                width: postEditScrollView.availableWidth
                spacing: 40

                Switch {
                    id: postEnabledButton
                    text: "Пост активен"

                    checked: generatorBackend.postListModel.currentPostEnabled

                    onToggled: {
                        generatorBackend.postListModel.currentPostEnabled = checked
                    }
                }

                TextField {
                    id: postNameTextField
                    background: TextFieldBackground {
                    }
                    placeholderText: "Мой пост"
                    text: generatorBackend.postListModel.currentPostName
                    selectByMouse: true

                    onTextEdited: {
                        generatorBackend.postListModel.currentPostName = text
                    }

                    Label {
                        text: "Имя поста"
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 5
                        color: main.textColor
                    }
                }

                TextField {
                    id: latitudeTextField
                    background: TextFieldBackground {
                    }
                    placeholderText: "60"
                    text: generatorBackend.postListModel.currentLatitude
                    selectByMouse: true

                    onTextEdited: {
                        generatorBackend.postListModel.currentLatitude = text
                    }

                    Label {
                        text: "Широта (град.)"
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 5
                        color: main.textColor
                    }

                    TextField {
                        id: longitudeTextField
                        background: TextFieldBackground {
                        }
                        placeholderText: "30"
                        anchors.left: parent.right
                        anchors.leftMargin: 20
                        text: generatorBackend.postListModel.currentLongitude
                        selectByMouse: true

                        onTextEdited: {
                            generatorBackend.postListModel.currentLongitude = text
                        }

                        Label {
                            text: "Долгота (град.)"
                            anchors.bottom: parent.top
                            anchors.bottomMargin: 5
                            color: main.textColor
                        }
                    }
                }

                TextField {
                    id: frequencyTextField
                    background: TextFieldBackground {
                    }
                    placeholderText: "100000000"
                    text: generatorBackend.postListModel.currentFrequency
                    selectByMouse: true

                    onTextEdited: {
                        generatorBackend.postListModel.currentFrequency = text
                    }

                    Label {
                        text: "Частота (Гц)"
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 5
                        color: main.textColor
                    }
                }

                TextField {
                    id: levelTextField
                    background: TextFieldBackground {
                    }
                    placeholderText: "10"
                    text: generatorBackend.postListModel.currentLevel
                    selectByMouse: true

                    onTextEdited: {
                        generatorBackend.postListModel.currentLevel = text
                    }

                    Label {
                        text: "Уровень (дБ)"
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 5
                        color: main.textColor
                    }

                    TextField {
                        id: levelSigmaTextField
                        background: TextFieldBackground {
                        }
                        placeholderText: "5"
                        anchors.left: parent.right
                        anchors.leftMargin: 20
                        text: generatorBackend.postListModel.currentLevelSigma
                        selectByMouse: true

                        onTextEdited: {
                            generatorBackend.postListModel.currentLevelSigma = text
                        }

                        Label {
                            text: "СКО уровня"
                            anchors.bottom: parent.top
                            anchors.bottomMargin: 5
                            color: main.textColor
                        }
                    }
                }

                TextField {
                    id: minAngleHTextField
                    background: TextFieldBackground {
                    }
                    placeholderText: "10"
                    text: generatorBackend.postListModel.currentMinAngleH
                    selectByMouse: true

                    onTextEdited: {
                        generatorBackend.postListModel.currentMinAngleH = text
                    }

                    Label {
                        text: "Мин. гор. угол (град.)"
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 5
                        color: main.textColor
                    }

                    TextField {
                        id: maxAngleHTextField
                        background: TextFieldBackground {
                        }
                        placeholderText: "5"
                        anchors.left: parent.right
                        anchors.leftMargin: 20
                        text: generatorBackend.postListModel.currentMaxAngleH
                        selectByMouse: true

                        onTextEdited: {
                            generatorBackend.postListModel.currentMaxAngleH = text
                        }

                        Label {
                            text: "Макс. гор. угол (град.)"
                            anchors.bottom: parent.top
                            anchors.bottomMargin: 5
                            color: main.textColor
                        }

                        ComboBox {
                            id: stepHComboBox
                            background: TextFieldBackground {
                            }
                            // placeholderText: "5"
                            anchors.left: parent.right
                            anchors.leftMargin: 20
                            currentIndex: generatorBackend.postListModel.currentStepH

                            onActivated: {
                                generatorBackend.postListModel.currentStepH = currentIndex
                            }

                            model: [
                                "1",
                                "0.5",
                                "0.2",
                                "0.1",
                                "0.01"
                            ]

                            Label {
                                text: "Шаг гор. угла (град.)"
                                anchors.bottom: parent.top
                                anchors.bottomMargin: 5
                                color: main.textColor
                            }
                        }
                    }
                }

                TextField {
                    id: minAngleVTextField
                    background: TextFieldBackground {
                    }
                    placeholderText: "10"
                    text: generatorBackend.postListModel.currentMinAngleV
                    selectByMouse: true

                    onTextEdited: {
                        generatorBackend.postListModel.currentMinAngleV = text
                    }

                    Label {
                        text: "Мин. верт. угол (град.)"
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 5
                        color: main.textColor
                    }

                    TextField {
                        id: maxAngleVTextField
                        background: TextFieldBackground {
                        }
                        placeholderText: "5"
                        anchors.left: parent.right
                        anchors.leftMargin: 20
                        text: generatorBackend.postListModel.currentMaxAngleV
                        selectByMouse: true

                        onTextEdited: {
                            generatorBackend.postListModel.currentMaxAngleV = text
                        }

                        Label {
                            text: "Макс. верт. угол (град.)"
                            anchors.bottom: parent.top
                            anchors.bottomMargin: 5
                            color: main.textColor
                        }

                        ComboBox {
                            id: stepVComboBox
                            background: TextFieldBackground {
                            }
                            // placeholderText: "5"
                            anchors.left: parent.right
                            anchors.leftMargin: 20
                            currentIndex: generatorBackend.postListModel.currentStepV

                            onActivated: {
                                generatorBackend.postListModel.currentStepV = currentIndex
                            }

                            model: [
                                "1",
                                "0.5",
                                "0.2",
                                "0.1",
                                "0.01"
                            ]

                            Label {
                                text: "Шаг верт. угла (град.)"
                                anchors.bottom: parent.top
                                anchors.bottomMargin: 5
                                color: main.textColor
                            }
                        }
                    }
                }

                TextField {
                    id: minPeriodField
                    background: TextFieldBackground {
                    }
                    placeholderText: "10"
                    text: generatorBackend.postListModel.currentMinPeriod
                    selectByMouse: true

                    onTextEdited: {
                        generatorBackend.postListModel.currentMinPeriod = text
                    }

                    Label {
                        text: "Мин. период генерации (мс)"
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 5
                        color: main.textColor
                    }

                    TextField {
                        id: maxPeriodTextField
                        background: TextFieldBackground {
                        }
                        placeholderText: "5"
                        anchors.left: parent.right
                        anchors.leftMargin: 20
                        selectByMouse: true
                        text: generatorBackend.postListModel.currentMaxPeriod

                        onTextEdited: {
                            generatorBackend.postListModel.currentMaxPeriod = text
                        }

                        Label {
                            text: "Макс. период генерации (мс)"
                            anchors.bottom: parent.top
                            anchors.bottomMargin: 5
                            color: main.textColor
                        }
                    }
                }

                ComboBox {
                    id: noiseTypeComboBox
                    background: TextFieldBackground {
                    }

                    model: [
                        "Нормальный",
                        "Равномерный"
                    ]

                    Label {
                        text: "Тип шума"
                        anchors.bottom: parent.top
                        anchors.bottomMargin: 5
                        color: main.textColor
                    }
                    currentIndex: generatorBackend.postListModel.currentNoiseType

                    onActivated: {
                        generatorBackend.postListModel.currentNoiseType = currentIndex
                    }

                    Loader {
                        sourceComponent: {
                            switch (generatorBackend.postListModel.currentNoiseType) {
                                case 0:
                                    return normalFields
                                case 1:
                                    return uniformFields
                                default:
                                    return null
                            }
                        }

                        anchors.left: parent.right
                        anchors.leftMargin: 20
                    }

                    Component {
                        id: normalFields

                        RowLayout {
                            spacing: 20
                            TextField {
                                id: meanNormalNoiseTextField
                                background: TextFieldBackground {
                                }
                                placeholderText: "0"
                                selectByMouse: true
                                text: generatorBackend.postListModel.noiseBackend.currentMean

                                onTextEdited: {
                                    generatorBackend.postListModel.noiseBackend.currentMean = text
                                }

                                Label {
                                    text: "Мат. ожидание шума"
                                    anchors.bottom: parent.top
                                    anchors.bottomMargin: 5
                                    color: main.textColor
                                }
                            }

                            TextField {
                                id: sigmaNormalNoiseTextField
                                background: TextFieldBackground {
                                }
                                placeholderText: "2"
                                selectByMouse: true
                                text: generatorBackend.postListModel.noiseBackend.currentSigma

                                onTextEdited: {
                                    generatorBackend.postListModel.noiseBackend.currentSigma = text
                                }

                                Label {
                                    text: "СКО шума"
                                    anchors.bottom: parent.top
                                    anchors.bottomMargin: 5
                                    color: main.textColor
                                }
                            }
                        }
                    }

                    Component {
                        id: uniformFields

                        RowLayout {
                            spacing: 20
                            TextField {
                                id: minUniformNoiseTextField
                                background: TextFieldBackground {
                                }
                                placeholderText: "-10"
                                selectByMouse: true
                                text: generatorBackend.postListModel.noiseBackend.currentMin


                                onTextEdited: {
                                    generatorBackend.postListModel.noiseBackend.currentMin = text
                                }

                                Label {
                                    text: "Мин. значение шума"
                                    anchors.bottom: parent.top
                                    anchors.bottomMargin: 5
                                    color: main.textColor
                                }
                            }

                            TextField {
                                id: maxUniformNoiseTextField
                                background: TextFieldBackground {
                                }
                                placeholderText: "20"
                                selectByMouse: true
                                text: generatorBackend.postListModel.noiseBackend.currentMax

                                onTextEdited: {
                                    generatorBackend.postListModel.noiseBackend.currentMax = text
                                }

                                Label {
                                    text: "Макс. значение шума"
                                    anchors.bottom: parent.top
                                    anchors.bottomMargin: 5
                                    color: main.textColor
                                }
                            }
                        }
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 10
                    RowLayout {
                        spacing: 20
                        Button {
                            Layout.fillWidth: true
                            background: ButtonBackground {}
                            text: "+"
                            Label {
                                text: "Пики (азимутальные)"
                                anchors.bottom: parent.top
                                anchors.bottomMargin: 5
                                color: main.textColor
                            }

                            //TODO: Плюсик должен нажиматься
                            onClicked: {

                            }
                        }
                        Button {
                            Layout.fillWidth: true
                            background: ButtonBackground {}
                            text: "-"

                            //TODO: Минусик должен нажиматься
                            onClicked: {

                            }
                        }
                    }
                    ListView {
                        id: peakHListView
                        interactive: false
                        clip: true
                        spacing: 10

                        Layout.fillWidth: true
                        Layout.preferredHeight: contentHeight

                        model: generatorBackend.postListModel.peakListModelH

                        delegate: ItemDelegate {
                            id: peakHListViewDelegate
                            width: peakHListView.width
                            height: 70
                            background: ListViewItemBackground {}
                            highlighted: ListView.isCurrentItem

                            property int rowIndex: index
                            property int peakTypeValue: model.peakType
                            property double centerValue: model.center
                            property double amplitudeValue: model.amplitude
                            property double sigmaValue: model.sigma
                            property double halfWidthValue: model.halfWidth

                            onClicked: {
                                peakHListView.currentIndex = index
                                // generatorBackend.postListModel.postUpdate()
                                // generatorBackend.postListModel.setPostIndex(index)
                            }

                            ComboBox {
                                id: peakHTypeComboBox
                                background: TextFieldBackground {
                                }
                                anchors.left: parent.left
                                anchors.bottom: parent.bottom
                                anchors.leftMargin: 10
                                anchors.bottomMargin: 10
                                currentIndex: peakHListViewDelegate.peakTypeValue

                                onActivated: {
                                    generatorBackend.postListModel.peakListModelH.setPeakType(peakHListViewDelegate.rowIndex, currentIndex)
                                }

                                model: [
                                    "Гауссовский",
                                    "Треугольный",
                                    "Прямоугольный"
                                ]

                                Label {
                                    text: "Тип пика"
                                    anchors.bottom: parent.top
                                    anchors.bottomMargin: 5
                                    color: main.textColor
                                }
                            }

                            TextField {
                                id: peakHCenterTextField
                                background: TextFieldBackground {
                                }
                                placeholderText: "180"
                                text: peakHListViewDelegate.centerValue
                                selectByMouse: true

                                anchors.left: peakHTypeComboBox.right
                                anchors.bottom: peakHTypeComboBox.bottom
                                anchors.leftMargin: 20

                                onTextEdited: {
                                    generatorBackend.postListModel.peakListModelH.setCenter(peakHListViewDelegate.rowIndex, text)
                                }

                                Label {
                                    text: "Координата вершины"
                                    anchors.bottom: parent.top
                                    anchors.bottomMargin: 5
                                    color: main.textColor
                                }
                            }

                            TextField {
                                id: peakHAmplitudeTextField
                                background: TextFieldBackground {
                                }
                                placeholderText: "10"
                                text: peakHListViewDelegate.amplitudeValue
                                selectByMouse: true

                                anchors.left: peakHCenterTextField.right
                                anchors.bottom: peakHCenterTextField.bottom
                                anchors.leftMargin: 20

                                onTextEdited: {
                                    generatorBackend.postListModel.peakListModelH.setAmplitude(peakHListViewDelegate.rowIndex, text)
                                }

                                Label {
                                    text: "Амплитуда"
                                    anchors.bottom: parent.top
                                    anchors.bottomMargin: 5
                                    color: main.textColor
                                }
                            }

                            TextField {
                                id: peakHSigmaTextField
                                background: TextFieldBackground {
                                }
                                placeholderText: "10"
                                text: peakHListViewDelegate.sigmaValue
                                selectByMouse: true

                                anchors.left: peakHAmplitudeTextField.right
                                anchors.bottom: peakHAmplitudeTextField.bottom
                                anchors.leftMargin: 20
                                visible: peakHTypeComboBox.currentIndex === 0

                                onTextEdited: {
                                    generatorBackend.postListModel.peakListModelH.setSigma(peakHListViewDelegate.rowIndex, text)
                                }

                                Label {
                                    text: "СКО"
                                    anchors.bottom: parent.top
                                    anchors.bottomMargin: 5
                                    color: main.textColor
                                }
                            }

                            TextField {
                                id: peakHHalfWidthTextField
                                background: TextFieldBackground {
                                }
                                placeholderText: "25"
                                text: peakHListViewDelegate.halfWidthValue
                                selectByMouse: true

                                anchors.left: peakHAmplitudeTextField.right
                                anchors.bottom: peakHAmplitudeTextField.bottom
                                anchors.leftMargin: 20
                                visible: peakHTypeComboBox.currentIndex === 1
                                    || peakHTypeComboBox.currentIndex === 2

                                onTextEdited: {
                                    generatorBackend.postListModel.peakListModelH.setHalfWidth(peakHListViewDelegate.rowIndex, text)
                                }

                                Label {
                                    text: "Полуширина"
                                    anchors.bottom: parent.top
                                    anchors.bottomMargin: 5
                                    color: main.textColor
                                }
                            }
                        }
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 10
                    RowLayout {
                        spacing: 20
                        Button {
                            Layout.fillWidth: true
                            background: ButtonBackground {}
                            text: "+"
                            Label {
                                text: "Пики (угломестные)"
                                anchors.bottom: parent.top
                                anchors.bottomMargin: 5
                                color: main.textColor
                            }

                            //TODO: Плюсик должен нажиматься
                            onClicked: {

                            }
                        }
                        Button {
                            Layout.fillWidth: true
                            background: ButtonBackground {}
                            text: "-"

                            //TODO: Минусик должен нажиматься
                            onClicked: {

                            }
                        }
                    }
                    ListView {
                        id: peakVListView
                        interactive: false
                        clip: true
                        spacing: 10

                        Layout.fillWidth: true
                        Layout.preferredHeight: contentHeight

                        model: generatorBackend.postListModel.peakListModelV

                        delegate: ItemDelegate {
                            id: peakVListViewDelegate
                            width: peakHListView.width
                            height: 70
                            background: ListViewItemBackground {
                            }
                            highlighted: ListView.isCurrentItem

                            property int rowIndex: index
                            property int peakTypeValue: model.peakType
                            property double centerValue: model.center
                            property double amplitudeValue: model.amplitude
                            property double sigmaValue: model.sigma
                            property double halfWidthValue: model.halfWidth

                            onClicked: {
                                peakHListView.currentIndex = index
                                // generatorBackend.postListModel.postUpdate()
                                // generatorBackend.postListModel.setPostIndex(index)
                            }

                            ComboBox {
                                id: peakVTypeComboBox
                                background: TextFieldBackground {
                                }
                                anchors.left: parent.left
                                anchors.bottom: parent.bottom
                                anchors.leftMargin: 10
                                anchors.bottomMargin: 10
                                currentIndex: peakVListViewDelegate.peakTypeValue

                                onActivated: {
                                    generatorBackend.postListModel.peakListModelV.setPeakType(peakVListViewDelegate.rowIndex, currentIndex)
                                }

                                model: [
                                    "Гауссовский",
                                    "Треугольный",
                                    "Прямоугольный"
                                ]

                                Label {
                                    text: "Тип пика"
                                    anchors.bottom: parent.top
                                    anchors.bottomMargin: 5
                                    color: main.textColor
                                }
                            }

                            TextField {
                                id: peakVCenterTextField
                                background: TextFieldBackground {
                                }
                                placeholderText: "180"
                                text: peakVListViewDelegate.centerValue
                                selectByMouse: true

                                anchors.left: peakVTypeComboBox.right
                                anchors.bottom: peakVTypeComboBox.bottom
                                anchors.leftMargin: 20

                                onTextEdited: {
                                    generatorBackend.postListModel.peakListModelV.setCenter(peakVListViewDelegate.rowIndex, text)
                                }

                                Label {
                                    text: "Координата вершины"
                                    anchors.bottom: parent.top
                                    anchors.bottomMargin: 5
                                    color: main.textColor
                                }
                            }

                            TextField {
                                id: peakVAmplitudeTextField
                                background: TextFieldBackground {
                                }
                                placeholderText: "10"
                                text: peakVListViewDelegate.amplitudeValue
                                selectByMouse: true

                                anchors.left: peakVCenterTextField.right
                                anchors.bottom: peakVCenterTextField.bottom
                                anchors.leftMargin: 20

                                onTextEdited: {
                                    generatorBackend.postListModel.peakListModelV.setAmplitude(peakVListViewDelegate.rowIndex, text)
                                }

                                Label {
                                    text: "Амплитуда"
                                    anchors.bottom: parent.top
                                    anchors.bottomMargin: 5
                                    color: main.textColor
                                }
                            }

                            TextField {
                                id: peakVSigmaTextField
                                background: TextFieldBackground {
                                }
                                placeholderText: "5"
                                text: peakVListViewDelegate.sigmaValue
                                selectByMouse: true

                                anchors.left: peakVAmplitudeTextField.right
                                anchors.bottom: peakVAmplitudeTextField.bottom
                                anchors.leftMargin: 20
                                visible: peakVTypeComboBox.currentIndex === 0

                                onTextEdited: {
                                    generatorBackend.postListModel.peakListModelV.setSigma(peakVListViewDelegate.rowIndex, text)
                                }

                                Label {
                                    text: "СКО"
                                    anchors.bottom: parent.top
                                    anchors.bottomMargin: 5
                                    color: main.textColor
                                }
                            }

                            TextField {
                                id: peakVHalfWidthTextField
                                background: TextFieldBackground {
                                }
                                placeholderText: "25"
                                text: peakVListViewDelegate.halfWidthValue
                                selectByMouse: true

                                anchors.left: peakVAmplitudeTextField.right
                                anchors.bottom: peakVAmplitudeTextField.bottom
                                anchors.leftMargin: 20
                                visible: peakVTypeComboBox.currentIndex === 1 || peakVTypeComboBox.currentIndex === 2

                                onTextEdited: {
                                    generatorBackend.postListModel.peakListModelV.setHalfWidth(peakVListViewDelegate.rowIndex, text)
                                }

                                Label {
                                    text: "Полуширина"
                                    anchors.bottom: parent.top
                                    anchors.bottomMargin: 5
                                    color: main.textColor
                                }
                            }
                        }
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
        background: ButtonBackground {
        }

        text: generatorBackend.generatorEnabled ? "Остановить генератор нагрузки" : "Запустить генератор нагрузки"

        onClicked: generatorBackend.onGeneratorEnabledButtonClicked()
    }

    Button {
        id: postConfigSaveButton
        width: 175
        anchors.right: generationToggleButton.left
        anchors.rightMargin: 20
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        background: ButtonBackground {
        }

        text: "Сохранить изменения"

        onClicked: generatorBackend.postListModel.postUpdate()
    }

    Button {
        id: postConfigFallbackButton
        width: 175
        anchors.right: postConfigSaveButton.left
        anchors.rightMargin: 20
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        background: ButtonBackground {
        }

        text: "Отмена изменений"

        onClicked: {
            postListView.currentIndex = generatorBackend.postListModel.fallback()
        }
    }
}

