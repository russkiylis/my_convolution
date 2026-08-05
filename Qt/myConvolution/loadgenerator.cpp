#include <random>
#include <algorithm>
#include <iterator>
#include <QDebug>

#include "loadgenerator.h"

#include "geometryutils.h"

LoadGenerator::PostConfig::PostConfig() :
    enabled(false),
    latitude(60),
    longitude(30),
    frequency(100000000),
    level(10),
    levelSigma(1),
    minAngleH(0),
    maxAngleH(360),
    minAngleV(-45),
    maxAngleV(45),
    stepH(0.1),
    stepV(1),
    minPeriod(1000),
    maxPeriod(5000)
{
    noiseConfig = std::make_unique<NormalNoise::NormalNoiseConfig>(10, 1);
    peakConfigs.push_back(std::make_unique<GaussPeak::GaussPeakConfig>(GeometryUtils::SphericalCoordDeg(180, 0), 30, 10));
}

LoadGenerator::PostConfig::PostConfig(const PostConfig &other) :
    enabled(other.enabled),
    postName(other.postName),
    latitude(other.latitude),
    longitude(other.longitude),
    frequency(other.frequency),
    level(other.level),
    levelSigma(other.levelSigma),
    minAngleH(other.minAngleH),
    maxAngleH(other.maxAngleH),
    minAngleV(other.minAngleV),
    maxAngleV(other.maxAngleV),
    stepH(other.stepH),
    stepV(other.stepV),
    minPeriod(other.minPeriod),
    maxPeriod(other.maxPeriod),
    noiseConfig(other.noiseConfig->clone())
{
    peakConfigs.clear();
    // Проходим по вектору умных указателей и копируем их
    for (const auto &peakConfig : other.peakConfigs) {
        peakConfigs.push_back(peakConfig->clone());
    }
}

LoadGenerator::PostConfig & LoadGenerator::PostConfig::operator=(const PostConfig &other) {
    if (this == &other)
        return *this;
    enabled = other.enabled;
    postName = other.postName;
    latitude = other.latitude;
    longitude = other.longitude;
    frequency = other.frequency;
    level = other.level;
    levelSigma = other.levelSigma;
    minAngleH = other.minAngleH;
    maxAngleH = other.maxAngleH;
    minAngleV = other.minAngleV;
    maxAngleV = other.maxAngleV;
    stepH = other.stepH;
    stepV = other.stepV;
    minPeriod = other.minPeriod;
    maxPeriod = other.maxPeriod;
    noiseConfig = other.noiseConfig->clone();
    peakConfigs.clear();
    // Проходим по вектору умных указателей и копируем их
    for (const auto & peakConfig : other.peakConfigs) {
        peakConfigs.push_back(peakConfig->clone());
    }
    return *this;
}

LoadGenerator::Post::Post(PostConfig const &config, LoadGenerator *loadGenerator) :
    m_loadGenerator(loadGenerator),
    m_config(config),
    m_rng(std::random_device{}())
{
    m_config.noiseConfig->seed = std::random_device{}();
    m_noise = m_config.noiseConfig->createNoise(); // В зависимости от типа конфига нам выдадут разные шумы
    for (const auto & peakConfig : m_config.peakConfigs) {
        m_peaks.push_back(peakConfig->createPeak()); // В зависимости от типа конфига нам выдадут разные пики
    }

    // Инициализвация вектора направлений

    const double rangeH = m_config.maxAngleH - m_config.minAngleH;
    const double rangeV = m_config.maxAngleV - m_config.minAngleV;
    const auto stepCountH = static_cast<std::size_t>(
        std::floor(rangeH / m_config.stepH)
    );
    const auto stepCountV = static_cast<std::size_t>(
        std::floor(rangeV / m_config.stepV)
    );
    for (size_t i = 0; i < stepCountV; ++i ) {
        for (size_t j = 0; j < stepCountH; ++j) {
            m_direction.push_back(GeometryUtils::SphericalCoordDeg(static_cast<double>(j) * m_config.stepH + m_config.minAngleH,
                static_cast<double>(i) * m_config.stepV + m_config.minAngleV).toLinearCoord());
        }
    }

}

