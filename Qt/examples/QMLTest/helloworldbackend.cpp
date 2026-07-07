#include "helloworldbackend.h"
#include <QDebug>
#include <QString>

HelloWorldBackend::HelloWorldBackend(QObject *parent)
    : QObject{parent}, _helloWorldText("")
{
}

void HelloWorldBackend::buttonClicked() {
    qDebug() << "Hello World button clicked";

    if (helloWorldText() == "Hello World") {
        setHelloWorldText("");
    } else {
        setHelloWorldText("Hello World");
    }

}

QString HelloWorldBackend::helloWorldText() const {
    return _helloWorldText;
}

void HelloWorldBackend::setHelloWorldText(QString const & newHelloWorldText) {
    if (_helloWorldText == newHelloWorldText)
        return;

    _helloWorldText = newHelloWorldText;

    qDebug() << "Hello World text Changed";

    emit helloWorldTextChanged();
}
