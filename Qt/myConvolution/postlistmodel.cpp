#include "postlistmodel.h"
#include "generatorbackend.h"

void PostListModel::setFallbackConfig(const std::vector<LoadGenerator::PostConfig> &fallbackConfig)
{
    m_fallbackConfig = fallbackConfig;
}

int PostListModel::postIndex() const
{
    return m_postIndex;
}

void PostListModel::setPostIndex(const int postIndex)
{
    if (postIndex < 0 || postIndex >= static_cast<int>(m_config.size()))
        throw std::runtime_error("Сломанный postIndex");
    if (postIndex == m_postIndex)
        return;

    m_postIndex = postIndex;
    qmlUpdate();
}

QString PostListModel::currentPostName() const {
    return m_config[m_postIndex].postName;
}

void PostListModel::setCurrentPostName(const QString &currentPostName) {
    QString postName = currentPostName;
    if (currentPostName.trimmed().isEmpty()) {
        postName = QStringLiteral("Пост %1").arg(postIndex() + 1);
    }

    if (postName == m_config[m_postIndex].postName)
        return;

    m_config[m_postIndex].postName = postName;
    emit currentPostNameChanged(postName);
}

QString PostListModel::currentLatitude() const {
    return QString::number(m_config[m_postIndex].latitude);
}

void PostListModel::setCurrentLatitude(const QString &currentLatitude) {
    bool ok = false;
    const double parsedLatitude = currentLatitude.toDouble(&ok);

    // Если вместо числа мусор, то ничего не трогаем
    if (!ok || !std::isfinite(parsedLatitude))
        return;
    const double latitude = std::clamp(parsedLatitude, -90.0, 90.0);

    if (latitude == m_config[m_postIndex].latitude)
        return;

    m_config[m_postIndex].latitude = latitude;
    emit currentLatitudeChanged(QString::number(latitude));
}

QString PostListModel::currentLongitude() const {
    return QString::number(m_config[m_postIndex].longitude);
}

void PostListModel::setCurrentLongitude(const QString &currentLongitude) {
    bool ok = false;
    const double parsedLongitude = currentLongitude.toDouble(&ok);

    // Если вместо числа мусор, то ничего не трогаем
    if (!ok || !std::isfinite(parsedLongitude))
        return;
    const double longitude = std::clamp(parsedLongitude, -180.0, 180.0);

    if (longitude == m_config[m_postIndex].longitude)
        return;

    m_config[m_postIndex].longitude = longitude;
    emit currentLongitudeChanged(QString::number(longitude));
}

QString PostListModel::currentFrequency() const {
    return QString::number(m_config[m_postIndex].frequency);
}

void PostListModel::setCurrentFrequency(const QString &currentFrequency) {
    bool ok = false;
    const double parsedFrequency = currentFrequency.toDouble(&ok);

    // Если вместо числа мусор, то ничего не трогаем
    if (!ok || !std::isfinite(parsedFrequency))
        return;
    const double frequency = std::clamp(parsedFrequency, 0.0, static_cast<double>(std::numeric_limits<float>::max())); // IDK: Сколько минимум ГЦ?

    if (frequency == m_config[m_postIndex].frequency)
        return;

    m_config[m_postIndex].frequency = frequency;
    emit currentFrequencyChanged(QString::number(frequency));
}

QString PostListModel::currentLevel() const {
    return QString::number(m_config[m_postIndex].level);
}

void PostListModel::setCurrentLevel(const QString &currentLevel) {
    bool ok = false;
    const double parsedLevel = currentLevel.toDouble(&ok);

    // Если вместо числа мусор, то ничего не трогаем
    if (!ok || !std::isfinite(parsedLevel))
        return;

    // IDK: Какой минимум/максимум у уровня?
    const double level = std::clamp(
        parsedLevel,
        static_cast<double>(std::numeric_limits<float>::lowest()),
        static_cast<double>(std::numeric_limits<float>::max())
    );

    if (level == m_config[m_postIndex].level)
        return;

    m_config[m_postIndex].level = level;
    emit currentLevelChanged(QString::number(level));
}

QString PostListModel::currentLevelSigma() const {
    return QString::number(m_config[m_postIndex].levelSigma);
}

void PostListModel::setCurrentLevelSigma(const QString &currentLevelSigma) {
    bool ok = false;
    const double parsedLevelSigma = currentLevelSigma.toDouble(&ok);

    if (!ok || !std::isfinite(parsedLevelSigma))
        return;

    // IDK: Каков разброс?
    const double levelSigma = std::clamp(
        parsedLevelSigma,
        static_cast<double>(std::numeric_limits<float>::min()),
        static_cast<double>(std::numeric_limits<float>::max())
    );

    if (levelSigma == m_config[m_postIndex].levelSigma)
        return;

    m_config[m_postIndex].levelSigma = levelSigma;
    emit currentLevelSigmaChanged(QString::number(levelSigma));
}

