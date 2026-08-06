#pragma once

#include <memory>
#include <qqml.h>
#include <qabstractitemmodel.h>
#include "dataforvisualization.h"

class VisualizationTableModel : public QAbstractTableModel {
    Q_OBJECT

public:

    // Колонки
    enum Column
    {
        IdColumn,
        TimestampColumn,
        SysnameColumn,
        AzimuthColumn,
        ElevationColumn,
        PowerColumn,
        FrequencyColumn,
        LatitudeColumn,
        LongitudeColumn,
        QualityColumn,
        ConvColumn,

        ColumnCount
    };

    // Роли (как отображать колонку - просто текстом либо спарклайном)
    enum Role
    {
        SparklineRole = Qt::UserRole + 1,
        IsSparklineRole
    };

    // Конструктор
    explicit VisualizationTableModel(QObject *parent = nullptr);

    // Подсчёт строк
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;

    // Подсчёт колонок
    [[nodiscard]] int columnCount(const QModelIndex &parent) const override;

    // Выдача данных
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

    // Выдача данных для хедера
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    // Подача имён ролей в qml
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    // Замена отображаемых данных
    void replaceData(const QVector<RowForVisualization> &rows,
                 const QVector<QVector<double>> &convs);

private:
    DataForVisualization m_data;
};

