#include "backend.h"
#include <QDebug>   // для работы с консолью

int Backend::dbStatus() const {
    return _dbStatus;
}

QString Backend::lastError()
{
    return _db.lastError();
}

QString Backend::hostName() const {
    return _hostName;
}

void Backend::setHostName(const QString &host_name) {
    _hostName = host_name;
    emit hostNameChanged(_hostName);
}

QString Backend::port() const {
    return _port;
}

void Backend::setPort(const QString &port) {
    _port = port;
    emit portChanged(_port);
}

QString Backend::userName() const {
    return _userName;
}

void Backend::setUserName(const QString &user_name) {
    _userName = user_name;
    emit userNameChanged(_userName);
}

QString Backend::password() const {
    return _password;
}

void Backend::setPassword(const QString &password) {
    _password = password;
    emit passwordChanged(_password);
}

QString Backend::databaseName() const {
    return _databaseName;
}

void Backend::setDatabaseName(const QString &database_name) {
    _databaseName = database_name;
    emit databaseNameChanged(_databaseName);
}

QString Backend::connectOptions() const {
    return _connectOptions;
}

void Backend::setConnectOptions(const QString &connect_options) {
    _connectOptions = connect_options;
    emit connectOptionsChanged(_connectOptions);
}

void Backend::setDbStatus(int const &dbStatus) {
    if (_dbStatus == dbStatus)
        return;
    _dbStatus = dbStatus;

    emit dbStatusChanged(dbStatus);
}

void Backend::setLastError(QString const &lastError) {
    if (_lastError == lastError)
        return;
    _lastError = lastError;

    emit lastErrorChanged(_lastError);
}

Backend::Backend(QObject *parent)
    : QObject{parent},
    _db(this)
{}

void Backend::onDbConnectionButtonClicked(QString hostName, QString port, QString userName, QString password,
                                          QString databaseName, QString connectOptions)
{
    // FIXME: Если попытаться подключиться к неправильной базке, а затем к правильной, то перед тем как подключиться к правильной, программа попробует подключиться к неправильной
    qDebug() << "Кнопка подключения нажата.";

    if (_dbStatus != 2) {
        // Если не БД не подключена, то подключаем
        if (hostName == "")
            hostName = "127.0.0.1";
        if (port == "")
            port = "5432";
        if (userName == "")
            userName = "russkiylis";
        if (password == "")
            password = "1337";
        if (databaseName == "")
            databaseName = "my_convolution";
        if (connectOptions == "")
            connectOptions = "connect_timeout = 3";

        _db.setHostName(hostName);
        _db.setPort(port.toInt());
        _db.setUserName(userName);
        _db.setPassword(password);
        _db.setDbName(databaseName);
        _db.setConnectOptions(connectOptions);

        _db.openConnection();
    } else {
        // Если БД подключена, то отключаем
        _db.closeConnection();
    }
}