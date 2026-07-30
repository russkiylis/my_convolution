#pragma once
#include <QObject>
#include "databasemanager.h"

// Отображаемая строчка (без спарклайна свёрток)
struct RowForVisualization
{
    qint64 id;
    QDateTime timestamp;

    QString sysname;
    double azimuth;
    double elevation;
    double power;
    double frequency;
    double latitude;
    double longitude;

    QString dataType;

    double qualityH;
    double qualityV;
};

class VisualizeBackend : public QObject {
    Q_OBJECT
public:
    explicit VisualizeBackend(DatabaseManager &writerDb, QObject *parent = nullptr);

    Q_INVOKABLE void onDbConnectionButtonClicked();
    Q_INVOKABLE void readDb();
private:
    // Обновить данные подключения-читалки в соответствии с данными подключения-писателя
    void updateFromWriter();

    DatabaseManager &m_writerDb;
    DatabaseManager m_readerDb;

    int m_readerDbStatus = false;
    QString m_readerLastError = "Ошибок нет.";

    QVector<RowForVisualization> m_rowsForVisualization;

public slots:
    void slotSetDbStatus(int dbStatus);
    void slotSetLastError(QString lastError);
    void slotSetData(QVector<RowForVisualization> rows, QVector<QVector<double>> convsH, QVector<QVector<double>> convsV);
};
