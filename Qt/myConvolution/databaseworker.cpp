#include <QDebug>
#include <QtSql/QSqlDatabase>
#include <QSqlError>
#include "databaseworker.h"

DatabaseConfiguration::DatabaseConfiguration(const DatabaseConfiguration &other): hostName(other.hostName),
    connectionName(other.connectionName),
    port(other.port),
    dbName(other.dbName),
    userName(other.userName),
    password(other.password),
    fullConnectionName(other.fullConnectionName),
    connectOptions(other.connectOptions) {
}

DatabaseConfiguration & DatabaseConfiguration::operator=(const DatabaseConfiguration &other) {
    if (this == &other)
        return *this;
    hostName = other.hostName;
    port = other.port;
    dbName = other.dbName;
    userName = other.userName;
    password = other.password;
    fullConnectionName = other.fullConnectionName;
    connectOptions = other.connectOptions;
    return *this;
}

DatabaseConfiguration::DatabaseConfiguration(QString const &connectionName, QString const &hostName,
                                             QString const &dbName, QString const &userName, QString const &password, int const &port,
                                             QString const &connectOptions) :    connectionName(connectionName),
                                                                                 hostName(hostName),
                                                                                 dbName(dbName),
                                                                                 userName(userName),
                                                                                 password(password),
                                                                                 port(port),
                                                                                 connectOptions(connectOptions),
                                                                                fullConnectionName("\"" + connectionName
                                                                                                        + "\": pg://" + userName
                                                                                                        + ":" + password
                                                                                                        + "@" + hostName
                                                                                                        + ":" + QString::number(port)
                                                                                                        + "/" + dbName)
{
}

void DatabaseWorker::update()
{
    if (!_valid) {
        qDebug().noquote().nospace() << "[!] "
                                     << _config.fullConnectionName
                                     << ": объект подключения не валиден. "
                                     << "Невозможно изменить параметры подключения.";
        return;
    }
    if (_connected) {
        qDebug().noquote().nospace() << "[!] "
                                     << _config.fullConnectionName
                                     << ": невозможно изменить параметры "
                                        "открытого соединения.";
        return;
    }

    QSqlDatabase db = QSqlDatabase::database(_config.connectionName);
    db.setHostName(_config.hostName);
    db.setPort(_config.port);
    db.setDatabaseName(_config.dbName);
    db.setUserName(_config.userName);
    db.setPassword(_config.password);
    db.setConnectOptions(_config.connectOptions);
    setFullConnectionName();
    qDebug().noquote().nospace() << _config.fullConnectionName
                                 << ": параметры подключения изменены.";
}

void DatabaseWorker::setFullConnectionName() {
    _config.fullConnectionName = "\"" + _config.connectionName
                      + "\": pg://" + _config.userName
                      + ":" + _config.password
                      + "@" + _config.hostName
                      + ":" + QString::number(_config.port)
                      + "/" + _config.dbName;
}

DatabaseWorker::DatabaseWorker(const DatabaseConfiguration &database_configuration, QObject *parent)
    : QObject{parent},
    _config(database_configuration)
{

}

void DatabaseWorker::slotManagerUpdate()
{
    emit signalManagerUpdate(_connected, _valid, _busy);
}

void DatabaseWorker::slotInitialize()
{
    _busy = true;
    emit signalManagerUpdate(_connected, _valid, _busy);

    // Проверка на существование подключения с таким же именем
    if (QSqlDatabase::contains(_config.connectionName)) {
        qDebug().noquote().nospace() << "[!] Подключение с именем \""
                                     << _config.connectionName << "\" "
                                     << "уже существует. Объект подключения не валиден.";
        return;
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", _config.connectionName); // Создали подключение к БД
    _valid = true;

    qDebug().noquote().nospace() << "Создан объект подключения \""
                                 << _config.connectionName << "\"";

    _busy = false;
    emit signalManagerUpdate(_connected, _valid, _busy);
}

void DatabaseWorker::slotOpenConnection()
{
    _busy = true;
    emit signalManagerUpdate(_connected, _valid, _busy);

    if (!_valid) {
        qDebug().noquote().nospace() << "[!] "
                                     << _config.fullConnectionName
                                     << ": объект подключения не валиден. "
                                     << "Невозможно открыть соединение.";
        return;
    }
    if (_connected) {
        qDebug().noquote().nospace() << "[!] "
                 << _config.fullConnectionName
                 << ": Соединение не открыто: "
                 << "попытка повторного открытия соединения!";

        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy);
        return;
    }

    // Получаем объект для работы с БД
    QSqlDatabase db = QSqlDatabase::database(_config.connectionName);

    _connected = db.open();    // Попытка открыть физическое соединение
    if (_connected) {
        qDebug().noquote().nospace() << _config.fullConnectionName
                                     << ": Соединение открыто!";
    } else {
        qDebug().noquote().nospace() << "[!] "
                         << _config.fullConnectionName
                         << ": Соединение не открыто: "
                         << db.lastError().text();
    }

    _busy = false;
    emit signalManagerUpdate(_connected, _valid, _busy);
}

void DatabaseWorker::slotConfigUpdate(const DatabaseConfiguration & new_config)
{
    _busy = true;
    emit signalManagerUpdate(_connected, _valid, _busy);

    if (!_valid) {
        qDebug().noquote().nospace() << "[!] "
                                     << _config.fullConnectionName
                                     << ": объект подключения не валиден. "
                                     << "Невозможно изменить параметры подключения.";
        return;
    }
    if (_connected) {
        qDebug().noquote().nospace() << "[!] "
                                     << _config.fullConnectionName
                                     << ": невозможно изменить параметры "
                                        "открытого соединения.";
        return;
    }

    _config = new_config;
    update();   // Обновляем подключения QSql

    _busy = false;
    emit signalManagerUpdate(_connected, _valid, _busy);
}


