#pragma once

#include <QObject>
#include <QString>

// Класс, непосредственно работающий с базой данных.
// Сидит в другом потоке чтобы не мешать остальной программе.
class DatabaseWorker : public QObject
{
    Q_OBJECT

    QString _connectionName;

public:
    explicit DatabaseWorker(QString connectionName, QObject *parent = nullptr);

public slots:
    // Подключение к базе данных
    void handleConnect();

signals:
    void signalConnected(bool const & result);
};

