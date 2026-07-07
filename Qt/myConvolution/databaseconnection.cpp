#include <QDebug>
#include "databaseconnection.h"

DatabaseConnection::DatabaseConnection(QString const & connectionName,
                                       QString const & hostName,
                                       QString const & dbName,
                                       QString const & userName,
                                       QString const & password,
                                       int const & port,
                                       QObject * parent)
    :_connectionName(connectionName),
    _hostName(hostName),
    _dbName(dbName),
    _userName(userName),
    _password(password),
    _port(5432),
    QObject{parent}
{
    qDebug().noquote().nospace() << "Создано новое подключение \"" << _connectionName << "\": pg://" << _userName << ":" << _password << "@" << _hostName << ":" << _port << "/" << dbName;
}

void DatabaseConnection::setConnectionName(QString const & value) {
    if (_connectionName == value)
        return;
    qDebug().noquote().nospace() << "Подключение \"" << _connectionName << "\": имя изменено на \"" << value << "\"";
    _connectionName = value;
}

void DatabaseConnection::setHostName(QString const & value) {
    if (_hostName == value)
        return;
    qDebug().noquote().nospace() << "Подключение \"" << _connectionName << "\": адрес хоста изменён на \"" << value << "\"";
    _hostName = value;
}

void DatabaseConnection::setPort(int const & value) {
    if (_port == value)
        return;
    qDebug().noquote().nospace()) << "Подключение \"" << _connectionName << "\": порт изменён на \"" << value << "\"";
    _port = value;
}

void DatabaseConnection::setDbName(QString const & value) {
    if (_dbName == value)
        return;
    qDebug().noquote().nospace() << "Подключение \"" << _connectionName << "\": имя БД изменено на \"" << value << "\"";
    _dbName = value;
}

void DatabaseConnection::setUserName(QString const & value) {
    if (_userName == value)
        return;
    qDebug().noquote().nospace() << "Подключение \"" << _connectionName << "\": имя пользователя изменено на \"" << value << "\"";
    _userName = value;
}

void DatabaseConnection::setPassword(QString const & value) {
    if (_password == value)
        return;
    qDebug().noquote().nospace() << "Подключение \"" << _connectionName << "\": пароль изменён на \"" << value << "\"";
    _password = value;
}

