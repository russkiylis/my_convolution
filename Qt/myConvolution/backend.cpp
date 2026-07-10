#include "backend.h"
#include <QDebug>   // для работы с консолью

int Backend::dbStatus() const {
    return static_cast<int>(_dbStatus);
}

QString Backend::lastError()
{
    return _db.lastError();
}

void Backend::setDbStatus(int const &dbStatus) {
    if (_dbStatus == dbStatus)
        return;
    _dbStatus = static_cast<int>(dbStatus);

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

void Backend::onDbConnectionButtonClicked()
{
    qDebug() << "Кнопка подключения нажата.";

    _db.openConnection();
}
