#include <QDebug>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QString>
#include "databasemanager.h"
#include "databaseworker.h"

DatabaseManager::DatabaseManager(QString const & connectionName,
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
        return;
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", _connectionName); // Создали подключение к БД
    _valid = true;
    update();

    DatabaseWorker *worker = new DatabaseWorker(_connectionName);   // Создаём объект рабочего класса
    worker->moveToThread(&workerThread);    // Переносим объект рабочего класса в другой поток

    //Соединение сигналов и слотов
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater); // Когда рабочий поток завершится, объект worker удалится
    connect(this, &DatabaseManager::signalOpenConnection, worker, &DatabaseWorker::handleConnect); // Когда менеджер пошлёт сигнал, вызвать функцию подключения в рабочем классе
    connect(worker, &DatabaseWorker::signalConnected, this, &DatabaseManager::handleConnected);  // Когда рабочий послал сигнал о результате подключения, тут вызовется соответствующий метод
    workerThread.start();

    qDebug().noquote().nospace() << "Создан объект подключения "
                                 << _fullConnectionName;

}

void DatabaseManager::setHostName(QString const & value) {
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

void DatabaseManager::setPort(int const & value) {
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

void DatabaseManager::setDbName(QString const & value) {
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

void DatabaseManager::setUserName(QString const & value) {
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

void DatabaseManager::setPassword(QString const & value) {
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

void DatabaseManager::setConnectOptions(QString const & value) {
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

void DatabaseManager::setFullConnectionName() {
    _fullConnectionName = "\"" + _connectionName
                          + "\": pg://" + _userName
                          + ":" + _password
                          + "@" + _hostName
                          + ":" + QString::number(_port)
                          + "/" + _dbName;
}

void DatabaseManager::update() {
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

void DatabaseManager::handleConnected(const bool &result)
{
    _connected = result;
}

void DatabaseManager::openConnection() {
    if (!_valid) {
        qDebug().noquote().nospace() << "[!] "
                                     << _fullConnectionName
                                     << ": объект подключения не валиден."
                                     << "Невозможно открыть соединение.";
        return;
    }

    if (!_connected) {
        // QSqlDatabase db = QSqlDatabase::database(_connectionName);
        // _connected = db.open();    // Попытка наладить физическое соединение
        // if (!_connected) {
        //     qDebug().noquote().nospace() << "[!] "
        //                                  << _fullConnectionName
        //                                  << ": не удалось подключиться к БД: "
        //                                  << db.lastError().text();
        // } else {
        //     qDebug().noquote().nospace() << _fullConnectionName
        //                                  << ": установлено подключение к БД!";
        // }
        // return _connected;
        qDebug() << "Запускаю сигнал...";
        emit signalOpenConnection();

    } else {
        qDebug().noquote().nospace() << "[!] "
                                     << _fullConnectionName
                                     << ": подключение уже установлено.";
    }
}
