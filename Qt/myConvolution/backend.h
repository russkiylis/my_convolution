#pragma once

#include <qqml.h>
#include "databasemanager.h"

// Класс, подключаемый к QML и отвечающий за логику работы интерфейса
class ConnectionBackend : public QObject
{
public:
    // Получить статус базы данных
    [[nodiscard]] int dbStatus() const;

    // Получить последнюю ошибку
    [[nodiscard]] QString lastError() const;

private:
    Q_OBJECT
    Q_PROPERTY(int dbStatus READ dbStatus WRITE setDbStatus NOTIFY dbStatusChanged)
    Q_PROPERTY(QString lastError READ lastError WRITE setLastError NOTIFY lastErrorChanged)
    Q_PROPERTY(QString hostName READ hostName WRITE setHostName NOTIFY hostNameChanged)
    Q_PROPERTY(QString port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString databaseName READ databaseName WRITE setDatabaseName NOTIFY databaseNameChanged)
    Q_PROPERTY(QString connectOptions READ connectOptions WRITE setConnectOptions NOTIFY connectOptionsChanged)

    DatabaseManager m_db;      // Менеджер базы данных
    int m_dbStatus = 0; // Статус базы данных, 0 - не подключена, 1 - готова, 2 - занята
    QString m_lastError = "Ошибок нет."; // Последняя ошибка при работе с БД

    // Далее идут поля для параметров подключения

    QString m_hostName = "";
    QString m_port = "";
    QString m_userName = "";
    QString m_password = "";
    QString m_databaseName = "";
    QString m_connectOptions = "";

public:
    // Поле "имя хоста"
    [[nodiscard]] QString hostName() const;

    void setHostName(const QString &host_name);

    // Поле "порт"
    [[nodiscard]] QString port() const;

    void setPort(const QString &port);

    // Поле "имя пользователя"
    [[nodiscard]] QString userName() const;

    void setUserName(const QString &user_name);

    // Поле "пароль"
    [[nodiscard]] QString password() const;

    void setPassword(const QString &password);

    // Поле "имя БД"
    [[nodiscard]] QString databaseName() const;

    void setDatabaseName(const QString &database_name);

    // Поле "настройки подключения"
    [[nodiscard]] QString connectOptions() const;

    void setConnectOptions(const QString &connect_options);

    // Конструктор класса
    explicit ConnectionBackend(QObject *parent = nullptr);

    // Задать статус базы данных
    void setDbStatus(int const &dbStatus);

    // Задать статус ошибки
    void setLastError(QString const &lastError);

    // Метод, вызывающийся при нажатии кнопки генерации нагрузки
    // в базу данных
    Q_INVOKABLE void onDbConnectionButtonClicked(QString hostName, QString port, QString userName, QString password,
                                                 QString databaseName, QString connectOptions);

signals:
    void dbStatusChanged(int dbStatus);
    void lastErrorChanged(QString lastError);

    void hostNameChanged(QString hostName);
    void portChanged(QString port);
    void userNameChanged(QString userName);
    void passwordChanged(QString password);
    void databaseNameChanged(QString databaseName);
    void connectOptionsChanged(QString connectOptions);
};
