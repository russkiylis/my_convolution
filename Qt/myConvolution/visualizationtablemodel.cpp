#include "visualizationtablemodel.h"

#include "utils.h"

VisualizationTableModel::VisualizationTableModel(QObject *parent) :
QAbstractTableModel{parent}
{
}

int VisualizationTableModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;

    return m_data.size();
}

int VisualizationTableModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;

    return ColumnCount;
}

QVariant VisualizationTableModel::data(const QModelIndex &index, int role) const {

    if (!index.isValid())
        return {};

    if (index.row() < 0 || index.row() >= m_data.size())
        return {};

    if (index.column() < 0 || index.column() >= ColumnCount)
        return {};

    DataForVisualization data = m_data.at(index.row());

    // Обрабатываем те колонки, которые надо просто вывести текстом
    if (role == Qt::DisplayRole) {
        switch (static_cast<Column>(index.column())) {
        case IdColumn:
            return data.row.id;
        case TimestampColumn:
            return data.row.timestamp.toLocalTime().toString("dd.MM.yyyy HH:mm:ss.zzz");
        case SysnameColumn:
            return data.row.sysname;
        case AzimuthColumn:
            return data.row.azimuth;
        case ElevationColumn:
            return data.row.elevation;
        case PowerColumn:
            return data.row.power;
        case FrequencyColumn:
            return data.row.frequency;
        case LatitudeColumn:
            return data.row.latitude;
        case LongitudeColumn:
            return data.row.longitude;
        case QualityHColumn:
            return data.row.qualityH;
        case QualityVColumn:
            return data.row.qualityV;
        case ConvHColumn:
            return "";
        case ConvVColumn:
            return "";
        default:
            return {};
        }
    }

    if (role == IsSparklineRole) {
        return index.column() == ConvHColumn || index.column() == ConvVColumn;
    }

    if (role == SparklineRole) {
        QVariantList result;
        QVector<double> shrinkedConvH;
        QVector<double> shrinkedConvV;

        switch (static_cast<Column>(index.column())) {
        case ConvHColumn:
            shrinkedConvH = Utils::shrinkVector(data.convH, 100);
            result.reserve(shrinkedConvH.size());
            for (const double i : shrinkedConvH) {
                result.append(i);
            }
            return result;
        case ConvVColumn:
            shrinkedConvV = Utils::shrinkVector(data.convV, 100);
            result.reserve(shrinkedConvV.size());
            for (const double i : shrinkedConvV) {
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
    case QualityHColumn:
        return QStringLiteral("Качество (азимут)");
    case QualityVColumn:
        return QStringLiteral("Качество (угол места)");
    case ConvHColumn:
        return QStringLiteral("Свёртка (азимут)");
    case ConvVColumn:
        return QStringLiteral("Свёртка (угол места)");
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
    const QVector<QVector<double>> &convsH,
    const QVector<QVector<double>> &convsV
    ) {
    beginResetModel();
    // TODO: Записать данные
    m_data.clear();
    for (int i = 0; i < rows.size(); ++i) {
        DataForVisualization data;
        data.row = rows.at(i);
        data.convH = convsH.at(i);
        data.convV = convsV.at(i);
        m_data.push_back(data);
    }
    endResetModel();
}
