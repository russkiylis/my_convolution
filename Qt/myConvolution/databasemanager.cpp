#include <QDebug>
#include <QString>
#include "databasemanager.h"
#include "databaseworker.h"
#include "backend.h"

DatabaseManager::DatabaseManager(ConnectionBackend *backend,
                                       QString const & connectionName,
                                       QString const & hostName,
                                       QString const & dbName,
                                       QString const & userName,
                                       QString const & password,
                                       int const & port,
                                       QString const & connectOptions,
                                       QObject * parent)
    :QObject{parent},
    m_backend(backend),
    m_connectionName(connectionName),
    m_hostName(hostName),
    m_port(5432),
    m_dbName(dbName),
    m_userName(userName),
    m_password(password),
    m_fullConnectionName("\"" + m_connectionName
        + "\": pg://" + m_userName
        + ":" + m_password
        + "@" + m_hostName
        + ":" + QString::number(m_port)
        + "/" + m_dbName),
    m_connectOptions(connectOptions)
{
    const auto dbConfig = DatabaseConfiguration(connectionName, hostName, dbName, userName, password, port,
                                                           connectOptions);

    auto *worker = new DatabaseWorker(dbConfig);   // Создаём объект рабочего класса
    worker->moveToThread(&m_workerThread);    // Переносим объект рабочего класса в другой поток

    //Соединение сигналов и слотов
    connect(&m_workerThread, &QThread::finished, worker, &QObject::deleteLater); // Когда рабочий поток завершится, объект worker удалится
    connect(this, &DatabaseManager::signalConfigUpdate, worker, &DatabaseWorker::slotConfigUpdate);
    connect(this, &DatabaseManager::signalOpenConnection, worker, &DatabaseWorker::slotOpenConnection);
    connect(this, &DatabaseManager::signalCloseConnection, worker, &DatabaseWorker::slotCloseConnection);
    connect(this, &DatabaseManager::signalManagerUpdate, worker, &DatabaseWorker::slotManagerUpdate);
    connect(this, &DatabaseManager::signalInitialize, worker, &DatabaseWorker::slotInitialize);
    connect(worker, &DatabaseWorker::signalManagerUpdate, this, &DatabaseManager::slotManagerUpdate);
    m_workerThread.start();

    // Проинициализируем рабочий объект
    emit signalInitialize();

}

DatabaseManager::~DatabaseManager() {
    // Адекватная остановка рабочего потока при уничтожении управляющего рабочим объекта
    m_workerThread.quit();
    m_workerThread.wait();
}

void DatabaseManager::setHostName(QString const & value) {
    if (!m_valid) {
        qDebug().noquote().nospace() << "[!] "
                                     << m_fullConnectionName
                                     << ": объект подключения не валиден. "
                                     << "Невозможно изменить параметры подключения.";
        return;
    }
    if (m_connected) {
        qDebug().noquote().nospace() << "[!] "
                                     << m_fullConnectionName
                                     << ": невозможно изменить параметры "
                                        "открытого соединения.";
        return;
    }
    if (m_hostName == value)
        return;
    m_hostName = value;
    setFullConnectionName();

    // Создаём новый конфиг и посылаем его в рабочий класс
    const auto dbConfig = DatabaseConfiguration(m_connectionName, m_hostName, m_dbName, m_userName, m_password, m_port,
                                                           m_connectOptions);
    emit signalConfigUpdate(dbConfig);
}

void DatabaseManager::setPort(int const & value) {
    if (!m_valid) {
        qDebug().noquote().nospace() << "[!] "
                                     << m_fullConnectionName
                                     << ": объект подключения не валиден. "
                                     << "Невозможно изменить параметры подключения.";
        return;
    }
    if (m_connected) {
        qDebug().noquote().nospace() << "[!] "
                                     << m_fullConnectionName
                                     << ": невозможно изменить параметры "
                                        "открытого соединения.";
        return;
    }
    if (m_port == value)
        return;
    m_port = value;
    setFullConnectionName();

    // Создаём новый конфиг и посылаем его в рабочий класс
    const auto dbConfig = DatabaseConfiguration(m_connectionName, m_hostName, m_dbName, m_userName, m_password, m_port,
                                                           m_connectOptions);
    emit signalConfigUpdate(dbConfig);
}

