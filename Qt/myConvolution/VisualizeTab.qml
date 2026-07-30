import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    Button {
        text: "Прочитать"

        onClicked: visualizeBackend.readDb()
    }
}
