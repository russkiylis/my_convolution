import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: control

    property string caption
    property string suffix: ""
    property int decimals: 0
    property alias from: slider.from
    property alias to: slider.to
    property alias stepSize: slider.stepSize
    property alias value: slider.value
    readonly property alias pressed: slider.pressed
    property alias sliderName: slider.objectName

    spacing: 1

    RowLayout {
        Layout.fillWidth: true
        Label {
            text: control.caption
            Layout.fillWidth: true
        }
        Label {
            text: Number(slider.value).toLocaleString(Qt.locale("ru_RU"), "f", control.decimals) + control.suffix
            font.weight: Font.DemiBold
            color: "#16676e"
        }
    }

    Slider {
        id: slider
        Layout.fillWidth: true
        implicitHeight: 29
        live: true
        snapMode: Slider.SnapAlways
        Accessible.name: control.caption
    }
}
