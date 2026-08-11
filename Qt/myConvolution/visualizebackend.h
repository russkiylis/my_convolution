#pragma once

#include <memory>
#include "databasemanager.h"
#include "visualizationtablemodel.h"

class VisualizeBackend final : public QObject {
    Q_OBJECT
    Q_PROPERTY(VisualizationTableModel *visualizationTableModel READ visualizationTableModel CONSTANT)
public:
    explicit VisualizeBackend(DatabaseManager &writerDb, QObject *parent = nullptr);

    Q_INVOKABLE void onDbConnectionButtonClicked();
    Q_INVOKABLE void readDb();

    VisualizationTableModel *visualizationTableModel();
private:
    // Обновить данные подключения-читалки в соответствии с данными подключения-писателя
    void updateFromWriter();

    DatabaseManager &m_writerDb;
    DatabaseManager m_readerDb;
    VisualizationTableModel m_visualizationModel;

    int m_readerDbStatus = false;
    QString m_readerLastError = "Ошибок нет.";

    QVector<RowForVisualization> m_rowsForVisualization;

public slots:
    void slotSetDbStatus(int dbStatus);
    void slotSetLastError(const QString &lastError);
    void slotSetData(const QVector<RowForVisualization> &rows, const QVector<QVector<double>> &convs);
};
