#ifndef HELLOWORLDBACKEND_H
#define HELLOWORLDBACKEND_H

#include <QObject>
#include <QString>

class HelloWorldBackend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString helloWorldText READ helloWorldText WRITE setHelloWorldText NOTIFY helloWorldTextChanged)

public:
    explicit HelloWorldBackend(QObject *parent = nullptr);

    QString helloWorldText() const;
    void setHelloWorldText(QString const & newHelloWorldText);

    Q_INVOKABLE void buttonClicked();

signals:
    void helloWorldTextChanged();

private:
    QString _helloWorldText;
};

#endif // HELLOWORLDBACKEND_H
