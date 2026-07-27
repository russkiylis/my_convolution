#include "savebackend.h"

SaveBackend::SaveBackend(DatabaseManager &db, QObject *parent) :
    QObject{parent},
    m_db(db)
{
}

void SaveBackend::processDataPackage(const LoadGenerator::DataPackage &package) {
    //TODO: Обработка дата пекедж
}
