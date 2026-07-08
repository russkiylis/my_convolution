#include <QDebug>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QString>
#include "databaseconnection.h"

DatabaseConnection::DatabaseConnection(QString const & connectionName,
                                       QString const & hostName,
                                       QString const & dbName,
                                       QString const & userName,
                                       QString const & password,
                                       int const & port,
                                       QString const & connectOptions,
                                       QObject * parent)
    :_connectionName(connectionName),
    _hostName(hostName),
    _dbName(dbName),
    _userName(userName),
    _password(password),
    _port(5432),
    _connectOptions(connectOptions),
    _fullConnectionName("\"" + _connectionName
                            + "\": pg://" + _userName
                            + ":" + _password
                            + "@" + _hostName
                            + ":" + QString::number(_port)
                            + "/" + _dbName),
    QObject{parent}
{
    if (QSqlDatabase::database(_connectionName).isValid()) {
        qDebug().noquote().nospace() << "[!] Подключение с именем \""
                                     << _connectionName << "\" "
                                     << "уже существует. Объект подключения не валиден.";
    } else {
        QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", _connectionName); // Создали подключение к БД
        _valid = true;
        update();

        qDebug().noquote().nospace() << "Создан объект подключения "
                                     << _fullConnectionName;
    }
}

void DatabaseConnection::setHostName(QString const & value) {
    if (!_valid) {
        qDebug().noquote().nospace() << "[!] "
                                     << _fullConnectionName
                                     << ": объект подключения не валиден."
                                     << "Невозможно изменить параметры подключения.";
        return;
    }
    if (_connected) {
        qDebug().noquote().nospace() << "[!] "
                                     << _fullConnectionName
                                     << ": невозможно изменить параметры "
                                        "открытого соединения.";
        return;
    }
    if (_hostName == value)
        return;
    _hostName = value;
    update();
}

void DatabaseConnection::setPort(int const & value) {
    if (!_valid) {
        qDebug().noquote().nospace() << "[!] "
                                     << _fullConnectionName
                                     << ": объект подключения не валиден."
                                     << "Невозможно изменить параметры подключения.";
        return;
    }
    if (_connected) {
        qDebug().noquote().nospace() << "[!] "
                                     << _fullConnectionName
                                     << ": невозможно изменить параметры "
                                        "открытого соединения.";
        return;
    }
    if (_port == value)
        return;
    _port = value;
    update();
}

void DatabaseConnection::setDbName(QString const & value) {
    if (!_valid) {
        qDebug().noquote().nospace() << "[!] "
                                     << _fullConnectionName
                                     << ": объект подключения не валиден."
                                     << "Невозможно изменить параметры подключения.";
        return;
    }
    if (_connected) {
        qDebug().noquote().nospace() << "[!] "
                                     << _fullConnectionName
                                     << ": невозможно изменить параметры "
                                        "открытого соединения.";
        return;
    }
    if (_dbName == value)
        return;
    _dbName = value;
    update();
}

void DatabaseConnection::setUserName(QString const & value) {
    if (!_valid) {
        qDebug().noquote().nospace() << "[!] "
                                     << _fullConnectionName
                                     << ": объект подключения не валиден."
                                     << "Невозможно изменить параметры подключения.";
        return;
    }
    if (_connected) {
        qDebug().noquote().nospace() << "[!] "
                                     << _fullConnectionName
                                     << ": невозможно изменить параметры "
                                        "открытого соединения.";
        return;
    }
    if (_userName == value)
        return;
    _userName = value;
    update();
}

void DatabaseConnection::setPassword(QString const & value) {
    if (!_valid) {
        qDebug().noquote().nospace() << "[!] "
                                     << _fullConnectionName
                                     << ": объект подключения не валиден."
                                     << "Невозможно изменить параметры подключения.";
        return;
    }
    if (_connected) {
        qDebug().noquote().nospace() << "[!] "
                                     << _fullConnectionName
                                     << ": невозможно изменить параметры "
                                        "открытого соединения.";
        return;
    }
    if (_password == value)
        return;
    _password = value;
    update();
}

void DatabaseConnection::setConnectOptions(QString const & value) {
    if (!_valid) {
        qDebug().noquote().nospace() << "[!] "
                                     << _fullConnectionName
                                     << ": объект подключения не валиден."
                                     << "Невозможно изменить параметры подключения.";
        return;
    }
    if (_connected) {
        qDebug().noquote().nospace() << "[!] "
                                     << _fullConnectionName
                                     << ": невозможно изменить параметры "
                                        "открытого соединения.";
        return;
    }
    if (_connectOptions == value)
        return;
    _connectOptions = value;
    update();
}

void DatabaseConnection::setFullConnectionName() {
    _fullConnectionName = "\"" + _connectionName
                          + "\": pg://" + _userName
                          + ":" + _password
                          + "@" + _hostName
                          + ":" + QString::number(_port)
                          + "/" + _dbName;
}

void DatabaseConnection::update() {
    if (!_valid) {
        qDebug().noquote().nospace() << "[!] "
                                     << _fullConnectionName
                                     << ": объект подключения не валиден."
                                     << "Невозможно изменить параметры подключения.";
        return;
    }
    if (_connected) {
        qDebug().noquote().nospace() << "[!] "
                                     << _fullConnectionName
                                     << ": невозможно изменить параметры "
                                        "открытого соединения.";
        return;
    }

    QSqlDatabase db = QSqlDatabase::database(_connectionName);
    db.setHostName(_hostName);
    db.setPort(_port);
    db.setDatabaseName(_dbName);
    db.setUserName(_userName);
    db.setPassword(_password);
    db.setConnectOptions(_connectOptions);
    setFullConnectionName();
    qDebug().noquote().nospace() << _fullConnectionName
                                 << ": параметры подключения изменены.";
}

bool DatabaseConnection::openConnection() {
    if (!_valid) {
        qDebug().noquote().nospace() << "[!] "
                                     << _fullConnectionName
                                     << ": объект подключения не валиден."
                                     << "Невозможно открыть соединение.";
        return false;
    }

    if (!_connected) {
        QSqlDatabase db = QSqlDatabase::database(_connectionName);
        _connected = db.open();    // Попытка наладить физическое соединение
        if (!_connected) {
            qDebug().noquote().nospace() << "[!] "
                                         << _fullConnectionName
                                         << ": не удалось подключиться к БД: "
                                         << db.lastError().text();
        } else {
            qDebug().noquote().nospace() << _fullConnectionName
                                         << ": установлено подключение к БД!";
        }
        return _connected;

    } else {
        qDebug().noquote().nospace() << "[!] "
                                     << _fullConnectionName
                                     << ": подключение уже установлено.";
        return false;
    }
}
