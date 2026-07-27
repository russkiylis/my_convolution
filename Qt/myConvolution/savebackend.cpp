#include "savebackend.h"

SaveBackend::SaveBackend(DatabaseManager &db, QObject *parent) :
    QObject{parent},
    m_db(db),
    m_currentDataType(static_cast<DataType>(0))
{
}

void SaveBackend::processDataPackage(const LoadGenerator::DataPackage &package) {
    //TODO: Обработка дата пекедж
    // qDebug() << package.postName;

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
