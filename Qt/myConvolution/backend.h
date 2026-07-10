#pragma once

#include <QObject>
#include <qqml.h>

#include "databasemanager.h"

// Класс, подключаемый к QML и отвечающий за логику работы интерфейса
class Backend : public QObject
{
public:
    // Получить статус базы данных
    int dbStatus() const;
    QString lastError();

private:
    Q_OBJECT
    Q_PROPERTY(int dbStatus READ dbStatus WRITE setDbStatus NOTIFY dbStatusChanged)
    Q_PROPERTY(QString lastError READ lastError WRITE setLastError NOTIFY lastErrorChanged)

    DatabaseManager _db;      // Менеджер базы данных
    int _dbStatus = 0; // Статус базы данных, 0 - не подключена, 1 - готова, 2 - занята
    QString _lastError = "Ошибок нет.";

public:
    // Конструктор класса
    explicit Backend(QObject *parent = nullptr);

    // Задать статус базы данных
    void setDbStatus(int const &dbStatus);

    // Задать статус ошибки
    void setLastError(QString const &lastError);

    // Метод, вызывающийся при нажатии кнопки генерации нагрузки
    // в базу данных
    Q_INVOKABLE void onDbConnectionButtonClicked();

signals:
    void dbStatusChanged(int dbStatus);
    void lastErrorChanged(QString lastError);
};
