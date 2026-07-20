#include "postlistmodel.h"

PostListModel::PostListModel(std::vector<LoadGenerator::PostConfig> &config, QObject *parent) :
    QAbstractListModel {parent},
    m_config(config)
{}

int PostListModel::rowCount(const QModelIndex &parent) const {
    return static_cast<int>(m_config.size());
}

QHash<int, QByteArray> PostListModel::roleNames() const
{
    return {
        {PostNameRole, "postName"},  // text: model.postName
        {EnabledRole, "enabled"},
        {MinPeriodRole, "minPeriod"},
        {MaxPeriodRole, "maxPeriod"}
    };
}

QVariant PostListModel::data(const QModelIndex &index, int role) const {

    // Получаем конфиг по индексу
    const LoadGenerator::PostConfig &config = m_config.at(index.row());

    if (m_config.size() == 0)
        return {};

    // Проходим по ролям
    switch (role) {
        case PostNameRole:
            return config.postName;
        case EnabledRole:
            return config.enabled;
        case MinPeriodRole:
            return static_cast<int>(config.minPeriod.count());
        case MaxPeriodRole:
            return static_cast<int>(config.maxPeriod.count());
        default:
            return {};
    }

}
