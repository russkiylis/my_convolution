#include <QDebug>
#include <QString>
#include "databasemanager.h"
#include "databaseworker.h"
#include "backend.h"

DatabaseManager::DatabaseManager(Backend *backend,
                                       QString const & connectionName,
                                       QString const & hostName,
                                       QString const & dbName,
                                       QString const & userName,
                                       QString const & password,
                                       int const & port,
                                       QString const & connectOptions,
                                       QObject * parent)
    :QObject{parent},
    _backend(backend),
    _connectionName(connectionName),
    _hostName(hostName),
    _port(5432),
    _dbName(dbName),
    _userName(userName),
    _password(password),
    _fullConnectionName("\"" + _connectionName
        + "\": pg://" + _userName
        + ":" + _password
        + "@" + _hostName
        + ":" + QString::number(_port)
        + "/" + _dbName),
    _connectOptions(connectOptions)
{
    const auto dbConfig = DatabaseConfiguration(connectionName, hostName, dbName, userName, password, port,
                                                           connectOptions);

    auto *worker = new DatabaseWorker(dbConfig);   // Создаём объект рабочего класса
    worker->moveToThread(&workerThread);    // Переносим объект рабочего класса в другой поток

    //Соединение сигналов и слотов
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater); // Когда рабочий поток завершится, объект worker удалится
    connect(this, &DatabaseManager::signalConfigUpdate, worker, &DatabaseWorker::slotConfigUpdate);
    connect(this, &DatabaseManager::signalOpenConnection, worker, &DatabaseWorker::slotOpenConnection);
    connect(this, &DatabaseManager::signalCloseConnection, worker, &DatabaseWorker::slotCloseConnection);
    connect(this, &DatabaseManager::signalManagerUpdate, worker, &DatabaseWorker::slotManagerUpdate);
    connect(this, &DatabaseManager::signalInitialize, worker, &DatabaseWorker::slotInitialize);
    connect(worker, &DatabaseWorker::signalManagerUpdate, this, &DatabaseManager::slotManagerUpdate);
    workerThread.start();

    // Проинициализируем рабочий объект
    emit signalInitialize();

}

DatabaseManager::~DatabaseManager() {
    // Адекватная остановка рабочего потока при уничтожении управляющего рабочим объекта
    workerThread.quit();
    workerThread.wait();
}

void DatabaseManager::setHostName(QString const & value) {
    if (!_valid) {
        qDebug().noquote().nospace() << "[!] "
                                     << _fullConnectionName
                                     << ": объект подключения не валиден. "
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
    setFullConnectionName();

    // Создаём новый конфиг и посылаем его в рабочий класс
    const auto dbConfig = DatabaseConfiguration(_connectionName, _hostName, _dbName, _userName, _password, _port,
                                                           _connectOptions);
    emit signalConfigUpdate(dbConfig);
}

void DatabaseManager::setPort(int const & value) {
    if (!_valid) {
        qDebug().noquote().nospace() << "[!] "
                                     << _fullConnectionName
                                     << ": объект подключения не валиден. "
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
    setFullConnectionName();

    // Создаём новый конфиг и посылаем его в рабочий класс
    const auto dbConfig = DatabaseConfiguration(_connectionName, _hostName, _dbName, _userName, _password, _port,
                                                           _connectOptions);
    emit signalConfigUpdate(dbConfig);
}

void DatabaseManager::setDbName(QString const & value) {
    if (!_valid) {
        qDebug().noquote().nospace() << "[!] "
                                     << _fullConnectionName
                                     << ": объект подключения не валиден. "
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
    setFullConnectionName();

    // Создаём новый конфиг и посылаем его в рабочий класс
    const auto dbConfig = DatabaseConfiguration(_connectionName, _hostName, _dbName, _userName, _password, _port,
                                                           _connectOptions);
    emit signalConfigUpdate(dbConfig);
}

void DatabaseManager::setUserName(QString const & value) {
    if (!_valid) {
        qDebug().noquote().nospace() << "[!] "
                                     << _fullConnectionName
                                     << ": объект подключения не валиден. "
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
    setFullConnectionName();

    // Создаём новый конфиг и посылаем его в рабочий класс
    const auto dbConfig = DatabaseConfiguration(_connectionName, _hostName, _dbName, _userName, _password, _port,
                                                           _connectOptions);
    emit signalConfigUpdate(dbConfig);
}

void DatabaseManager::setPassword(QString const & value) {
    if (!_valid) {
        qDebug().noquote().nospace() << "[!] "
                                     << _fullConnectionName
                                     << ": объект подключения не валиден. "
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
    setFullConnectionName();

    // Создаём новый конфиг и посылаем его в рабочий класс
    const auto dbConfig = DatabaseConfiguration(_connectionName, _hostName, _dbName, _userName, _password, _port,
                                                           _connectOptions);
    emit signalConfigUpdate(dbConfig);
}

void DatabaseManager::setConnectOptions(QString const & value) {
    if (!_valid) {
        qDebug().noquote().nospace() << "[!] "
                                     << _fullConnectionName
                                     << ": объект подключения не валиден. "
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
    setFullConnectionName();

    // Создаём новый конфиг и посылаем его в рабочий класс
    const auto dbConfig = DatabaseConfiguration(_connectionName, _hostName, _dbName, _userName, _password, _port,
                                                           _connectOptions);
    emit signalConfigUpdate(dbConfig);
}

void DatabaseManager::setFullConnectionName() {
    _fullConnectionName = "\"" + _connectionName
                          + "\": pg://" + _userName
                          + ":" + _password
                          + "@" + _hostName
                          + ":" + QString::number(_port)
                          + "/" + _dbName;
}

void DatabaseManager::openConnection() {
    if (!_valid) {
        QString errorText = "[!] "
                            + _fullConnectionName
                            + ": объект подключения не валиден. "
                            + "Невозможно открыть соединение.";
        qDebug().noquote().nospace()  << errorText;
    }

    // В рабочий поток посылаем команду установить соединение
    emit signalOpenConnection();
}

void DatabaseManager::closeConnection() {
    if (!_valid) {
        QString errorText = "[!] "
                            + _fullConnectionName
                            + ": объект подключения не валиден. "
                            + "Невозможно закрыть соединение.";
        qDebug().noquote().nospace()  << errorText;
    }

    // В рабочий поток посылаем команду закрыть соединение
    emit signalCloseConnection();
}

void DatabaseManager::slotManagerUpdate(bool const &connected, bool const &valid, bool const &busy, QString const &lastError) {
    _connected = connected;
    _valid = valid;
    _busy = busy;
    _lastError = lastError;

    int dbStatus = 0;
    if (busy) {
        dbStatus = 1;
    } else if (!_valid || !_connected) {
        dbStatus = 0;
    } else {
        dbStatus = 2;
    }

    _backend->setLastError(_lastError);
    _backend->setDbStatus(dbStatus);
}
