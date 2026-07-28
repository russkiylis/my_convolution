#include "savebackend.h"
#include "databasemanager.h"

SaveBackend::SaveBackend(DatabaseManager &db, QObject *parent) :
    QObject{parent},
    m_db(db),
    m_currentDataType(static_cast<DataType>(0))
{
}

void SaveBackend::processDataPackage(const LoadGenerator::DataPackage &package) {
    switch (m_currentDataType) {
    case doublePrecision:
        m_db.saveDataPackage(package);
        break;
    case real:
        m_db.saveDataPackage(DataPackageFloat(package));
        break;
    case smallint:
        m_db.saveDataPackage(DataPackageInt16(package));
        break;
    default:
        throw std::logic_error("Сомнительный тип данных!");
    }
}

int SaveBackend::currentDataType() const
{
    return m_currentDataType;
}

void SaveBackend::setCurrentDataType(int type) {
    m_currentDataType = static_cast<DataType>(type);
    qDebug().noquote().nospace() << "Тип записи сменился: " << m_currentDataType;
    emit currentDataTypeChanged(m_currentDataType);
}

void SaveBackend::onSaveEnableButtonClicked() {
    setSaveEnabled(!m_saveEnabled);
    QString msg = m_saveEnabled ? "Запись в базу данных включена." : "Запись в базу данных отключена.";
    qDebug() << msg;
}

bool SaveBackend::saveEnabled() const
{
    return m_saveEnabled;
}

void SaveBackend::setSaveEnabled(bool saveEnabled)
{
    m_saveEnabled = saveEnabled;
    emit saveEnabledChanged(saveEnabled);
}
