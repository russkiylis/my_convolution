#include "visualizationtablemodel.h"

#include "utils.h"

VisualizationTableModel::VisualizationTableModel(QObject *parent) :
    QAbstractTableModel {parent}
{
}

int VisualizationTableModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;

    return m_data.rows.size();
}

int VisualizationTableModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;

    return ColumnCount;
}

QVariant VisualizationTableModel::data(const QModelIndex &index, int role) const {

    if (!index.isValid())
        return {};

    if (index.row() < 0 || index.row() >= m_data.rows.size())
        return {};

    if (index.column() < 0 || index.column() >= ColumnCount)
        return {};

    RowForVisualization const & row = m_data.rows.at(index.row());
    QVector<double> const & conv = m_data.convs.at(index.row());

    // Обрабатываем те колонки, которые надо просто вывести текстом
    if (role == Qt::DisplayRole) {
        switch (static_cast<Column>(index.column())) {
        case IdColumn:
            return row.id;
        case TimestampColumn:
            return row.timestamp.toLocalTime().toString("dd.MM.yyyy HH:mm:ss.zzz");
        case SysnameColumn:
            return row.sysname;
        case AzimuthColumn:
            return row.azimuth;
        case ElevationColumn:
            return row.elevation;
        case PowerColumn:
            return row.power;
        case FrequencyColumn:
            return row.frequency;
        case LatitudeColumn:
            return row.latitude;
        case LongitudeColumn:
            return row.longitude;
        case QualityColumn:
            return row.quality;
        case ConvColumn:
            return "";
        default:
            return {};
        }
    }

    if (role == IsSparklineRole) {
        return index.column() == ConvColumn;
    }

    if (role == SparklineRole) {
        QVariantList result;
        QVector<double> shrinkedConvH;

        switch (static_cast<Column>(index.column())) {
        case ConvColumn:
            shrinkedConvH = Utils::shrinkVector(Utils::flattenVector(conv, row.countH), 100);
            result.reserve(shrinkedConvH.size());
            for (const double i : shrinkedConvH) {
                result.append(i);
            }
            return result;
        default:
            return {};
        }
    }

    return {};
}

QVariant VisualizationTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation != Qt::Horizontal)
        return {};

    switch (static_cast<Column>(section)) {
    case IdColumn:
        return QStringLiteral("ID");
    case TimestampColumn:
        return QStringLiteral("Временная метка");
    case SysnameColumn:
        return QStringLiteral("Имя поста");
    case AzimuthColumn:
        return QStringLiteral("Азимут");
    case ElevationColumn:
        return QStringLiteral("Угол места");
    case PowerColumn:
        return QStringLiteral("Мощность");
    case FrequencyColumn:
        return QStringLiteral("Частота");
    case LatitudeColumn:
        return QStringLiteral("Широта");
    case LongitudeColumn:
        return QStringLiteral("Долгота");
    case QualityColumn:
        return QStringLiteral("Качество");
    case ConvColumn:
        return QStringLiteral("Свёртка (азимут)");
    default:
        return {};
    }
}

QHash<int, QByteArray> VisualizationTableModel::roleNames() const
{
    return {
            {Qt::DisplayRole, "display"},
            {SparklineRole, "sparkline"},
            {IsSparklineRole, "isSparkline"}
    };
}

void VisualizationTableModel::replaceData(
    const QVector<RowForVisualization> &rows,
    const QVector<QVector<double>> &convs
    ) {
    beginResetModel();

    m_data.convs = convs;
    m_data.rows = rows;

    endResetModel();
}
