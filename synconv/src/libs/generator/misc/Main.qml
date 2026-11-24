import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtDataVisualization 1.2
import Synconv.Viewer 1.0

ApplicationWindow {
    id: window
    visible: true
    title: "3D-визуализатор свёрток"
    width: 1180
    height: 760
    minimumWidth: 850
    minimumHeight: 570
    color: "#f3f6f8"
    palette.window: "#ffffff"
    palette.windowText: "#18313d"
    palette.text: "#18313d"
    palette.buttonText: "#18313d"
    palette.button: "#f2f5f6"
    palette.base: "#ffffff"
    palette.highlight: "#167b84"
    palette.highlightedText: "#ffffff"

    readonly property var steps: [0.1, 0.25, 0.5, 1, 5]
    readonly property var stepLabels: ["0,1°", "0,25°", "0,5°", "1°", "5°"]
    property bool ready: false
    property bool loadingPeak: false
    property int nextPeakNumber: 1
    readonly property bool dragging: azimuth.pressed || elevation.pressed || amplitude.pressed ||
                                     peakWidth.pressed || noiseMean.pressed || noiseSigma.pressed ||
                                     noiseMin.pressed || noiseMax.pressed

    ListModel { id: peaks }

    TrackpadOrbit {
        parent: surface
        anchors.fill: parent
        camera: surface.scene.activeCamera
    }

    // Промежуточные кадры продолжают поступать при непрерывном движении.
    function schedule() {
        if (ready && !refresh.running)
            refresh.start()
    }

    onDraggingChanged: {
        if (!dragging)
            schedule()
    }

    function loadSelectedPeak() {
        if (!ready || loadingPeak || peakSelector.currentIndex < 0 || peakSelector.currentIndex >= peaks.count)
            return

        loadingPeak = true
        var peak = peaks.get(peakSelector.currentIndex)
        shape.currentIndex = peak.shape
        azimuth.value = peak.azimuth
        elevation.value = peak.elevation
        amplitude.value = peak.amplitude
        peakWidth.value = peak.width
        loadingPeak = false
    }

    function editPeak(role, value) {
        var index = peakSelector.currentIndex
        if (!ready || loadingPeak || index < 0 || index >= peaks.count || peaks.get(index)[role] === value)
            return
        peaks.setProperty(index, role, value)
        schedule()
    }

    function addPeak() {
        var n = nextPeakNumber++
        loadingPeak = true
        peaks.append({ title: "Пик " + n, shape: 0, azimuth: (170 + (n - 1) * 90) % 360,
                       elevation: n % 2 === 1 ? 15 : -15, amplitude: 75, width: 18 })
        peakSelector.currentIndex = peaks.count - 1
        loadingPeak = false
        loadSelectedPeak()
        schedule()
    }

    function removePeak() {
        var index = peakSelector.currentIndex
        if (index < 0 || index >= peaks.count)
            return
        loadingPeak = true
        peaks.remove(index)
        peakSelector.currentIndex = Math.min(index, peaks.count - 1)
        loadingPeak = false
        loadSelectedPeak()
        schedule()
    }

    function regenerate() {
        var snapshot = []
        for (var i = 0; i < peaks.count; ++i) {
            var peak = peaks.get(i)
            snapshot.push({ shape: peak.shape, azimuth: peak.azimuth, elevation: peak.elevation,
                            amplitude: peak.amplitude, width: peak.width })
        }
        var noiseSpec = noiseSelector.currentIndex === 0
                      ? { type: "normal", mean: noiseMean.value, sigma: noiseSigma.value }
                      : { type: "uniform", min: noiseMin.value, max: noiseMax.value }
        viewer.generate(series, snapshot, noiseSpec, steps[stepH.currentIndex],
                        steps[stepV.currentIndex], seed.value, dragging)
    }

    function resetCamera() {
        surface.scene.activeCamera.cameraPreset = Camera3D.CameraPresetIsometricRight
        surface.scene.activeCamera.zoomLevel = 105
    }

    Component.onCompleted: {
        resetCamera()
        ready = true
        addPeak()
    }

    Timer {
        id: refresh
        interval: 32
        onTriggered: window.regenerate()
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            color: "#ffffff"
            Layout.preferredWidth: 300
            Layout.fillHeight: true

            ScrollView {
                id: scroll
                anchors.fill: parent
                anchors.margins: 20
                clip: true
                contentWidth: availableWidth
                ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

                ColumnLayout {
                    width: scroll.availableWidth
                    spacing: 9
                    Label {
                        text: "Пики (" + peaks.count + ")"
                        font.pixelSize: 22
                        font.weight: Font.DemiBold
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        ComboBox {
                            id: peakSelector
                            objectName: "peakSelector"
                            Layout.fillWidth: true
                            model: peaks
                            textRole: "title"
                            displayText: currentIndex >= 0 ? currentText : "Нет пиков"
                            enabled: peaks.count > 0
                            Accessible.name: "Выбор пика"
                            onCurrentIndexChanged: window.loadSelectedPeak()
                        }
                        ToolButton {
                            objectName: "addPeak"
                            text: "+"
                            Accessible.name: "Добавить пик"
                            ToolTip.text: Accessible.name
                            ToolTip.visible: hovered
                            onClicked: window.addPeak()
                        }
                        ToolButton {
                            objectName: "removePeak"
                            text: "−"
                            enabled: peaks.count > 0
                            Accessible.name: "Удалить выбранный пик"
                            ToolTip.text: Accessible.name
                            ToolTip.visible: hovered
                            onClicked: window.removePeak()
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        visible: peaks.count > 0
                        spacing: 9
                        ComboBox {
                            id: shape
                            objectName: "shapeSelector"
                            Layout.fillWidth: true
                            Accessible.name: "Форма пика"
                            model: ["Гауссовский", "Треугольный", "Прямоугольный"]
                            onCurrentIndexChanged: window.editPeak("shape", currentIndex)
                        }
                        ParameterSlider {
                            id: azimuth
                            sliderName: "azimuthSlider"
                            Layout.fillWidth: true
                            caption: "Азимут H"
                            suffix: "°"
                            decimals: 1
                            from: 0; to: 360; stepSize: 0.1; value: 170
                            onValueChanged: window.editPeak("azimuth", value)
                        }
                        ParameterSlider {
                            id: elevation
                            sliderName: "elevationSlider"
                            Layout.fillWidth: true
                            caption: "Угол места V"
                            suffix: "°"
                            decimals: 1
                            from: -90; to: 90; stepSize: 0.1; value: 15
                            onValueChanged: window.editPeak("elevation", value)
                        }
                        ParameterSlider {
                            id: amplitude
                            sliderName: "amplitudeSlider"
                            Layout.fillWidth: true
                            caption: "Амплитуда"
                            from: 0; to: 150; stepSize: 1; value: 75
                            onValueChanged: window.editPeak("amplitude", value)
                        }
                        ParameterSlider {
                            id: peakWidth
                            sliderName: "widthSlider"
                            Layout.fillWidth: true
                            caption: shape.currentIndex === 0 ? "СКО пика" : "Полуширина"
                            suffix: "°"
                            decimals: 1
                            from: 0; to: 90; stepSize: 0.1; value: 18
                            onValueChanged: window.editPeak("width", value)
                        }
                    }

                    ToolSeparator {
                        Layout.fillWidth: true
                        orientation: Qt.Horizontal
                    }
                    Label {
                        text: "Шум и сетка"
                        font.pixelSize: 19
                        font.weight: Font.DemiBold
                    }
                    ComboBox {
                        id: noiseSelector
                        objectName: "noiseSelector"
                        Layout.fillWidth: true
                        Accessible.name: "Распределение шума"
                        model: ["Нормальный шум", "Равномерный шум"]
                        onCurrentIndexChanged: window.schedule()
                    }
                    ParameterSlider {
                        id: noiseMean
                        sliderName: "noiseMeanSlider"
                        Layout.fillWidth: true
                        visible: noiseSelector.currentIndex === 0
                        caption: "Среднее шума"
                        decimals: 1
                        from: -100; to: 100; stepSize: 0.1; value: 0
                        onValueChanged: window.schedule()
                    }
                    ParameterSlider {
                        id: noiseSigma
                        sliderName: "noiseSlider"
                        Layout.fillWidth: true
                        visible: noiseSelector.currentIndex === 0
                        caption: "СКО шума"
                        decimals: 1
                        from: 0; to: 25; stepSize: 0.1; value: 0
                        onValueChanged: window.schedule()
                    }
                    ParameterSlider {
                        id: noiseMin
                        sliderName: "noiseMinSlider"
                        Layout.fillWidth: true
                        visible: noiseSelector.currentIndex === 1
                        caption: "Нижняя граница"
                        decimals: 1
                        from: -100; to: noiseMax.value; stepSize: 0.1; value: -10
                        onValueChanged: window.schedule()
                    }
                    ParameterSlider {
                        id: noiseMax
                        sliderName: "noiseMaxSlider"
                        Layout.fillWidth: true
                        visible: noiseSelector.currentIndex === 1
                        caption: "Верхняя граница"
                        decimals: 1
                        from: noiseMin.value; to: 100; stepSize: 0.1; value: 10
                        onValueChanged: window.schedule()
                    }
                    Label {
                        Layout.fillWidth: true
                        visible: noiseSelector.currentIndex === 1
                        text: "Среднее шума: " + ((noiseMin.value + noiseMax.value) / 2)
                              .toLocaleString(Qt.locale("ru_RU"), "f", 2)
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        Label { text: "Seed шума"; Layout.fillWidth: true }
                        SpinBox {
                            id: seed
                            objectName: "noiseSeed"
                            Accessible.name: "Seed шума"
                            editable: true
                            from: 0; to: 999999; value: 42
                            onValueChanged: window.schedule()
                        }
                    }
                    GridLayout {
                        Layout.fillWidth: true
                        columns: 2
                        columnSpacing: 10
                        rowSpacing: 2
                        Label { text: "Шаг H" }
                        Label { text: "Шаг V" }
                        ComboBox {
                            id: stepH
                            objectName: "stepHSelector"
                            Layout.fillWidth: true
                            Accessible.name: "Шаг по азимуту"
                            model: window.stepLabels
                            currentIndex: 3
                            onCurrentIndexChanged: window.schedule()
                        }
                        ComboBox {
                            id: stepV
                            objectName: "stepVSelector"
                            Layout.fillWidth: true
                            Accessible.name: "Шаг по углу места"
                            model: window.stepLabels
                            currentIndex: 3
                            onCurrentIndexChanged: window.schedule()
                        }
                    }
                }
            }
        }

        Rectangle { Layout.fillHeight: true; Layout.preferredWidth: 1; color: "#d6e1e5" }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 18
            spacing: 9
            RowLayout {
                Layout.fillWidth: true
                Label {
                    text: "Поверхность свёртки"
                    font.pixelSize: 24
                    font.weight: Font.DemiBold
                    Layout.fillWidth: true
                }
                ToolButton {
                    text: "Сбросить вид"
                    onClicked: window.resetCamera()
                }
            }

            Surface3D {
                id: surface
                objectName: "surfaceGraph"
                Layout.fillWidth: true
                Layout.fillHeight: true
                horizontalAspectRatio: 2
                aspectRatio: 2.4
                shadowQuality: AbstractGraph3D.ShadowQualityNone
                selectionMode: AbstractGraph3D.SelectionItem
                theme: Theme3D {
                    type: Theme3D.ThemeQt
                    windowColor: "#f3f6f8"
                    backgroundColor: "#e7eef0"
                    gridLineColor: "#b2c5ca"
                    labelTextColor: "#213e49"
                    labelBackgroundEnabled: false
                    font.pointSize: 12
                }
                axisX: ValueAxis3D {
                    title: "Азимут H, °"; titleVisible: true
                    autoAdjustRange: false; min: 0; max: 360
                    segmentCount: 6; labelFormat: "%.0f"
                }
                axisY: ValueAxis3D {
                    title: "Значение"; titleVisible: true
                    autoAdjustRange: false; min: 0; max: 100
                    segmentCount: 4; labelFormat: "%.0f"
                }
                axisZ: ValueAxis3D {
                    title: "Угол места V, °"; titleVisible: true
                    autoAdjustRange: false; min: -90; max: 90
                    segmentCount: 4; labelFormat: "%.0f"
                }
                Surface3DSeries {
                    id: series
                    objectName: "surfaceSeries"
                    drawMode: Surface3DSeries.DrawSurface
                    flatShadingEnabled: false
                    itemLabelFormat: "H @xLabel°   V @zLabel°   Значение @yLabel"
                    colorStyle: Theme3D.ColorStyleRangeGradient
                    baseGradient: ColorGradient {
                        ColorGradientStop { position: 0; color: "#234781" }
                        ColorGradientStop { position: 0.3; color: "#098cb4" }
                        ColorGradientStop { position: 0.55; color: "#5ec993" }
                        ColorGradientStop { position: 0.78; color: "#ffd56c" }
                        ColorGradientStop { position: 1; color: "#df6047" }
                    }
                }
            }

            Label {
                Layout.fillWidth: true
                text: viewer.error || viewer.summary || "Расчёт свёртки…"
                color: viewer.error ? "#ab342b" : "#213e49"
                font.weight: Font.DemiBold
                wrapMode: Text.WordWrap
            }
            Label {
                Layout.fillWidth: true
                text: "Вращение — двумя пальцами или ПКМ  ·  Масштаб — щипок или колесо  ·  Значение — щелчок по поверхности"
                color: "#526a75"
                wrapMode: Text.WordWrap
            }
        }
    }
}
