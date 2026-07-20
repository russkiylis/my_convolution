#include "backend.h"
#include <QDebug>   // для работы с консолью

int ConnectionBackend::dbStatus() const {
    return m_dbStatus;
}

QString ConnectionBackend::lastError() const
{
    return m_db.lastError();
}

QString ConnectionBackend::hostName() const {
    return m_hostName;
}

void ConnectionBackend::setHostName(const QString &host_name) {
    m_hostName = host_name;
    emit hostNameChanged(m_hostName);
}

QString ConnectionBackend::port() const {
    return m_port;
}

void ConnectionBackend::setPort(const QString &port) {
    m_port = port;
    emit portChanged(m_port);
}

QString ConnectionBackend::userName() const {
    return m_userName;
}

void ConnectionBackend::setUserName(const QString &user_name) {
    m_userName = user_name;
    emit userNameChanged(m_userName);
}

QString ConnectionBackend::password() const {
    return m_password;
}

void ConnectionBackend::setPassword(const QString &password) {
    m_password = password;
    emit passwordChanged(m_password);
}

QString ConnectionBackend::databaseName() const {
    return m_databaseName;
}

void ConnectionBackend::setDatabaseName(const QString &database_name) {
    m_databaseName = database_name;
    emit databaseNameChanged(m_databaseName);
}

QString ConnectionBackend::connectOptions() const {
    return m_connectOptions;
}

void ConnectionBackend::setConnectOptions(const QString &connect_options) {
    m_connectOptions = connect_options;
    emit connectOptionsChanged(m_connectOptions);
}

void ConnectionBackend::setDbStatus(int const &dbStatus) {
    if (m_dbStatus == dbStatus)
        return;
    m_dbStatus = dbStatus;

    emit dbStatusChanged(dbStatus);
}

void ConnectionBackend::setLastError(QString const &lastError) {
    if (m_lastError == lastError)
        return;
    m_lastError = lastError;

    emit lastErrorChanged(m_lastError);
}

ConnectionBackend::ConnectionBackend(QObject *parent)
    : QObject{parent},
    m_db(this)
{}

void ConnectionBackend::onDbConnectionButtonClicked(QString hostName, QString port, QString userName, QString password,
                                          QString databaseName, QString connectOptions)
{
    // FIXME: Если попытаться подключиться к неправильной базке, а затем к правильной, то перед тем как подключиться к правильной, программа попробует подключиться к неправильной
    qDebug() << "Кнопка подключения нажата.";

    if (m_dbStatus != 2) {
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

        m_db.setHostName(hostName);
        m_db.setPort(port.toInt());
        m_db.setUserName(userName);
        m_db.setPassword(password);
        m_db.setDbName(databaseName);
        m_db.setConnectOptions(connectOptions);

        m_db.openConnection();
    } else {
        // Если БД подключена, то отключаем
        m_db.closeConnection();
    }
}