QString PostListModel::currentMinAngleH() const {
    return QString::number(m_config[m_postIndex].minAngleH);
}

void PostListModel::setCurrentMinAngleH(const QString &currentMinAngleH) {
    bool ok = false;
    const int parsedMinAngleH = currentMinAngleH.toInt(&ok);

    if (!ok)
        return;

    constexpr int lowerBound = 0;
    constexpr int upperBound = 359;
    const int currentMaxAngleH = m_config[m_postIndex].maxAngleH;
    const int allowedUpperBound = std::min(upperBound, currentMaxAngleH - 1);

    if (allowedUpperBound < lowerBound)
        return;

    const int minAngleH = std::clamp(parsedMinAngleH, lowerBound, allowedUpperBound);

    if (minAngleH == m_config[m_postIndex].minAngleH)
        return;

    m_config[m_postIndex].minAngleH = minAngleH;
    emit currentMinAngleHChanged(QString::number(minAngleH));
}

QString PostListModel::currentMaxAngleH() const {
    return QString::number(m_config[m_postIndex].maxAngleH);
}

void PostListModel::setCurrentMaxAngleH(const QString &currentMaxAngleH) {
    bool ok = false;
    const int parsedMaxAngleH = currentMaxAngleH.toInt(&ok);

    if (!ok)
        return;

    constexpr int lowerBound = 1;
    constexpr int upperBound = 360;
    const int currentMinAngleH = m_config[m_postIndex].minAngleH;
    const int allowedLowerBound = std::max(lowerBound, currentMinAngleH + 1);

    if (allowedLowerBound > upperBound)
        return;

    const int maxAngleH = std::clamp(parsedMaxAngleH, allowedLowerBound, upperBound);

    if (maxAngleH == m_config[m_postIndex].maxAngleH)
        return;

    m_config[m_postIndex].maxAngleH = maxAngleH;
    emit currentMaxAngleHChanged(QString::number(maxAngleH));
}

int PostListModel::currentStepH() const {
    constexpr std::array<double, 5> etalonStep = {1, 0.5, 0.2, 0.1, 0.01};
    for (std::size_t i = 0; i < etalonStep.size(); ++i) {
        if (constexpr double epsilon = 0.001; std::abs(m_config[m_postIndex].stepH - etalonStep[i]) <= epsilon) {
            return static_cast<int>(i);
        }
    }
    throw std::runtime_error("Сломанный StepH");
}

void PostListModel::setCurrentStepH(int currentStepH) {
    if (currentStepH < 0 || currentStepH > 4)
        throw std::runtime_error("Сломанный StepH");
    if (currentStepH == this->currentStepH())
        return;

    switch (static_cast<AngleStep>(currentStepH)) {
    case _1:
        m_config[m_postIndex].stepH = 1;
        break;
    case _05:
        m_config[m_postIndex].stepH = 0.5;
        break;
    case _02:
        m_config[m_postIndex].stepH = 0.2;
        break;
    case _01:
        m_config[m_postIndex].stepH = 0.1;
        break;
    case _001:
        m_config[m_postIndex].stepH = 0.01;
    }
    emit currentStepHChanged(currentStepH);
}

QString PostListModel::currentMinAngleV() const {
    return QString::number(m_config[m_postIndex].minAngleV);
}

void PostListModel::setCurrentMinAngleV(const QString &currentMinAngleV) {
    bool ok = false;
    const int parsedMinAngleV = currentMinAngleV.toInt(&ok);

    if (!ok)
        return;

    constexpr int lowerBound = -45;
    constexpr int upperBound = 44;
    const int currentMaxAngleV = m_config[m_postIndex].maxAngleV;
    const int allowedUpperBound = std::min(upperBound, currentMaxAngleV - 1);

    if (allowedUpperBound < lowerBound)
        return;

    const int minAngleV = std::clamp(parsedMinAngleV, lowerBound, allowedUpperBound);

    if (minAngleV == m_config[m_postIndex].minAngleV)
        return;

    m_config[m_postIndex].minAngleV = minAngleV;
    emit currentMinAngleVChanged(QString::number(minAngleV));
}

QString PostListModel::currentMaxAngleV() const {
    return QString::number(m_config[m_postIndex].maxAngleV);
}

