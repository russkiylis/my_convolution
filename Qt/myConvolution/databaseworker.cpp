#include <QDebug>
#include "databaseworker.h"

DatabaseWorker::DatabaseWorker(QString connectionName, QObject *parent)
    : QObject{parent},
    _connectionName(connectionName)
{
}

void DatabaseWorker::handleConnect()
{
    bool result;
    // smth
    qDebug() << "Я в другом потоке!";

    emit signalConnected(result);
}


