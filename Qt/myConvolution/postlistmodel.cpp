#include "postlistmodel.h"
#include "generatorbackend.h"

PostListModel::PostListModel(GeneratorBackend *generatorBackend, std::vector<LoadGenerator::PostConfig> &config, QObject *parent) :
    QAbstractListModel {parent},
    m_generatorBackend(generatorBackend),
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

int PostListModel::addPost() {

    int newIndex = static_cast<int>(m_config.size());   // Получаем индекс нового поста
    LoadGenerator::PostConfig newConfig;    // Начальный конфиг нового поста
    newConfig.postName = QStringLiteral("Пост %1").arg(newIndex + 1);   // Начальное имя поста - его индекс
    newConfig.enabled = true;

    beginInsertRows(QModelIndex(), newIndex, newIndex);     // Начинаем вставлять
    m_config.push_back(std::move(newConfig)); // Засовываем в m_config новый конфиг
    m_generatorBackend->signalPostConfigUpdate(m_config);   // Подаём в loadgenerator сигнал обновить посты
    endInsertRows();    // Заканчиваем вставлять

    return newIndex;
}