void PostListModel::setCurrentMaxAngleV(const QString &currentMaxAngleV) {
    bool ok = false;
    const int parsedMaxAngleV = currentMaxAngleV.toInt(&ok);

    if (!ok)
        return;

    constexpr int lowerBound = -44;
    constexpr int upperBound = 45;
    const int currentMinAngleV = m_config[m_postIndex].minAngleV;
    const int allowedLowerBound = std::max(lowerBound, currentMinAngleV + 1);

    if (allowedLowerBound > upperBound)
        return;

    const int maxAngleV = std::clamp(parsedMaxAngleV, allowedLowerBound, upperBound);

    if (maxAngleV == m_config[m_postIndex].maxAngleV)
        return;

    m_config[m_postIndex].maxAngleV = maxAngleV;
    emit currentMaxAngleVChanged(QString::number(maxAngleV));
}

int PostListModel::currentStepV() const {
    constexpr std::array<double, 5> etalonStep = {1, 0.5, 0.2, 0.1, 0.01};
    for (std::size_t i = 0; i < etalonStep.size(); ++i) {
        if (constexpr double epsilon = 0.001; std::abs(m_config[m_postIndex].stepV - etalonStep[i]) <= epsilon) {
            return static_cast<int>(i);
        }
    }
    throw std::runtime_error("Сломанный StepV");
}

void PostListModel::setCurrentStepV(int currentStepV) {
    if (currentStepV < 0 || currentStepV > 4)
        throw std::runtime_error("Сломанный StepV");
    if (currentStepV == this->currentStepV())
        return;

    switch (static_cast<AngleStep>(currentStepV)) {
    case _1:
        m_config[m_postIndex].stepV = 1;
        break;
    case _05:
        m_config[m_postIndex].stepV = 0.5;
        break;
    case _02:
        m_config[m_postIndex].stepV = 0.2;
        break;
    case _01:
        m_config[m_postIndex].stepV = 0.1;
        break;
    case _001:
        m_config[m_postIndex].stepV = 0.01;
    }
    emit currentStepVChanged(currentStepV);
}

QString PostListModel::currentMinPeriod() const {
    return QString::number(m_config[m_postIndex].minPeriod.count());
}

void PostListModel::setCurrentMinPeriod(const QString &currentMinPeriod) {
    bool ok = false;
    const int parsedMinPeriod = currentMinPeriod.toInt(&ok);

    if (!ok)
        return;

    constexpr int lowerBound = 10;
    constexpr int upperBound = std::numeric_limits<short>::max();   // IDK: А я не знаю каков максимум
    const int currentMaxPeriod = static_cast<int>(m_config[m_postIndex].maxPeriod.count());
    const int allowedUpperBound = std::min(upperBound, currentMaxPeriod);

    if (allowedUpperBound < lowerBound)
        return;

    const int minPeriod = std::clamp(parsedMinPeriod, lowerBound, allowedUpperBound);

    if (minPeriod == m_config[m_postIndex].minPeriod.count())
        return;

    m_config[m_postIndex].minPeriod = std::chrono::milliseconds(minPeriod);
    emit currentMinPeriodChanged(QString::number(minPeriod));
}

QString PostListModel::currentMaxPeriod() const {
    return QString::number(m_config[m_postIndex].maxPeriod.count());
}

void PostListModel::setCurrentMaxPeriod(const QString &currentMaxPeriod) {
    bool ok = false;
    const int parsedMaxPeriod = currentMaxPeriod.toInt(&ok);

    if (!ok)
        return;

    constexpr int lowerBound = 10;
    constexpr int upperBound = std::numeric_limits<short>::max();   // IDK:  я не знаю каков максимум
    const int currentMinPeriod = static_cast<int>(m_config[m_postIndex].minPeriod.count());
    const int allowedLowerBound = std::max(lowerBound, currentMinPeriod);

    if (allowedLowerBound > upperBound)
        return;

    const int maxPeriod = std::clamp(parsedMaxPeriod, allowedLowerBound, upperBound);

    if (maxPeriod == m_config[m_postIndex].maxPeriod.count())
        return;

    m_config[m_postIndex].maxPeriod = std::chrono::milliseconds(maxPeriod);
    emit currentMaxPeriodChanged(QString::number(maxPeriod));
}

bool PostListModel::currentPostEnabled() const {
    return m_config[m_postIndex].enabled;
}

int PostListModel::currentNoiseType()
{
    // При первичном чтении создадим объект
    if (m_noiseBackend == nullptr) {
        switch (m_config[m_postIndex].noiseConfig->noiseType()) {
        case AbstractNoise::NoiseType::Normal:
            m_noiseBackend = std::make_unique<NormalNoiseBackend>(m_config, m_postIndex);
            emit noiseBackendChanged();
            break;
        case AbstractNoise::NoiseType::Uniform:
            m_noiseBackend = std::make_unique<UniformNoiseBackend>(m_config, m_postIndex);
            emit noiseBackendChanged();
        }
    }

    return static_cast<int>(m_config[m_postIndex].noiseConfig->noiseType());
}

