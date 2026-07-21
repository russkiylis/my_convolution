#include "postlistmodel.h"
#include "generatorbackend.h"

void PostListModel::setFallbackConfig(const std::vector<LoadGenerator::PostConfig> &fallbackConfig)
{
    m_fallbackConfig = fallbackConfig;
}

int PostListModel::postIndex() const
{
    qDebug() << m_postIndex;
    return m_postIndex;
}

void PostListModel::setPostIndex(int postIndex)
{
    // qDebug() << postIndex;
    m_postIndex = postIndex;
    emits();
}

QString PostListModel::currentPostName() const {
    return m_config[m_postIndex].postName;
}

void PostListModel::setCurrentPostName(const QString &currentPostName) {
    // qDebug() << currentPostName;
    m_config[m_postIndex].postName = currentPostName;
    emit currentPostNameChanged(currentPostName);
}

QString PostListModel::currentLatitude() const {
    return QString::number(m_config[m_postIndex].latitude);
}

void PostListModel::setCurrentLatitude(const QString &currentLatitude) {
    m_config[m_postIndex].latitude = currentLatitude.toDouble();
    emit currentLatitudeChanged(currentLatitude);
}

QString PostListModel::currentLongitude() const {
    return QString::number(m_config[m_postIndex].longitude);
}

void PostListModel::setCurrentLongitude(const QString &currentLongitude) {
    m_config[m_postIndex].longitude = currentLongitude.toDouble();
    emit currentLongitudeChanged(currentLongitude);
}

QString PostListModel::currentFrequency() const {
    return QString::number(m_config[m_postIndex].frequency);
}

void PostListModel::setCurrentFrequency(const QString &frequency) {
    m_config[m_postIndex].frequency = frequency.toDouble();
    emit currentFrequencyChanged(frequency);
}

QString PostListModel::currentLevel() const {
    return QString::number(m_config[m_postIndex].level);
}

void PostListModel::setCurrentLevel(const QString &currentLevel) {
    m_config[m_postIndex].level = currentLevel.toDouble();
    emit currentLevelChanged(currentLevel);
}

QString PostListModel::currentLevelSigma() const {
    return QString::number(m_config[m_postIndex].levelSigma);
}

void PostListModel::setCurrentLevelSigma(const QString &currentLevelSigma) {
    m_config[m_postIndex].levelSigma = currentLevelSigma.toDouble();
    emit currentLevelSigmaChanged(currentLevelSigma);
}

QString PostListModel::currentMinAngleH() const {
    return QString::number(m_config[m_postIndex].minAngleH);
}

void PostListModel::setCurrentMinAngleH(const QString &currentMinAngleH) {
    m_config[m_postIndex].minAngleH = currentMinAngleH.toInt();
    emit currentMinAngleHChanged(currentMinAngleH);
}

QString PostListModel::currentMaxAngleH() const {
    return QString::number(m_config[m_postIndex].maxAngleH);
}

void PostListModel::setCurrentMaxAngleH(const QString &currentMaxAngleH) {
    m_config[m_postIndex].maxAngleH = currentMaxAngleH.toInt();
}

int PostListModel::currentStepH() const {
    constexpr std::array<double, 5> etalonStep = {1, 0.5, 0.2, 0.1, 0.01};
    constexpr double epsilon = 0.001;
    for (std::size_t i = 0; i < etalonStep.size(); ++i) {
        if (std::abs(m_config[m_postIndex].stepH - etalonStep[i]) <= epsilon) {
            return static_cast<int>(i);
        }
    }
    throw std::runtime_error("Сломанный StepH");
}

void PostListModel::setCurrentStepH(int currentStepH) {
    if (currentStepH < 0 || currentStepH > 4)
        throw std::runtime_error("Сломанный StepH");

    switch (static_cast<AngleStep>(currentStepH)) {
    case _1:
        m_config[m_postIndex].stepH = 1;
        emit currentStepHChanged(currentStepH);
        break;
    case _05:
        m_config[m_postIndex].stepH = 0.5;
        emit currentStepHChanged(currentStepH);
        break;
    case _02:
        m_config[m_postIndex].stepH = 0.2;
        emit currentStepHChanged(currentStepH);
        break;
    case _01:
        m_config[m_postIndex].stepH = 0.1;
        emit currentStepHChanged(currentStepH);
        break;
    case _001:
        m_config[m_postIndex].stepH = 0.01;
        emit currentStepHChanged(currentStepH);
    }
}

QString PostListModel::currentMinAngleV() const {
    return QString::number(m_config[m_postIndex].minAngleV);
}

void PostListModel::setCurrentMinAngleV(const QString &currentMinAngleV) {
    m_config[m_postIndex].minAngleV = currentMinAngleV.toInt();
    emit currentMinAngleVChanged(currentMinAngleV);
}

