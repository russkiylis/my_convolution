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
    if (m_saveEnabled)
        m_db.saveDataPackage(package, m_currentDataType, m_currentByteOrder);
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

int SaveBackend::currentByteOrder() const
{
    return m_currentByteOrder;
}

void SaveBackend::setCurrentByteOrder(int byteOrder)
{
    m_currentByteOrder = static_cast<ByteArrayCoder::ByteOrder>(byteOrder);
    qDebug().noquote().nospace() << "Порядок байтов сменился: " << m_currentByteOrder;
    emit currentByteOrderChanged(m_currentByteOrder);
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
