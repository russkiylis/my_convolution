#include "postlistmodel.h"
#include "generatorbackend.h"

int PostListModel::postIndex() const
{
    qDebug() << m_postIndex;
    return m_postIndex;
}

void PostListModel::setPostIndex(int postIndex)
{
    qDebug() << postIndex;
    m_postIndex = postIndex;
    postUpdate();
}

QString PostListModel::currentPostName() const {
    return m_config[m_postIndex].postName;
}

void PostListModel::setCurrentPostName(const QString &currentPostName) {
    qDebug() << currentPostName;
    m_config[m_postIndex].postName = currentPostName;
    emit currentPostNameChanged(currentPostName);
}

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

    setPostIndex(newIndex);
    return newIndex;
}

int PostListModel::removePost(int index) {

    int oldSize = static_cast<int>(m_config.size());
    if (index < 0 || index >= oldSize) {
        return -1;
    }

    beginRemoveRows(QModelIndex(), index, index);
    m_config.erase(m_config.begin() + index);
    m_generatorBackend->signalPostConfigUpdate(m_config);
    endRemoveRows();

    int newSize = static_cast<int>(m_config.size());

    // Список стал пустым, значит выбранного элемента больше нет
    if (newSize == 0) {
        int i = -1;
        setPostIndex(i);
        return i;
    }
    // Если удалили последний элемент
    if (index >= newSize) {
        int i = newSize - 1;
        setPostIndex(i);
        return i;
    }
    // Если удалили из середины, то индекс менять не нужно
    setPostIndex(index);
    return index;

}

void PostListModel::postUpdate() {
    beginResetModel();
    m_generatorBackend->signalPostConfigUpdate(m_config);   // Подаём в loadgenerator сигнал обновить посты
    emit currentPostNameChanged(currentPostName());
    endResetModel();
}
