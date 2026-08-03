// Эксперименты с byteArray

#include <iostream>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDataStream>
#include <QDebug>

int main()
{
    double constexpr x = 13.37;
    qint16 constexpr x2 = 2;
    float constexpr x3 = 2.82f;

    QByteArray y;
    QDataStream stream(&y, QIODevice::ReadWrite);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);

    stream << x;
    stream << x2;
    stream << x3;
    qDebug().noquote().nospace() << "Числа " << " в бинарном виде: " << y.toHex();
    qDebug().noquote().nospace() << "Размер QByteArray: " << y.size();

    double decoded;
    qint16 decoded2;
    float decoded3;
    stream.device()->seek(0);
    stream >> decoded;
    qDebug().noquote().nospace() << "Пришло число " << decoded;
    stream >> decoded2;
    qDebug().noquote().nospace() << "Пришло число " << decoded2;
    stream >> decoded3;
    qDebug().noquote().nospace() << "Пришло число " << decoded3;

    return 0;
}