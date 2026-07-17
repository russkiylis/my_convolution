#include "postlistmodel.h"

PostListModel::PostListModel(std::vector<LoadGenerator::PostConfig> &config, QObject *parent) :
    QAbstractListModel {parent},
    _config(config)
{}

int PostListModel::rowCount(const QModelIndex &parent) const {
    return static_cast<int>(_config.size());
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
    const LoadGenerator::PostConfig &config = _config.at(index.row());

    if (_config.size() == 0)
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