void LoadGenerator::Post::call(TimePoint const &now) {
    if (m_config.enabled) {  // Если пост включён
        if (now >= m_nextGenTime) {  // Если пришло время новой генерации

            // Расчёт свёртки
            m_data.conv.clear();
            for (const auto & direction : m_direction) {
                double valueH = m_noise->next();
                for (const auto & peak : m_peaks) {
                    valueH += peak->valueAt(direction);
                }
                m_data.conv.push_back(valueH);
            }

            // Взятие свёрток по модулю IDK: возможно брать модуль не надо
            for (auto & value : m_data.conv) {
                value = std::abs(value);
            }

            // Нормировка свёртки и запись качества и направления
            const auto maxIt = std::max_element(m_data.conv.begin(), m_data.conv.end());
            const double maxValue = *maxIt;
            for (auto & value : m_data.conv) {
                value /= maxValue;
            }
            m_data.quality = maxValue;   // Записываем это как качество IDK: Я не знаю как расчитать quality
            const auto maxIndex = std::distance(m_data.conv.begin(), maxIt);
            m_data.bearing = m_direction[maxIndex].toSphericalCoordDeg();   // Записываем направление по максимуму

            // Расчёт временной метки
            m_data.timestamp = QDateTime::currentDateTimeUtc();

            // Далее копирование информации которую мы и так знаем
            std::normal_distribution<double> levelDistribution(m_config.level, m_config.levelSigma);
            m_data.level = levelDistribution(m_rng);
            m_data.frequency = m_config.frequency;
            m_data.coordinate.setLatitude(m_config.latitude);
            m_data.coordinate.setLongitude(m_config.longitude);
            m_data.postName = m_config.postName;

            m_data.minAngleH = m_config.minAngleH;
            m_data.maxAngleH = m_config.maxAngleH;
            m_data.minAngleV = m_config.minAngleV;
            m_data.maxAngleV = m_config.maxAngleV;
            m_data.stepH = m_config.stepH;
            m_data.stepV = m_config.stepV;

            newNextGenTime();   // Генерируем время, через которое произойдёт новая генерация

            m_loadGenerator->sendData(m_data);     // Отправляем сигнал
        }
    }
}

void LoadGenerator::Post::newNextGenTime()
{
    std::uniform_real_distribution<double> cooldownDistribution {
        static_cast<double>(m_config.minPeriod.count())/1000, static_cast<double>(m_config.maxPeriod.count())/1000
    }; // Создаём закон равномерного распределения
    const double cooldownSeconds = cooldownDistribution(m_rng);  // Генерируем кд в секундах
    const auto cooldownDuration = std::chrono::duration<double>(cooldownSeconds); // Кастим в duration

    // Кастим в steady_clock::duration и складываем со steady_clock_now
    m_nextGenTime = Clock::now() + std::chrono::duration_cast<Clock::duration>(cooldownDuration);
}


LoadGenerator::LoadGenerator(std::vector<PostConfig> const &postConfigs, QObject *parent) :
    QObject(parent),
    m_postConfigs(postConfigs),
    m_callingEnabled(false)
{
    load();
}

std::vector<LoadGenerator::PostConfig> LoadGenerator::postConfigs() const
{
    return m_postConfigs;
}

void LoadGenerator::setPostConfigs(const std::vector<PostConfig> &postConfigs)
{
    m_postConfigs = postConfigs;
    load();
}

void LoadGenerator::load() {
    // FIXME: тут бы тоже почекать что мы в конфигах натворили
    m_posts.clear();
    for (const auto & postConfig : m_postConfigs) {
        m_posts.emplace_back(postConfig, this);    // Закидываем в вектор Posts, инициализованные postConfig
    }
    qDebug() << "Настройки генератора нагрузки обновлены.";
}

void LoadGenerator::sendData(DataPackage const &package)
{
    emit signalSendData(package);
}

void LoadGenerator::slotPostCallToggle(const bool toggle) {
    if (toggle) {   // Если мы хотим включить опрос постов
        if (m_callingEnabled) {
            qDebug() << "[!] Попытка включить включенный опрос постов генератора!";
            return;
        }

        Q_ASSERT(QThread::currentThread() == thread()); // Проверка на то что мы в потоке
        if (m_timer == nullptr)
            m_timer = new QTimer(this);  // Создаём таймер
        m_timer->setTimerType(Qt::TimerType::PreciseTimer);
        m_timer->setInterval(10);

        // Подключаем таймер к слоту, который будет опрашивать посты
        connect(m_timer, &QTimer::timeout, this, &LoadGenerator::slotPostCall);

        m_timer->start();
        m_callingEnabled = true;
        qDebug() << "Опрос постов генератора запущен.";
        emit signalPostCallToggle(m_callingEnabled);

    } else {    // Если мы хотим выключить опрос постов
        if (!m_callingEnabled) {
            qDebug() << "[!] Попытка выключить выключенный опрос постов генератора!";
            return;
        }

        Q_ASSERT(QThread::currentThread() == thread()); // Проверка на то что мы в потоке
        if (m_timer != nullptr)
            m_timer->stop();     // Останавливаем таймер
        m_callingEnabled = false;
        qDebug() << "Опрос постов генератора остановлен.";
        emit signalPostCallToggle(m_callingEnabled);
    }
}

void LoadGenerator::slotPostConfigUpdate(const std::vector<PostConfig>& newConfig) {
    setPostConfigs(newConfig);
}

void LoadGenerator::slotPostCall() {
    for (auto & post : m_posts) {
        post.call(std::chrono::steady_clock::now());
    }
}
