#pragma once

#include <QObject>
#include <QString>
#include <QThread>
#include "databaseworker.h"

class ConnectionBackend;

// Класс для работы с БД
// У него есть рабочий в другом потоке
class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    // Конструктор
    explicit DatabaseManager(ConnectionBackend *backend,
                                QString const & connectionName = "myConvolution",
                                QString const & hostName = "127.0.0.1",
                                QString const & dbName = "my_convolution",
                                QString const & userName = "russkiylis",
                                QString const & password = "1337",
                                int const & port = 5432,
                                QString const & connectOptions = "connect_timeout = 3",
                                QObject * parent = nullptr);

    // Деструктор
    ~DatabaseManager() override;

    // Имя подключения (не путать с именем БД)
    [[nodiscard]] QString connectionName() const
    {
        return _connectionName;
    }

    // Адрес хоста
    [[nodiscard]] QString hostName() const
    {
        return _hostName;
    }

    // Порт
    [[nodiscard]] int port() const
    {
        return _port;
    }

    // Имя БД
    [[nodiscard]] QString dbName() const
    {
        return _dbName;
    }

    // Имя пользователя
    [[nodiscard]] QString userName() const
    {
        return _userName;
    }

    // Пароль
    [[nodiscard]] QString password() const
    {
        return _password;
    }

    // Полное имя подключения
    [[nodiscard]] QString fullConnectionName() const
    {
        return _fullConnectionName;
    }

    // Настройки подключения
    [[nodiscard]] QString connectOptions() const
    {
        return _connectOptions;
    }

    // Валиден ли объект
    [[nodiscard]] bool valid() const
    {
        return _valid;
    }

    // Занято ли подключение к БД
    [[nodiscard]] bool busy() const
    {
        return _busy;
    }

    // Установлено ли подключение к БД
    [[nodiscard]] bool connected() const
    {
        return _connected;
    }

    // Последняя ошибка БД
    [[nodiscard]] QString lastError() const
    {
        return _lastError;
    }

    // Задать адрес хоста
    void setHostName(QString const & value);

    // Задать порт
    void setPort(int const & value);

    // Задать имя БД
    void setDbName(QString const & value);

    // Задать имя пользователя
    void setUserName(QString const & value);

    // Задать пароль
    void setPassword(QString const & value);

    // Задать настройки подключения
    void setConnectOptions(QString const & value);

    // Открыть подключение
    void openConnection();

    // Закрыть подключение
    void closeConnection();

private:
    ConnectionBackend *_backend;
    QString const _connectionName;
    QString _hostName;
    int _port;
    QString _dbName;
    QString _userName;
    QString _password;
    QString _fullConnectionName;
    QString _connectOptions;

    // Задание полного имени подключения
    void setFullConnectionName();

    // // Задание новых параметров подключения
    // void update();

    bool _valid = false;     // Если удалось создать подключения, то объект валиден
    bool _connected = false; // Если удалось подключиться
    bool _busy = false; // Если рабочий поток занят
    QString _lastError = "Ошибок нет."; // Последняя ошибка

    QThread workerThread;  // Поток, где живёт рабочий класс

public slots:
    void slotManagerUpdate(bool const &connected, bool const &valid, bool const &busy, QString const &lastError);

signals:
    void signalInitialize();
    void signalOpenConnection();
    void signalCloseConnection();
    void signalManagerUpdate();
    void signalConfigUpdate(const DatabaseConfiguration & new_config);

};
