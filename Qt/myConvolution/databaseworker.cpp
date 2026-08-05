#include <QDebug>
#include <QtSql/QSqlQuery>
#include <QSqlError>
#include <utility>
#include "databaseworker.h"
#include "savebackend.h"
#include "visualizebackend.h"

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

    QSqlDatabase::addDatabase("QPSQL", _config.connectionName); // Создали подключение к БД
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

void DatabaseWorker::slotInsert(const LoadGenerator::DataPackage &package, const int type, const int byteOrder) {
    _busy = true;
    emit signalManagerUpdate(_connected, _valid, _busy, _lastError);

    // Получаем байтовый кодер
    const std::unique_ptr<ByteArrayCoder> coder =
        ByteArrayCoder::create(
            static_cast<ByteArrayCoder::DataType>(type), static_cast<ByteArrayCoder::ByteOrder>(byteOrder));

    // Превращаем свёртку в массив байтов
    QByteArray convBytes = coder->serialize(package.conv);

    // Получаем строку с названием формата
    QString dataTypeStr;
    switch (static_cast<ByteArrayCoder::DataType>(type)) {
    case ByteArrayCoder::doublePrecision:
        switch (static_cast<ByteArrayCoder::ByteOrder>(byteOrder)) {
        case ByteArrayCoder::LittleEndian:
            dataTypeStr = "double_le";
            break;
        case ByteArrayCoder::BigEndian:
            dataTypeStr = "double_be";
            break;
        }
        break;
    case ByteArrayCoder::real:
        switch (static_cast<ByteArrayCoder::ByteOrder>(byteOrder)) {
        case ByteArrayCoder::LittleEndian:
            dataTypeStr = "real_le";
            break;
        case ByteArrayCoder::BigEndian:
            dataTypeStr = "real_be";
            break;
        }
        break;
    case ByteArrayCoder::smallint:
        switch (static_cast<ByteArrayCoder::ByteOrder>(byteOrder)) {
        case ByteArrayCoder::LittleEndian:
            dataTypeStr = "smallint_le";
            break;
        case ByteArrayCoder::BigEndian:
            dataTypeStr = "smallint_be";
            break;
        }
        break;
    }

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
    if (!Utils::fileToString(":sql/insert.sql", command, &fileError)) {
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

    query.bindValue(":data_type", dataTypeStr);
    query.bindValue(":min_angle_h", package.minAngleH);
    query.bindValue(":max_angle_h", package.maxAngleH);
    query.bindValue(":step_h", package.stepH);
    query.bindValue(":count_h", package.countH);
    query.bindValue(":min_angle_v", package.minAngleV);
    query.bindValue(":max_angle_v", package.maxAngleV);
    query.bindValue(":step_v", package.stepV);
    query.bindValue(":count_v", package.countV);
    query.bindValue(":quality", package.quality);
    query.bindValue(":conv", convBytes);

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

    // TODO: ПРОДОЛЖАТЬ ТУТ
    // Вычленяем id для того, чтобы внести его в df_result
    query.next();
    const int id = query.value(0).toInt();

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
    query.bindValue(":quality_h", package.qualityH);
    query.bindValue(":min_angle_v", package.minAngleV);
    query.bindValue(":max_angle_v", package.maxAngleV);
    query.bindValue(":step_v", package.stepV);
    query.bindValue(":conv_v_double", Utils::vectorToPgArray(package.convV));
    query.bindValue(":quality_v", package.qualityV);

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
    const int id = query.value(0).toInt();

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
    query.bindValue(":quality_h", package.qualityH);
    query.bindValue(":min_angle_v", package.minAngleV);
    query.bindValue(":max_angle_v", package.maxAngleV);
    query.bindValue(":step_v", package.stepV);
    query.bindValue(":conv_v_real", Utils::vectorToPgArray(package.convV));
    query.bindValue(":quality_v", package.qualityV);

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
    const int id = query.value(0).toInt();

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
    query.bindValue(":quality_h", package.qualityH);
    query.bindValue(":min_angle_v", package.minAngleV);
    query.bindValue(":max_angle_v", package.maxAngleV);
    query.bindValue(":step_v", package.stepV);
    query.bindValue(":conv_v_smallint", Utils::vectorToPgArray(package.convV));
    query.bindValue(":quality_v", package.qualityV);

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
    const int id = query.value(0).toInt();

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

void DatabaseWorker::slotReadDb() {
    qDebug() << "Чтение базы данных...";

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

    // Читаем таблицу
    if (!Utils::fileToString(":sql/selectRowForVisualization.sql", command, &fileError)) {
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

    QVector<RowForVisualization> rowsForVisualization;
    QVector<QVector<double>> convsHForVisualization;
    QVector<QVector<double>> convsVForVisualization;
    QVector<double> convHForVisualization;
    QVector<double> convVForVisualization;
    while (query.next()) {
        RowForVisualization row;
        row.id = query.value("id").toLongLong();
        row.timestamp = query.value("result_timestamp").toDateTime();
        row.sysname = query.value("sysname").toString();
        row.azimuth = query.value("azimuth").toDouble();
        row.elevation = query.value("elevation").toDouble();
        row.power = query.value("power").toDouble();
        row.frequency = query.value("frequency").toDouble();
        row.latitude = query.value("latitude").toDouble();
        row.longitude = query.value("longitude").toDouble();
        row.dataType = query.value("data_type").toString();
        row.qualityH = query.value("quality_h").toDouble();
        row.qualityV = query.value("quality_v").toDouble();
        rowsForVisualization.push_back(row);

        // TODO: Убрать это и добавить 
        QSqlQuery convQuery(db);
        // Забираем свёртки по id
        if (row.dataType == "double_precision") {
            if (!Utils::fileToString(":sql/selectDoublePrecision.sql", command, &fileError)) {
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
        } else if (row.dataType == "real") {
            if (!Utils::fileToString(":sql/selectReal.sql", command, &fileError)) {
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
        } else if (row.dataType == "smallint") {
            if (!Utils::fileToString(":sql/selectSmallint.sql", command, &fileError)) {
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
        } else {
            throw std::logic_error("При прочтении базы данных вернулся непонятный data_type!");
        }
        if (!convQuery.prepare(command)) {
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
        convQuery.bindValue(":id", row.id);
        convQuery.exec();
        convQuery.next();

        QString parseError;
        if (!Utils::jsonToVector(convQuery.value("conv_h"), convHForVisualization, &parseError)) {
            _lastError = "[!] "
            + _config.fullConnectionName
            + ": Операция не удалась: "
            + parseError;
            qDebug().noquote().nospace() << _lastError;
            db.rollback();
            _busy = false;
            emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
            return;
        }
        if (!Utils::jsonToVector(convQuery.value("conv_v"), convVForVisualization, &parseError)) {
            _lastError = "[!] "
            + _config.fullConnectionName
            + ": Операция не удалась: "
            + parseError;
            qDebug().noquote().nospace() << _lastError;
            db.rollback();
            _busy = false;
            emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
            return;
        }

        if (row.dataType == "smallint") {
            const double maxH = *std::max_element(convHForVisualization.constBegin(), convHForVisualization.constEnd());
            for (auto & value : convHForVisualization) {
                value /= maxH;
            }
            const double maxV = *std::max_element(convVForVisualization.constBegin(), convVForVisualization.constEnd());
            for (auto & value : convVForVisualization) {
                value /= maxV;
            }
        }

        convsHForVisualization.push_back(convHForVisualization);
        convsVForVisualization.push_back(convVForVisualization);
    }
    qDebug().noquote().nospace() << "Прочитано " << rowsForVisualization.size() << " строк.";
    db.commit();

    // Отправляем прочитанные данные из рабочего потока
    emit signalReadDb(rowsForVisualization, convsHForVisualization, convsVForVisualization);

    _busy = false;
    qDebug().noquote().nospace() << "Успешное чтение базы данных.";
    _lastError = "Ошибок нет.";
    emit signalManagerUpdate(_connected, _valid, _busy, _lastError);
}
