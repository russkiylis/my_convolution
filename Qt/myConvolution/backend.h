#pragma once

#include <QObject>
#include "databasemanager.h"

// Класс, подключаемый к QML и отвечающий за логику работы интерфейса
class Backend : public QObject
{
    Q_OBJECT
    DatabaseManager db;      // Менеджер базы данных

public:
    // Конструктор класса
    explicit Backend(QObject *parent = nullptr);

    // Метод, вызывающийся при нажатии кнопки генерации нагрузки
    // в базу данных
    Q_INVOKABLE void onGenerationButtonClicked();

};
