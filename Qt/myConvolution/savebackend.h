#pragma once

#include "loadgenerator.h"
#include "utils.h"
#include "databasemanager.h"

// Пакет данных, свёртка как Int16 (smallint в psql)
struct DataPackageInt16
{
    DataPackageInt16() = default;
    // Преобразуем обычный DataPackage в DataPackageInt16
    explicit DataPackageInt16(const LoadGenerator::DataPackage &other) :
        timestamp(other.timestamp),
        bearingH(other.bearingH),
        bearingV(other.bearingV),
        qualityH(other.qualityH),
        qualityV(other.qualityV),
        level(other.level),
        frequency(other.frequency),
        coordinate(other.coordinate),
        postName(other.postName),
        minAngleH(other.minAngleH),
        maxAngleH(other.maxAngleH),
        minAngleV(other.minAngleV),
        maxAngleV(other.maxAngleV),
        stepH(other.stepH),
        stepV(other.stepV)
    {
        for (const double value : other.convH) {
            convH.push_back(Utils::doubleToQint16(value)); // Изначально была нормировка к единице, чтобы хранить в интах, помножим на 30000
        }
        for (const double value : other.convV) {
            convV.push_back(Utils::doubleToQint16(value));
        }
    }

    // Преобразуем обычный DataPackage к DataPackageInt16
    DataPackageInt16 & operator=(const LoadGenerator::DataPackage &other)
    {
        timestamp = other.timestamp;
        bearingH = other.bearingH;
        bearingV = other.bearingV;
        qualityH = other.qualityH;
        qualityV = other.qualityV;
        level = other.level;
        frequency = other.frequency;
        coordinate = other.coordinate;
        postName = other.postName;
        minAngleH = other.minAngleH;
        maxAngleH = other.maxAngleH;
        minAngleV = other.minAngleV;
        maxAngleV = other.maxAngleV;
        stepH = other.stepH;
        stepV = other.stepV;

        convH.clear();
        convV.clear();
        for (const double value : other.convH) {
            convH.push_back(Utils::doubleToQint16(value));
        }
        for (const double value : other.convV) {
            convV.push_back(Utils::doubleToQint16(value));
        }

        return *this;
    }

    QDateTime timestamp;        // Временная метка
    double bearingH{};            // Направление по горизонтали
    double bearingV{};            // Направление по вертикали
    double qualityH{};            // Качество свёртки по горизонтали
    double qualityV{};            // Качество свёртки по вертикали
    double level{};               // Мощность
    double frequency{};           // Частота
    QGeoCoordinate coordinate;  // Координата
    QString postName;           // Имя поста

    int minAngleH{};               // Минимальный угол (горизонтальный)
    int maxAngleH{};               // Максимальный угол (горизонтальный)
    int minAngleV{};               // Минимамльный угол (вертикальный)
    int maxAngleV{};               // Максимальный угол (вертикальный)
    double stepH{};                // Шаг (горизонтальный)
    double stepV{};                // Шаг (вертикальный)

    std::vector<qint16> convH;   // Свёртка по горизонтали
    std::vector<qint16> convV;   // Свёртка по вертикали
};
Q_DECLARE_METATYPE(DataPackageInt16)

// Пакет данных, свёртка как Float (real в psql)
struct DataPackageFloat
{
    DataPackageFloat() = default;
    // Преобразуем обычный DataPackage в DataPackageFloat
    explicit DataPackageFloat(const LoadGenerator::DataPackage &other) :
        timestamp(other.timestamp),
        bearingH(other.bearingH),
        bearingV(other.bearingV),
        qualityH(other.qualityH),
        qualityV(other.qualityV),
        level(other.level),
        frequency(other.frequency),
        coordinate(other.coordinate),
        postName(other.postName),
        minAngleH(other.minAngleH),
        maxAngleH(other.maxAngleH),
        minAngleV(other.minAngleV),
        maxAngleV(other.maxAngleV),
        stepH(other.stepH),
        stepV(other.stepV)
    {
        for (const double value : other.convH) {
            convH.push_back(static_cast<float>(value));
        }
        for (const double value : other.convV) {
            convV.push_back(static_cast<float>(value));
        }
    }

    // Преобразуем обычный DataPackageInt к DataPackage
    DataPackageFloat & operator=(const LoadGenerator::DataPackage &other)
    {
        timestamp = other.timestamp;
        bearingH = other.bearingH;
        bearingV = other.bearingV;
        qualityH = other.qualityH;
        qualityV = other.qualityV;
        level = other.level;
        frequency = other.frequency;
        coordinate = other.coordinate;
        postName = other.postName;
        minAngleH = other.minAngleH;
        maxAngleH = other.maxAngleH;
        minAngleV = other.minAngleV;
        maxAngleV = other.maxAngleV;
        stepH = other.stepH;
        stepV = other.stepV;

        convH.clear();
        convV.clear();
        for (const double value : other.convH) {
            convH.push_back(static_cast<float>(value));
        }
        for (const double value : other.convV) {
            convV.push_back(static_cast<float>(value));
        }

        return *this;
    }

    QDateTime timestamp;        // Временная метка
    double bearingH{};            // Направление по горизонтали
    double bearingV{};            // Направление по вертикали
    double qualityH{};            // Качество свёртки по горизонтали
    double qualityV{};            // Качество свёртки по вертикали
    double level{};               // Мощность
    double frequency{};           // Частота
    QGeoCoordinate coordinate;  // Координата
    QString postName;           // Имя поста

    int minAngleH{};               // Минимальный угол (горизонтальный)
    int maxAngleH{};               // Максимальный угол (горизонтальный)
    int minAngleV{};               // Минимамльный угол (вертикальный)
    int maxAngleV{};               // Максимальный угол (вертикальный)
    double stepH{};                // Шаг (горизонтальный)
    double stepV{};                // Шаг (вертикальный)

    std::vector<float> convH;   // Свёртка по горизонтали
    std::vector<float> convV;   // Свёртка по вертикали
};
Q_DECLARE_METATYPE(DataPackageFloat)

enum DataType
{
    doublePrecision,
    real,
    smallint
};

class SaveBackend : public QObject {
    Q_OBJECT

    Q_PROPERTY(int currentDataType READ currentDataType WRITE setCurrentDataType NOTIFY currentDataTypeChanged)
    Q_PROPERTY(bool saveEnabled READ saveEnabled WRITE setSaveEnabled NOTIFY saveEnabledChanged)

public:
    explicit SaveBackend(DatabaseManager &db, QObject *parent = nullptr);

    void processDataPackage(const LoadGenerator::DataPackage &package) const;

    [[nodiscard]] int currentDataType() const;

    void setCurrentDataType(int type);

    Q_INVOKABLE void onSaveEnableButtonClicked();
    Q_INVOKABLE void onClearTableButtonClicked() const;
    Q_INVOKABLE void onRecreateTableButtonClicked() const;
    Q_INVOKABLE void onDeleteTableButtonClicked() const;

private:
    DatabaseManager &m_db;  // Менеджер базы данных
    DataType m_currentDataType; // Текущий выбранный тип данных
    bool m_saveEnabled = false; // Происходит ли сохранение в БД

public:
    [[nodiscard]] bool saveEnabled() const;
    void setSaveEnabled(bool saveEnabled);

private:
signals:
    void currentDataTypeChanged(int type);
    void saveEnabledChanged(bool saveEnabled);
};