QString PostListModel::currentMaxAngleV() const {
    return QString::number(m_config[m_postIndex].maxAngleV);
}

void PostListModel::setCurrentMaxAngleV(const QString &currentMaxAngleV) {
    m_config[m_postIndex].maxAngleV = currentMaxAngleV.toInt();
    emit currentMinAngleHChanged(currentMaxAngleV);
}

int PostListModel::currentStepV() const {
    constexpr std::array<double, 5> etalonStep = {1, 0.5, 0.2, 0.1, 0.01};
    constexpr double epsilon = 0.001;
    for (std::size_t i = 0; i < etalonStep.size(); ++i) {
        if (std::abs(m_config[m_postIndex].stepV - etalonStep[i]) <= epsilon) {
            return static_cast<int>(i);
        }
    }
    throw std::runtime_error("Сломанный StepV");
}

void PostListModel::setCurrentStepV(int currentStepV) {
    if (currentStepV < 0 || currentStepV > 4)
        throw std::runtime_error("Сломанный StepV");

    switch (static_cast<AngleStep>(currentStepV)) {
    case _1:
        m_config[m_postIndex].stepV = 1;
        emit currentStepVChanged(currentStepV);
        break;
    case _05:
        m_config[m_postIndex].stepV = 0.5;
        emit currentStepVChanged(currentStepV);
        break;
    case _02:
        m_config[m_postIndex].stepV = 0.2;
        emit currentStepVChanged(currentStepV);
        break;
    case _01:
        m_config[m_postIndex].stepV = 0.1;
        emit currentStepVChanged(currentStepV);
        break;
    case _001:
        m_config[m_postIndex].stepV = 0.01;
        emit currentStepVChanged(currentStepV);
    }
}

QString PostListModel::currentMinPeriod() const {
    return QString::number(m_config[m_postIndex].minPeriod.count());
}

void PostListModel::setCurrentMinPeriod(const QString &currentMinPeriod) {
    m_config[m_postIndex].minPeriod = std::chrono::milliseconds(currentMinPeriod.toInt());
    emit currentMinPeriodChanged(currentMinPeriod);
}

QString PostListModel::currentMaxPeriod() const {
    return QString::number(m_config[m_postIndex].maxPeriod.count());
}

void PostListModel::setCurrentMaxPeriod(const QString &currentMaxPeriod) {
    m_config[m_postIndex].maxPeriod = std::chrono::milliseconds(currentMaxPeriod.toInt());
    emit currentMinPeriodChanged(currentMaxPeriod);
}

bool PostListModel::currentPostEnabled() const {
    return m_config[m_postIndex].enabled;
}

void PostListModel::setCurrentPostEnabled(bool currentPostEnabled) {
    m_config[m_postIndex].enabled = currentPostEnabled;
    emit currentPostEnabledChanged(currentPostEnabled);
}

PostListModel::PostListModel(GeneratorBackend *generatorBackend, std::vector<LoadGenerator::PostConfig> &config, QObject *parent) :
    QAbstractListModel {parent},
    m_generatorBackend(generatorBackend),
    m_config(config),
    m_fallbackConfig(config)
{

}

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
    // m_generatorBackend->signalPostConfigUpdate(m_config);   // Подаём в loadgenerator сигнал обновить посты
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
    // m_generatorBackend->signalPostConfigUpdate(m_config);
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
    emits();
    endResetModel();
    m_fallbackConfig = m_config;    // Теперь будем откатываться до этого состояния
}

void PostListModel::fallback() {
    m_config = m_fallbackConfig; // Отменяем все изменения
    beginResetModel();
    emits();
    endResetModel();
    qDebug() << "Изменения сброшены.";
}

void PostListModel::emits() {
    emit currentPostNameChanged(currentPostName());
    emit currentLatitudeChanged(currentLatitude());
    emit currentLongitudeChanged(currentLongitude());
    emit currentFrequencyChanged(currentFrequency());
    emit currentLevelChanged(currentLevel());
    emit currentLevelSigmaChanged(currentLevelSigma());
    emit currentMinAngleHChanged(currentMinAngleH());
    emit currentMaxAngleHChanged(currentMaxAngleH());
    emit currentStepHChanged(currentStepH());
    emit currentMinAngleVChanged(currentMinAngleV());
    emit currentMaxAngleVChanged(currentMaxAngleV());
    emit currentStepVChanged(currentStepV());
    emit currentMinPeriodChanged(currentMinPeriod());
    emit currentMaxPeriodChanged(currentMaxPeriod());
    emit currentPostEnabledChanged(currentPostEnabled());
}
