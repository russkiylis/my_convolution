#pragma once

#include <QObject>

// Класс, подключаемый к QML и отвечающий за логику работы интерфейса
class Backend : public QObject
{
    Q_OBJECT

public:
    // Конструктор класса
    explicit Backend(QObject *parent = nullptr);

    // Метод, вызывающийся при нажатии кнопки генерации нагрузки
    // в базу данных
    Q_INVOKABLE void onGenerationButtonClicked();

};