void DatabaseManager::setDbName(QString const & value) {
    if (!m_valid) {
        qDebug().noquote().nospace() << "[!] "
                                     << m_fullConnectionName
                                     << ": объект подключения не валиден. "
                                     << "Невозможно изменить параметры подключения.";
        return;
    }
    if (m_connected) {
        qDebug().noquote().nospace() << "[!] "
                                     << m_fullConnectionName
                                     << ": невозможно изменить параметры "
                                        "открытого соединения.";
        return;
    }
    if (m_dbName == value)
        return;
    m_dbName = value;
    setFullConnectionName();

    // Создаём новый конфиг и посылаем его в рабочий класс
    const auto dbConfig = DatabaseConfiguration(m_connectionName, m_hostName, m_dbName, m_userName, m_password, m_port,
                                                           m_connectOptions);
    emit signalConfigUpdate(dbConfig);
}

void DatabaseManager::setUserName(QString const & value) {
    if (!m_valid) {
        qDebug().noquote().nospace() << "[!] "
                                     << m_fullConnectionName
                                     << ": объект подключения не валиден. "
                                     << "Невозможно изменить параметры подключения.";
        return;
    }
    if (m_connected) {
        qDebug().noquote().nospace() << "[!] "
                                     << m_fullConnectionName
                                     << ": невозможно изменить параметры "
                                        "открытого соединения.";
        return;
    }
    if (m_userName == value)
        return;
    m_userName = value;
    setFullConnectionName();

    // Создаём новый конфиг и посылаем его в рабочий класс
    const auto dbConfig = DatabaseConfiguration(m_connectionName, m_hostName, m_dbName, m_userName, m_password, m_port,
                                                           m_connectOptions);
    emit signalConfigUpdate(dbConfig);
}

void DatabaseManager::setPassword(QString const & value) {
    if (!m_valid) {
        qDebug().noquote().nospace() << "[!] "
                                     << m_fullConnectionName
                                     << ": объект подключения не валиден. "
                                     << "Невозможно изменить параметры подключения.";
        return;
    }
    if (m_connected) {
        qDebug().noquote().nospace() << "[!] "
                                     << m_fullConnectionName
                                     << ": невозможно изменить параметры "
                                        "открытого соединения.";
        return;
    }
    if (m_password == value)
        return;
    m_password = value;
    setFullConnectionName();

    // Создаём новый конфиг и посылаем его в рабочий класс
    const auto dbConfig = DatabaseConfiguration(m_connectionName, m_hostName, m_dbName, m_userName, m_password, m_port,
                                                           m_connectOptions);
    emit signalConfigUpdate(dbConfig);
}

void DatabaseManager::setConnectOptions(QString const & value) {
    if (!m_valid) {
        qDebug().noquote().nospace() << "[!] "
                                     << m_fullConnectionName
                                     << ": объект подключения не валиден. "
                                     << "Невозможно изменить параметры подключения.";
        return;
    }
    if (m_connected) {
        qDebug().noquote().nospace() << "[!] "
                                     << m_fullConnectionName
                                     << ": невозможно изменить параметры "
                                        "открытого соединения.";
        return;
    }
    if (m_connectOptions == value)
        return;
    m_connectOptions = value;
    setFullConnectionName();

    // Создаём новый конфиг и посылаем его в рабочий класс
    const auto dbConfig = DatabaseConfiguration(m_connectionName, m_hostName, m_dbName, m_userName, m_password, m_port,
                                                           m_connectOptions);
    emit signalConfigUpdate(dbConfig);
}

void DatabaseManager::setFullConnectionName() {
    m_fullConnectionName = "\"" + m_connectionName
                          + "\": pg://" + m_userName
                          + ":" + m_password
                          + "@" + m_hostName
                          + ":" + QString::number(m_port)
                          + "/" + m_dbName;
}

void DatabaseManager::openConnection() {
    if (!m_valid) {
        QString errorText = "[!] "
                            + m_fullConnectionName
                            + ": объект подключения не валиден. "
                            + "Невозможно открыть соединение.";
        qDebug().noquote().nospace()  << errorText;
    }

    // В рабочий поток посылаем команду установить соединение
    emit signalOpenConnection();
}

void DatabaseManager::closeConnection() {
    if (!m_valid) {
        QString errorText = "[!] "
                            + m_fullConnectionName
                            + ": объект подключения не валиден. "
                            + "Невозможно закрыть соединение.";
        qDebug().noquote().nospace()  << errorText;
    }

    // В рабочий поток посылаем команду закрыть соединение
    emit signalCloseConnection();
}

void DatabaseManager::slotManagerUpdate(bool const &connected, bool const &valid, bool const &busy, QString const &lastError) {
    m_connected = connected;
    m_valid = valid;
    m_busy = busy;
    m_lastError = lastError;

    int dbStatus = 0;
    if (busy) {
        dbStatus = 1;
    } else if (!m_valid || !m_connected) {
        dbStatus = 0;
    } else {
        dbStatus = 2;
    }

    m_backend->setLastError(m_lastError);
    m_backend->setDbStatus(dbStatus);
}
