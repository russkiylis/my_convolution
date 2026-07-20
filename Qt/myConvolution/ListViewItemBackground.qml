import QtQuick 2.15

Rectangle {
    color: parent.pressed ? main.pressColor2 : main.backgroundColor2
    border.color: parent.highlighted ? "black" : main.borderColor2
    radius: 5
}

