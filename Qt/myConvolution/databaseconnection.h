#pragma once

#include <QObject>
#include <QString>

// Класс для работы с подключением к БД
class DatabaseConnection : public QObject
{
    Q_OBJECT
public:
    // Конструктор
    explicit DatabaseConnection(QString const & connectionName = "myConvolution",
                                QString const & hostName = "127.0.0.1",
                                QString const & dbName = "my_convolution",
                                QString const & userName = "russkiylis",
                                QString const & password = "1337",
                                int const & port = 5432,
                                QObject * parent = nullptr);

    // Имя подключения (не путать с именем БД)
    QString connectionName() {return _connectionName;}

    // Адрес хоста
    QString hostName() {return _hostName;}

    // Порт
    int port() {return _port;}

    // Имя БД
    QString dbName() {return _dbName;}

    // Имя пользователя
    QString userName() {return _userName;}

    // Пароль
    QString password() {return _password;}

    // Задать имя подключения
    void setConnectionName(QString const & value);

    // Задать адрес хоста
    void setHostName(QString const & value);

    // Задать порт
    void setPort(int const & value);

    // Задать имя БД
    void setDbName(QString const & value);

    // Задать имя пользователя
    void setUserName(QString const & value);

    // Задать пароль
    void setPassword(QString const & value);

private:
    QString _connectionName;
    QString _hostName;
    int _port;
    QString _dbName;
    QString _userName;
    QString _password;
};
