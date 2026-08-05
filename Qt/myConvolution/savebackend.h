#pragma once

#include <memory>
#include "bytearraycoder.h"
#include "loadgenerator.h"
#include "utils.h"
#include "databasemanager.h"

class SaveBackend final : public QObject {
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
    ByteArrayCoder::DataType m_currentDataType; // Текущий выбранный тип данных
    bool m_saveEnabled = false; // Происходит ли сохранение в БД

public:
    [[nodiscard]] bool saveEnabled() const;
    void setSaveEnabled(bool saveEnabled);

private:
signals:
    void currentDataTypeChanged(int type);
    void saveEnabledChanged(bool saveEnabled);
};
