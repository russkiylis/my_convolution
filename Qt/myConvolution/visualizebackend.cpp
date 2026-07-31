#include "visualizebackend.h"

VisualizeBackend::VisualizeBackend(DatabaseManager &writerDb, QObject *parent) :
QObject{parent},
m_writerDb(writerDb),
m_readerDb(writerDb.connectionName() + "_reader")
{
    connect(&m_readerDb, &DatabaseManager::signalSetDbStatus, this, &VisualizeBackend::slotSetDbStatus);
    connect(&m_readerDb, &DatabaseManager::signalSetLastError, this, &VisualizeBackend::slotSetLastError);
    connect(&m_readerDb, &DatabaseManager::signalSetData, this, &VisualizeBackend::slotSetData);
}

void VisualizeBackend::onDbConnectionButtonClicked() {
    if (m_readerDbStatus != 2) {
        // Если не БД не подключена, то подключаем
        updateFromWriter();
        m_readerDb.openConnection();
    } else {
        // Если БД подключена, то отключаем
        m_readerDb.closeConnection();
    }
}

void VisualizeBackend::updateFromWriter() {
    qDebug() << "Обновление подключения-читалки...";

    m_readerDb.setConnectionName(m_writerDb.connectionName() + "_reader");
    m_readerDb.setHostName(m_writerDb.hostName());
    m_readerDb.setPort(m_writerDb.port());
    m_readerDb.setUserName(m_writerDb.userName());
    m_readerDb.setPassword(m_writerDb.password());
    m_readerDb.setDbName(m_writerDb.dbName());
    m_readerDb.setConnectOptions(m_writerDb.connectOptions());
}

void VisualizeBackend::readDb() {
    m_readerDb.readDb();
}

VisualizationTableModel * VisualizeBackend::visualizationTableModel() {
    return &m_visualizationModel;
}

void VisualizeBackend::slotSetDbStatus(const int dbStatus) {
    m_readerDbStatus = dbStatus;
}

void VisualizeBackend::slotSetLastError(const QString &lastError) {
    m_readerLastError = lastError;
}

void VisualizeBackend::slotSetData(
    const QVector<RowForVisualization>& rows,
    const QVector<QVector<double>>& convsH,
    const QVector<QVector<double>>& convsV
    ) {
    qDebug() << "Прочтённые данные дошли до бекенда!";
    m_visualizationModel.replaceData(rows, convsH, convsV);
}
