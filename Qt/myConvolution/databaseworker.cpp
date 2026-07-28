#include <QDebug>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
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
// FIXME: Тут получился дикий спагетти код как будто бы
void DatabaseWorker::slotInsertDouble(const LoadGenerator::DataPackage &package) {
    _busy = true;
    emit signalManagerUpdate(_connected, _valid, _busy, _lastError);

    // Получаем объект для работы с БД
    QSqlDatabase db = QSqlDatabase::database(_config.connectionName);

    db.transaction();   // Начитаем транзакцию

    // Получаем объект для работы с запросами
    QSqlQuery query(db);
    QString fileError;
    QString command;

    if (!_valid) {
        _lastError = "[!] "
                     + _config.fullConnectionName
                     + ": объект подключения не валиден. "
                     + "Невозможно записать данные.";
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }
    if (!_connected) {
        _lastError = "[!] "
                     + _config.fullConnectionName
                     + ": невозможно записать данные "
                     + "в закрытое соединение.";
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    // Считываем файл для записи свёртки
    if (!Utils::fileToString(":sql/insertDoublePrecision.sql", command, &fileError)) {
        _lastError = "[!] "
            + _config.fullConnectionName
            + ": Запись не удалась: "
            + fileError;
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    // Подготавливаем и выполняем команду записи свёртки
    if (!query.prepare(command)) {
        _lastError = "[!] "
        + _config.fullConnectionName
        + ": Запись не удалась: "
        + query.lastError().text();
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    query.bindValue(":min_angle_h", package.minAngleH);
    query.bindValue(":max_angle_h", package.maxAngleH);
    query.bindValue(":step_h", package.stepH);
    query.bindValue(":conv_h_double", Utils::vectorToPgArray(package.convH));
    query.bindValue(":min_angle_v", package.minAngleV);
    query.bindValue(":max_angle_v", package.maxAngleV);
    query.bindValue(":step_v", package.stepV);
    query.bindValue(":conv_v_double", Utils::vectorToPgArray(package.convV));

    if (!query.exec()) {
        _lastError = "[!] "
        + _config.fullConnectionName
        + ": Запись не удалась: "
        + query.lastError().text();
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    // Считываем файл для получения свёртки
    if (!Utils::fileToString(":sql/selectDoublePrecision.sql", command, &fileError)) {
        _lastError = "[!] "
            + _config.fullConnectionName
            + ": Запись не удалась: "
            + fileError;
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }
    command += "ORDER BY id DESC LIMIT 1;";

    if (!query.prepare(command)) {
        _lastError = "[!] "
        + _config.fullConnectionName
        + ": Запись не удалась: "
        + query.lastError().text();
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    query.bindValue(":value", Utils::vectorToPgArray(package.convH));

    if (!query.exec()) {
        _lastError = "[!] "
        + _config.fullConnectionName
        + ": Запись не удалась: "
        + query.lastError().text();
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    // Вычленяем id для того, чтобы внести его в df_result
    query.next();
    int id = query.value(0).toInt();

    // Считываем файл для записи df_result
    if (!Utils::fileToString(":sql/insertResult.sql", command, &fileError)) {
        _lastError = "[!] "
            + _config.fullConnectionName
            + ": Запись не удалась: "
            + fileError;
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    if (!query.prepare(command)) {
        _lastError = "[!] "
        + _config.fullConnectionName
        + ": Запись не удалась: "
        + query.lastError().text();
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    query.bindValue(":id", id);
    query.bindValue(":result_timestamp", package.timestamp);
    query.bindValue(":azimuth", package.bearingH);
    query.bindValue(":elevation", package.bearingV);
    query.bindValue(":power", package.level);
    query.bindValue(":frequency", package.frequency);
    query.bindValue(":longitude", package.coordinate.longitude());
    query.bindValue(":latitude", package.coordinate.latitude());
    query.bindValue(":sysname", package.postName);

    if (!query.exec()) {
        _lastError = "[!] "
        + _config.fullConnectionName
        + ": Запись не удалась: "
        + query.lastError().text();
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    db.commit();
    _busy = false;
    qDebug().noquote().nospace() << "Успешная запись в БД.";
    _lastError = "Ошибок нет.";
    emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
}

// FIXME: Тут получился дикий спагетти код как будто бы
void DatabaseWorker::slotInsertFloat(const DataPackageFloat &package) {
    _busy = true;
    emit signalManagerUpdate(_connected, _valid, _busy, _lastError);

    // Получаем объект для работы с БД
    QSqlDatabase db = QSqlDatabase::database(_config.connectionName);

    db.transaction();   // Начитаем транзакцию

    // Получаем объект для работы с запросами
    QSqlQuery query(db);
    QString fileError;
    QString command;

    if (!_valid) {
        _lastError = "[!] "
                     + _config.fullConnectionName
                     + ": объект подключения не валиден. "
                     + "Невозможно записать данные.";
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }
    if (!_connected) {
        _lastError = "[!] "
                     + _config.fullConnectionName
                     + ": невозможно записать данные "
                     + "в закрытое соединение.";
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    // Считываем файл для записи свёртки
    if (!Utils::fileToString(":sql/insertReal.sql", command, &fileError)) {
        _lastError = "[!] "
            + _config.fullConnectionName
            + ": Запись не удалась: "
            + fileError;
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    // Подготавливаем и выполняем команду записи свёртки
    if (!query.prepare(command)) {
        _lastError = "[!] "
        + _config.fullConnectionName
        + ": Запись не удалась: "
        + query.lastError().text();
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    query.bindValue(":min_angle_h", package.minAngleH);
    query.bindValue(":max_angle_h", package.maxAngleH);
    query.bindValue(":step_h", package.stepH);
    query.bindValue(":conv_h_real", Utils::vectorToPgArray(package.convH));
    query.bindValue(":min_angle_v", package.minAngleV);
    query.bindValue(":max_angle_v", package.maxAngleV);
    query.bindValue(":step_v", package.stepV);
    query.bindValue(":conv_v_real", Utils::vectorToPgArray(package.convV));

    if (!query.exec()) {
        _lastError = "[!] "
        + _config.fullConnectionName
        + ": Запись не удалась: "
        + query.lastError().text();
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    // Считываем файл для получения свёртки
    if (!Utils::fileToString(":sql/selectReal.sql", command, &fileError)) {
        _lastError = "[!] "
            + _config.fullConnectionName
            + ": Запись не удалась: "
            + fileError;
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }
    command += "ORDER BY id DESC LIMIT 1;";

    if (!query.prepare(command)) {
        _lastError = "[!] "
        + _config.fullConnectionName
        + ": Запись не удалась: "
        + query.lastError().text();
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    query.bindValue(":value", Utils::vectorToPgArray(package.convH));

    if (!query.exec()) {
        _lastError = "[!] "
        + _config.fullConnectionName
        + ": Запись не удалась: "
        + query.lastError().text();
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    // Вычленяем id для того, чтобы внести его в df_result
    query.next();
    int id = query.value(0).toInt();

    // Считываем файл для записи df_result
    if (!Utils::fileToString(":sql/insertResult.sql", command, &fileError)) {
        _lastError = "[!] "
            + _config.fullConnectionName
            + ": Запись не удалась: "
            + fileError;
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    if (!query.prepare(command)) {
        _lastError = "[!] "
        + _config.fullConnectionName
        + ": Запись не удалась: "
        + query.lastError().text();
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    query.bindValue(":id", id);
    query.bindValue(":result_timestamp", package.timestamp);
    query.bindValue(":azimuth", package.bearingH);
    query.bindValue(":elevation", package.bearingV);
    query.bindValue(":power", package.level);
    query.bindValue(":frequency", package.frequency);
    query.bindValue(":longitude", package.coordinate.longitude());
    query.bindValue(":latitude", package.coordinate.latitude());
    query.bindValue(":sysname", package.postName);

    if (!query.exec()) {
        _lastError = "[!] "
        + _config.fullConnectionName
        + ": Запись не удалась: "
        + query.lastError().text();
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    db.commit();
    _busy = false;
    qDebug().noquote().nospace() << "Успешная запись в БД.";
    _lastError = "Ошибок нет.";
    emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
}

// FIXME: Тут получился дикий спагетти код как будто бы
void DatabaseWorker::slotInsertInt16(const DataPackageInt16 &package) {
    _busy = true;
    emit signalManagerUpdate(_connected, _valid, _busy, _lastError);

    // Получаем объект для работы с БД
    QSqlDatabase db = QSqlDatabase::database(_config.connectionName);

    db.transaction();   // Начитаем транзакцию

    // Получаем объект для работы с запросами
    QSqlQuery query(db);
    QString fileError;
    QString command;

    if (!_valid) {
        _lastError = "[!] "
                     + _config.fullConnectionName
                     + ": объект подключения не валиден. "
                     + "Невозможно записать данные.";
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }
    if (!_connected) {
        _lastError = "[!] "
                     + _config.fullConnectionName
                     + ": невозможно записать данные "
                     + "в закрытое соединение.";
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    // Считываем файл для записи свёртки
    if (!Utils::fileToString(":sql/insertSmallint.sql", command, &fileError)) {
        _lastError = "[!] "
            + _config.fullConnectionName
            + ": Запись не удалась: "
            + fileError;
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    // Подготавливаем и выполняем команду записи свёртки
    if (!query.prepare(command)) {
        _lastError = "[!] "
        + _config.fullConnectionName
        + ": Запись не удалась: "
        + query.lastError().text();
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    query.bindValue(":min_angle_h", package.minAngleH);
    query.bindValue(":max_angle_h", package.maxAngleH);
    query.bindValue(":step_h", package.stepH);
    query.bindValue(":conv_h_smallint", Utils::vectorToPgArray(package.convH));
    query.bindValue(":min_angle_v", package.minAngleV);
    query.bindValue(":max_angle_v", package.maxAngleV);
    query.bindValue(":step_v", package.stepV);
    query.bindValue(":conv_v_smallint", Utils::vectorToPgArray(package.convV));

    if (!query.exec()) {
        _lastError = "[!] "
        + _config.fullConnectionName
        + ": Запись не удалась: "
        + query.lastError().text();
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    // Считываем файл для получения свёртки
    if (!Utils::fileToString(":sql/selectSmallint.sql", command, &fileError)) {
        _lastError = "[!] "
            + _config.fullConnectionName
            + ": Запись не удалась: "
            + fileError;
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }
    command += "ORDER BY id DESC LIMIT 1;";

    if (!query.prepare(command)) {
        _lastError = "[!] "
        + _config.fullConnectionName
        + ": Запись не удалась: "
        + query.lastError().text();
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    query.bindValue(":value", Utils::vectorToPgArray(package.convH));

    if (!query.exec()) {
        _lastError = "[!] "
        + _config.fullConnectionName
        + ": Запись не удалась: "
        + query.lastError().text();
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    // Вычленяем id для того, чтобы внести его в df_result
    query.next();
    int id = query.value(0).toInt();

    // Считываем файл для записи df_result
    if (!Utils::fileToString(":sql/insertResult.sql", command, &fileError)) {
        _lastError = "[!] "
            + _config.fullConnectionName
            + ": Запись не удалась: "
            + fileError;
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    if (!query.prepare(command)) {
        _lastError = "[!] "
        + _config.fullConnectionName
        + ": Запись не удалась: "
        + query.lastError().text();
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    query.bindValue(":id", id);
    query.bindValue(":result_timestamp", package.timestamp);
    query.bindValue(":azimuth", package.bearingH);
    query.bindValue(":elevation", package.bearingV);
    query.bindValue(":power", package.level);
    query.bindValue(":frequency", package.frequency);
    query.bindValue(":longitude", package.coordinate.longitude());
    query.bindValue(":latitude", package.coordinate.latitude());
    query.bindValue(":sysname", package.postName);

    if (!query.exec()) {
        _lastError = "[!] "
        + _config.fullConnectionName
        + ": Запись не удалась: "
        + query.lastError().text();
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    db.commit();
    _busy = false;
    qDebug().noquote().nospace() << "Успешная запись в БД.";
    _lastError = "Ошибок нет.";
    emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
}

void DatabaseWorker::slotClearTable() {
    qDebug() << "Очищение таблиц базы данных...";

    _busy = true;
    emit signalManagerUpdate(_connected, _valid, _busy, _lastError);

    // Получаем объект для работы с БД
    QSqlDatabase db = QSqlDatabase::database(_config.connectionName);

    db.transaction();   // Начитаем транзакцию

    // Получаем объект для работы с запросами
    QSqlQuery query(db);
    QString fileError;
    QString command;

    if (!_valid) {
        _lastError = "[!] "
                     + _config.fullConnectionName
                     + ": объект подключения не валиден. "
                     + "Невозможно выполнить операцию.";
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }
    if (!_connected) {
        _lastError = "[!] "
                     + _config.fullConnectionName
                     + ": невозможно выполнить операцию "
                     + "при закрытом соединении.";
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    // Очищаем таблицу со свёртками. Таблица с результатами очистится автоматически через CASCADE
    if (!Utils::fileToString(":sql/cleanConv.sql", command, &fileError)) {
        _lastError = "[!] "
            + _config.fullConnectionName
            + ": Операция не удалась: "
            + fileError;
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }
    if (!query.exec(command)) {
        _lastError = "[!] "
        + _config.fullConnectionName
        + ": Операция не удалась: "
        + query.lastError().text();
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    db.commit();
    _busy = false;
    qDebug().noquote().nospace() << "Успешное удаление данных из таблиц.";
    _lastError = "Ошибок нет.";
    emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
}

void DatabaseWorker::slotRecreateTable() {
    qDebug() << "Пересоздание таблиц базы данных...";

    _busy = true;
    emit signalManagerUpdate(_connected, _valid, _busy, _lastError);

    // Получаем объект для работы с БД
    QSqlDatabase db = QSqlDatabase::database(_config.connectionName);

    db.transaction();   // Начитаем транзакцию

    // Получаем объект для работы с запросами
    QSqlQuery query(db);
    QString fileError;
    QString command;

    if (!_valid) {
        _lastError = "[!] "
                     + _config.fullConnectionName
                     + ": объект подключения не валиден. "
                     + "Невозможно выполнить операцию.";
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }
    if (!_connected) {
        _lastError = "[!] "
                     + _config.fullConnectionName
                     + ": невозможно выполнить операцию "
                     + "при закрытом соединении.";
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    // Уничтожаем таблицу с результатами
    if (!Utils::fileToString(":sql/deleteResult.sql", command, &fileError)) {
        _lastError = "[!] "
            + _config.fullConnectionName
            + ": Операция не удалась: "
            + fileError;
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }
    if (!query.exec(command)) {
        _lastError = "[!] "
        + _config.fullConnectionName
        + ": Операция не удалась: "
        + query.lastError().text();
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    // Уничтожаем таблицу со свёртками
    if (!Utils::fileToString(":sql/deleteConv.sql", command, &fileError)) {
        _lastError = "[!] "
            + _config.fullConnectionName
            + ": Операция не удалась: "
            + fileError;
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }
    if (!query.exec(command)) {
        _lastError = "[!] "
        + _config.fullConnectionName
        + ": Операция не удалась: "
        + query.lastError().text();
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    // Уничтожаем тип данных "data_type"
    if (!Utils::fileToString(":sql/deleteDataTypeEnum.sql", command, &fileError)) {
        _lastError = "[!] "
            + _config.fullConnectionName
            + ": Операция не удалась: "
            + fileError;
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }
    if (!query.exec(command)) {
        _lastError = "[!] "
        + _config.fullConnectionName
        + ": Операция не удалась: "
        + query.lastError().text();
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    // Создаём тип данных "data_type" (на прошлом шаге мы его удалили чтобы его точно не было, а то вылезет ошибка)
    if (!Utils::fileToString(":sql/createDataTypeEnum.sql", command, &fileError)) {
        _lastError = "[!] "
            + _config.fullConnectionName
            + ": Операция не удалась: "
            + fileError;
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }
    if (!query.exec(command)) {
        _lastError = "[!] "
        + _config.fullConnectionName
        + ": Операция не удалась: "
        + query.lastError().text();
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    // Создаём таблицу со свёртками
    if (!Utils::fileToString(":sql/createConv.sql", command, &fileError)) {
        _lastError = "[!] "
            + _config.fullConnectionName
            + ": Операция не удалась: "
            + fileError;
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }
    if (!query.exec(command)) {
        _lastError = "[!] "
        + _config.fullConnectionName
        + ": Операция не удалась: "
        + query.lastError().text();
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    // Создаём таблицу с результатами
    if (!Utils::fileToString(":sql/createResult.sql", command, &fileError)) {
        _lastError = "[!] "
            + _config.fullConnectionName
            + ": Операция не удалась: "
            + fileError;
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }
    if (!query.exec(command)) {
        _lastError = "[!] "
        + _config.fullConnectionName
        + ": Операция не удалась: "
        + query.lastError().text();
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    db.commit();
    _busy = false;
    qDebug().noquote().nospace() << "Успешное пересоздание таблиц.";
    _lastError = "Ошибок нет.";
    emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
}

void DatabaseWorker::slotDeleteTable() {
    qDebug() << "Удаление таблиц базы данных...";

        _busy = true;
    emit signalManagerUpdate(_connected, _valid, _busy, _lastError);

    // Получаем объект для работы с БД
    QSqlDatabase db = QSqlDatabase::database(_config.connectionName);

    db.transaction();   // Начитаем транзакцию

    // Получаем объект для работы с запросами
    QSqlQuery query(db);
    QString fileError;
    QString command;

    if (!_valid) {
        _lastError = "[!] "
                     + _config.fullConnectionName
                     + ": объект подключения не валиден. "
                     + "Невозможно выполнить операцию.";
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }
    if (!_connected) {
        _lastError = "[!] "
                     + _config.fullConnectionName
                     + ": невозможно выполнить операцию "
                     + "при закрытом соединении.";
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    // Уничтожаем таблицу с результатами
    if (!Utils::fileToString(":sql/deleteResult.sql", command, &fileError)) {
        _lastError = "[!] "
            + _config.fullConnectionName
            + ": Операция не удалась: "
            + fileError;
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }
    if (!query.exec(command)) {
        _lastError = "[!] "
        + _config.fullConnectionName
        + ": Операция не удалась: "
        + query.lastError().text();
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    // Уничтожаем таблицу со свёртками
    if (!Utils::fileToString(":sql/deleteConv.sql", command, &fileError)) {
        _lastError = "[!] "
            + _config.fullConnectionName
            + ": Операция не удалась: "
            + fileError;
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }
    if (!query.exec(command)) {
        _lastError = "[!] "
        + _config.fullConnectionName
        + ": Операция не удалась: "
        + query.lastError().text();
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    // Уничтожаем тип данных "data_type"
    if (!Utils::fileToString(":sql/deleteDataTypeEnum.sql", command, &fileError)) {
        _lastError = "[!] "
            + _config.fullConnectionName
            + ": Операция не удалась: "
            + fileError;
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }
    if (!query.exec(command)) {
        _lastError = "[!] "
        + _config.fullConnectionName
        + ": Операция не удалась: "
        + query.lastError().text();
        qDebug().noquote().nospace() << _lastError;
        db.rollback();
        _busy = false;
        emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
        return;
    }

    db.commit();
    _busy = false;
    qDebug().noquote().nospace() << "Успешное удаление таблиц.";
    _lastError = "Сейчас точно полезут ошибки... Надо пересоздать таблицы.";
    emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
}
