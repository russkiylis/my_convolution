#pragma once

#include <QObject>
#include <QString>
#include <QThread>
#include "databaseworker.h"

class Backend;

// Класс для работы с БД
// У него есть рабочий в другом потоке
class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    // Конструктор
    explicit DatabaseManager(Backend *backend,
                                QString const & connectionName = "myConvolution",
                                QString const & hostName = "127.0.0.1",
                                QString const & dbName = "my_convolution",
                                QString const & userName = "russkiylis",
                                QString const & password = "1337",
                                int const & port = 5432,
                                QString const & connectOptions = "connect_timeout = 3",
                                QObject * parent = nullptr);

    // Деструктор
    ~DatabaseManager();

    // Имя подключения (не путать с именем БД)
    QString const connectionName() {return _connectionName;}

    // Адрес хоста
    QString const hostName() {return _hostName;}

    // Порт
    int const port() {return _port;}

    // Имя БД
    QString const dbName() {return _dbName;}

    // Имя пользователя
    QString const userName() {return _userName;}

    // Пароль
    QString const password() {return _password;}

    // Полное имя подключения
    QString const fullConnectionName() {return _fullConnectionName;}

    // Настройки подключения
    QString const connectOptions() {return _connectOptions;}

    // Валиден ли объект
    bool valid() {return _valid;}

    // Занято ли подключение к БД
    bool busy() {return _busy;}

    // Установлено ли подключение к БД
    bool connected() {return _connected;}

    // Последняя ошибка БД
    QString lastError() {return _lastError;}

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
    Backend *_backend;
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
    void slotManagerUpdate(bool connected, bool valid, bool busy, QString lastError);

signals:
    void signalInitialize();
    void signalOpenConnection();
    void signalCloseConnection();
    void signalManagerUpdate();
    void signalConfigUpdate(const DatabaseConfiguration & new_config);

};
