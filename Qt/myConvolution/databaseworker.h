#pragma once

#include <QObject>
#include <QString>

// Класс, непосредственно работающий с базой данных.
// Сидит в другом потоке чтобы не мешать остальной программе.

// Конфигурация базы данных
struct DatabaseConfiguration {
    DatabaseConfiguration(const DatabaseConfiguration &other);

    DatabaseConfiguration & operator=(const DatabaseConfiguration &other);

    DatabaseConfiguration & operator=(DatabaseConfiguration &&other) noexcept {
        if (this == &other)
            return *this;
        hostName = std::move(other.hostName);
        port = other.port;
        dbName = std::move(other.dbName);
        userName = std::move(other.userName);
        password = std::move(other.password);
        fullConnectionName = std::move(other.fullConnectionName);
        connectOptions = std::move(other.connectOptions);
        return *this;
    }

    explicit DatabaseConfiguration(QString const & connectionName = "myConvolution",
                                   QString const & hostName = "127.0.0.1",
                                   QString const & dbName = "my_convolution",
                                   QString const & userName = "russkiylis",
                                   QString const & password = "1337",
                                   int const & port = 5432,
                                   QString const & connectOptions = "connect_timeout = 3");
    QString const connectionName;
    QString hostName;
    int port;
    QString dbName;
    QString userName;
    QString password;
    QString fullConnectionName;
    QString connectOptions;
};

class DatabaseWorker : public QObject
{
    Q_OBJECT

    DatabaseConfiguration _config;

    bool _connected = false;
    bool _valid = false;
    bool _busy = false;

    // Обновление параметров базы данных QSqlDatabase
    void update();

    // Обновить полное имя подключения
    void setFullConnectionName();

public:
    explicit DatabaseWorker(const DatabaseConfiguration &database_configuration, QObject *parent = nullptr);

public slots:

    // Принудительный запрос обновления полей менеджера
    void slotManagerUpdate();

    // Инициализация объекта подключения (подключение к QSql)
    void slotInitialize();

    // Подключение к базе данных
    void slotOpenConnection();

    // Обновление конфигурации подключения
    void slotConfigUpdate(const DatabaseConfiguration & new_config);


signals:

    // Отправка сигнала об обновлении полей менеджера
    void signalManagerUpdate(bool connected, bool valid, bool busy);
};

