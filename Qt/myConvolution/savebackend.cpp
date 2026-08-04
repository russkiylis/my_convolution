#include "savebackend.h"

#include <complex>

#include "databasemanager.h"

SaveBackend::SaveBackend(DatabaseManager &db, QObject *parent) :
    QObject{parent},
    m_db(db),
    m_currentDataType(static_cast<ByteArrayCoder::DataType>(0))
{
}

void SaveBackend::processDataPackage(const LoadGenerator::DataPackage &package) const
{
    // TODO: Это всё идёт под снос ибо мы меняем бд на bytearray



    if (m_saveEnabled) {
        switch (m_currentDataType) {
        case ByteArrayCoder::doublePrecision:
            m_db.saveDataPackage(package);
            break;
        case ByteArrayCoder::real:
            m_db.saveDataPackage(DataPackageFloat(package));
            break;
        case ByteArrayCoder::smallint:
            m_db.saveDataPackage(DataPackageInt16(package));

            break;
        default:
            throw std::logic_error("Сомнительный тип данных!");
        }
    }
}

int SaveBackend::currentDataType() const
{
    return m_currentDataType;
}

void SaveBackend::setCurrentDataType(int type) {
    m_currentDataType = static_cast<ByteArrayCoder::DataType>(type);
    qDebug().noquote().nospace() << "Тип записи сменился: " << m_currentDataType;
    emit currentDataTypeChanged(m_currentDataType);
}

void SaveBackend::onSaveEnableButtonClicked() {
    setSaveEnabled(!m_saveEnabled);
    const QString msg = m_saveEnabled ? "Запись в базу данных включена." : "Запись в базу данных отключена.";
    qDebug().noquote().nospace() << msg;
}

void SaveBackend::onClearTableButtonClicked() const
{
    m_db.signalClearTable();
}

void SaveBackend::onRecreateTableButtonClicked() const
{
    m_db.signalRecreateTable();
}

void SaveBackend::onDeleteTableButtonClicked() const
{
    m_db.signalDeleteTable();
}

bool SaveBackend::saveEnabled() const
{
    return m_saveEnabled;
}

void SaveBackend::setSaveEnabled(const bool saveEnabled)
{
    m_saveEnabled = saveEnabled;
    emit saveEnabledChanged(saveEnabled);
}