void PostListModel::setCurrentNoiseType(int currentNoiseType) {
    switch (static_cast<AbstractNoise::NoiseType>(currentNoiseType)) {
    case AbstractNoise::NoiseType::Normal:
        if (!dynamic_cast<NormalNoise::NormalNoiseConfig*>(m_config[m_postIndex].noiseConfig.get())) {
            m_config[m_postIndex].noiseConfig = std::make_unique<NormalNoise::NormalNoiseConfig>(0,2);
        }
        if (!dynamic_cast<NormalNoiseBackend*>(m_noiseBackend.get())) {
            m_noiseBackend = std::make_unique<NormalNoiseBackend>(m_config, m_postIndex);
        }
        break;
    case AbstractNoise::NoiseType::Uniform:
        if (!dynamic_cast<UniformNoise::UniformNoiseConfig*>(m_config[m_postIndex].noiseConfig.get())) {
            m_config[m_postIndex].noiseConfig = std::make_unique<UniformNoise::UniformNoiseConfig>(-10,20);
        }
        if (!dynamic_cast<UniformNoiseBackend*>(m_noiseBackend.get())) {
            m_noiseBackend = std::make_unique<UniformNoiseBackend>(m_config, m_postIndex);
        }
        break;
    default:
        throw std::logic_error("Неправильный тип шума!");
    }
    emit currentNoiseTypeChanged(currentNoiseType);
    emit noiseBackendChanged();
}

void PostListModel::setCurrentPostEnabled(const bool currentPostEnabled) {
    if (currentPostEnabled == m_config[m_postIndex].enabled)
        return;
    m_config[m_postIndex].enabled = currentPostEnabled;
    emit currentPostEnabledChanged(currentPostEnabled);
}

PostListModel::PostListModel(GeneratorBackend *generatorBackend, std::vector<LoadGenerator::PostConfig> &config, QObject *parent) :
    QAbstractListModel {parent},
    m_config(config),
    m_fallbackConfig(config),
    m_generatorBackend(generatorBackend)
{
}

int PostListModel::rowCount(const QModelIndex &parent) const {
    if (!parent.isValid())
        return -1;
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

QVariant PostListModel::data(const QModelIndex &index, const int role) const {

    if (!index.isValid())
        return {};
    if (index.row() < 0 || index.row() >= static_cast<int>(m_config.size()))
        return {};
    if (m_config.empty())
        return {};

    // Получаем конфиг по индексу
    const LoadGenerator::PostConfig &config = m_config.at(index.row());

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

    const int newIndex = static_cast<int>(m_config.size());   // Получаем индекс нового поста
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

int PostListModel::removePost(const int index) {
    if (const int oldSize = static_cast<int>(m_config.size()); index < 0 || index >= oldSize) {
        return -1;
    }

    beginRemoveRows(QModelIndex(), index, index);
    m_config.erase(m_config.begin() + index);
    // m_generatorBackend->signalPostConfigUpdate(m_config);
    endRemoveRows();

    const int newSize = static_cast<int>(m_config.size());

    // Список стал пустым, значит выбранного элемента больше нет
    if (newSize == 0) {
        // FIXME: Сейчас интерфейс не даёт удалить все посты, но если это сделать, программа упадёт
        constexpr int i = -1;
        setPostIndex(i);
        return i;
    }
    // Если удалили последний элемент
    if (index >= newSize) {
        const int i = newSize - 1;
        setPostIndex(i);
        return i;
    }
    setPostIndex(index);
    qmlUpdate();
    return index;

}

void PostListModel::postUpdate() {
    beginResetModel();
    m_generatorBackend->signalPostConfigUpdate(m_config);   // Подаём в loadgenerator сигнал обновить посты
    endResetModel();
    m_fallbackConfig = m_config;    // Теперь будем откатываться до этого состояния
    m_fallbackPostIndex = postIndex();
}

int PostListModel::fallback() {
    beginResetModel();
    m_config = m_fallbackConfig; // Отменяем все изменения
    endResetModel();
    if (postIndex() >= static_cast<int>(m_fallbackConfig.size())) {
        setPostIndex(m_fallbackPostIndex);
    }
    qDebug() << "Изменения сброшены.";
    return postIndex();
}

AbstractNoiseBackend * PostListModel::noiseBackend() const {
    return m_noiseBackend.get();
}

void PostListModel::qmlUpdate() {
    setCurrentNoiseType(currentNoiseType()); // Для того чтобы адекватно выбрался бекенд шума

    emit postIndexChanged(postIndex());
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
    emit currentNoiseTypeChanged(currentNoiseType());

    m_noiseBackend->qmlUpdate();
}
