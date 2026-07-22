#include <QDebug>
#include <QtSql/QSqlDatabase>
#include <QSqlError>
#include <utility>
#include "databaseworker.h"

DatabaseConfiguration::DatabaseConfiguration(const DatabaseConfiguration &other) = default;

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
                                             QString connectOptions) :    connectionName(connectionName),
                                                                                 hostName(hostName),
                                                                                 port(port),
                                                                                 dbName(dbName),
                                                                                 userName(userName),
                                                                                 password(password),
                                                                                 fullConnectionName("\"" + connectionName
                                                                                     + "\": pg://" + userName
                                                                                     + ":" + password
                                                                                     + "@" + hostName
                                                                                     + ":" + QString::number(port)
                                                                                     + "/" + dbName),
                                                                                connectOptions(std::move(connectOptions))
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
    emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
}

void DatabaseWorker::slotInitialize()
{
    _busy = true;
    emit signalManagerUpdate(_connected, _valid, _busy, _lastError);

    // Проверка на существование подключения с таким же именем
    if (QSqlDatabase::contains(_config.connectionName)) {
        _lastError = "[!] Подключение с именем \""
                    +_config.connectionName + "\" "
                    +"уже существует. Объект подключения не валиден.";
        qDebug().noquote().nospace() << _lastError;
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", _config.connectionName); // Создали подключение к БД
    // FIXME: мы валидность делаем даже если нет драйверов
    _valid = true;

    qDebug().noquote().nospace() << "Создан объект подключения \""
                                 << _config.connectionName << "\"";
    _lastError = "Ошибок нет.";
    update();

    _busy = false;
    emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
}

void DatabaseWorker::slotOpenConnection()
{
    _busy = true;
    emit signalManagerUpdate(_connected, _valid, _busy, _lastError);

    if (!_valid) {
        _lastError = "[!] Объект подключения не валиден. Невозможно открыть соединение.";
        qDebug().noquote().nospace() << _lastError;
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }
    if (_connected) {
        _lastError = "[!] "
                 + _config.fullConnectionName
                 + ": Соединение не открыто: "
                 + "попытка повторного открытия соединения!";
        qDebug().noquote().nospace() << _lastError;
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    // Получаем объект для работы с БД
    QSqlDatabase db = QSqlDatabase::database(_config.connectionName);

    _connected = db.open();    // Попытка открыть физическое соединение
    if (_connected) {
        qDebug().noquote().nospace() << _config.fullConnectionName
                                     << ": Соединение открыто!";
        _lastError = "Ошибок нет.";
    } else {
        _lastError = "[!] "
                    + _config.fullConnectionName
                    + ": Соединение не открыто: "
                    + db.lastError().text();
        qDebug().noquote().nospace() << _lastError;
    }

    _busy = false;
    emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
}

void DatabaseWorker::slotCloseConnection() {
    _busy = true;
    emit signalManagerUpdate(_connected, _valid, _busy, _lastError);

    if (!_valid) {
        _lastError = "[!] Объект подключения не валиден. Невозможно закрыть соединение.";
        qDebug().noquote().nospace() << _lastError;
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }
    if (!_connected) {
        _lastError = "[!] "
                 + _config.fullConnectionName
                 + ": Соединение не закрыто: "
                 + "попытка закрытия закрытого соединения!";
        qDebug().noquote().nospace() << _lastError;
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    // Получаем объект для работы с БД
    QSqlDatabase db = QSqlDatabase::database(_config.connectionName);

    db.close();    // Попытка закрыть физическое соединение
    _connected = false;
    qDebug().noquote().nospace() << _config.fullConnectionName
                                 << ": Соединение закрыто!";
    _lastError = "Ошибок нет.";

    _busy = false;
    emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
}

void DatabaseWorker::slotConfigUpdate(const DatabaseConfiguration & new_config)
{
    // FIXME: Мб тут тоже что то нужно попроверять
    _busy = true;
    emit signalManagerUpdate(_connected, _valid, _busy, _lastError);

    if (!_valid) {
        _lastError = "[!] "
                     + _config.fullConnectionName
                     + ": объект подключения не валиден. "
                     + "Невозможно изменить параметры подключения.";
        qDebug().noquote().nospace() << _lastError;
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }
    if (_connected) {
        _lastError = "[!] "
                     + _config.fullConnectionName
                     + ": невозможно изменить параметры "
                     + "открытого соединения.";
        qDebug().noquote().nospace() << _lastError;
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    _config = new_config;
    update();   // Обновляем подключения QSql

    _busy = false;
    _lastError = "Ошибок нет.";
    emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
}
