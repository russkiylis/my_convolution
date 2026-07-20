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
        return m_connectionName;
    }

    // Адрес хоста
    [[nodiscard]] QString hostName() const
    {
        return m_hostName;
    }

    // Порт
    [[nodiscard]] int port() const
    {
        return m_port;
    }

    // Имя БД
    [[nodiscard]] QString dbName() const
    {
        return m_dbName;
    }

    // Имя пользователя
    [[nodiscard]] QString userName() const
    {
        return m_userName;
    }

    // Пароль
    [[nodiscard]] QString password() const
    {
        return m_password;
    }

    // Полное имя подключения
    [[nodiscard]] QString fullConnectionName() const
    {
        return m_fullConnectionName;
    }

    // Настройки подключения
    [[nodiscard]] QString connectOptions() const
    {
        return m_connectOptions;
    }

    // Валиден ли объект
    [[nodiscard]] bool valid() const
    {
        return m_valid;
    }

    // Занято ли подключение к БД
    [[nodiscard]] bool busy() const
    {
        return m_busy;
    }

    // Установлено ли подключение к БД
    [[nodiscard]] bool connected() const
    {
        return m_connected;
    }

    // Последняя ошибка БД
    [[nodiscard]] QString lastError() const
    {
        return m_lastError;
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
    ConnectionBackend *m_backend;
    QString const m_connectionName;
    QString m_hostName;
    int m_port;
    QString m_dbName;
    QString m_userName;
    QString m_password;
    QString m_fullConnectionName;
    QString m_connectOptions;

    // Задание полного имени подключения
    void setFullConnectionName();

    // // Задание новых параметров подключения
    // void update();

    bool m_valid = false;     // Если удалось создать подключения, то объект валиден
    bool m_connected = false; // Если удалось подключиться
    bool m_busy = false; // Если рабочий поток занят
    QString m_lastError = "Ошибок нет."; // Последняя ошибка

    QThread m_workerThread;  // Поток, где живёт рабочий класс

public slots:
    void slotManagerUpdate(bool const &connected, bool const &valid, bool const &busy, QString const &lastError);

signals:
    void signalInitialize();
    void signalOpenConnection();
    void signalCloseConnection();
    void signalManagerUpdate();
    void signalConfigUpdate(const DatabaseConfiguration & new_config);

};
