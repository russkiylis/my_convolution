#include <random>
#include <algorithm>
#include <iterator>

#include "loadgenerator.h"

LoadGenerator::PostConfig::PostConfig(const PostConfig &other) :
    enabled(false),
    postName(other.postName),
    latitude(other.latitude),
    longitude(other.longitude),
    frequency(other.frequency),
    level(other.level),
    levelSigma(other.levelSigma),
    minAngleH(other.minAngleH),
    maxAngleH(other.maxAngleH),
    minAngleV(0),
    maxAngleV(0),
    stepH(other.stepH),
    stepV(0),
    minPeriod(other.minPeriod),
    maxPeriod(other.maxPeriod),
    noiseConfig(other.noiseConfig->clone())
{
    // Проходим по вектору умных указателей и копируем их
    for (const auto &peakConfig : other.peakConfigsV) {
        peakConfigsV.push_back(peakConfig->clone());
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
    // Проходим по вектору умных указателей и копируем их
    for (const auto & peakConfig : other.peakConfigsV) {
        peakConfigsV.push_back(peakConfig->clone());
    }
    return *this;
}

LoadGenerator::Post::Post(PostConfig const &config) :
    _config(config),
    _rng(std::random_device{}())
{
    _noise = _config.noiseConfig->createNoise(); // В зависимости от типа конфига нам выдадут разные шумы
    for (const auto & peakConfig : _config.peakConfigsV) {
        _peaksV.push_back(peakConfig->createPeak()); // В зависимости от типа конфига нам выдадут разные пики
    }
    for (const auto & peakConfig : _config.peakConfigsH) {
        _peaksH.push_back(peakConfig->createPeak()); // В зависимости от типа конфига нам выдадут разные пики
    }

    // Инициализвация вектора градусов (горизонтальных)

    const double rangeH = _config.maxAngleH - _config.minAngleH;
    const auto stepCountH = static_cast<std::size_t>(
        std::floor(rangeH / _config.stepH)
    );
    for (size_t i = 0; i <= stepCountH; ++i ) {
        _degH.push_back(static_cast<double>(i) * _config.stepH + _config.minAngleH);
    }

    // Инициализация вектора градусов (вертикальных)

    const double rangeV = _config.maxAngleV - _config.minAngleV;
    const auto stepCountV = static_cast<std::size_t>(
        std::floor(rangeV / _config.stepV)
    );
    for (size_t i = 0; i <= stepCountV; ++i ) {
        _degV.push_back(static_cast<double>(i) * _config.stepV + _config.minAngleV);
    }

}

void LoadGenerator::Post::call(TimePoint const &now) {
    if (_config.enabled) {  // Если пост включён
        if (now >= _nextGenTime) {  // Если пришло время новой генерации

            // Расчёт свёртки по горизонтали
            _data.convH.clear();
            for (const auto & deg : _degH) {
                double valueH = _noise->next();
                for (const auto & peak : _peaksH) {
                    valueH += peak->valueAt(deg);
                }
                _data.convH.push_back(valueH);
            }

            // Расчёт свёртки по вертикали
            _data.convV.clear();
            for (const auto & deg : _degV) {
                double valueV = _noise->next();
                for (const auto & peak : _peaksV) {
                    valueV += peak->valueAt(deg);
                }
                _data.convV.push_back(valueV);
            }

            // Нормировка горизонтальной свёртки и запись качества и направления
            const auto maxItH = std::max_element(_data.convH.begin(), _data.convH.end());
            const double maxValueH = *maxItH;
            for (auto & value : _data.convH) {
                value /= maxValueH;
            }
            _data.qualityH = maxValueH;   // Записываем это как качество IDK: Я не знаю как расчитать quality
            const auto maxIndexH = std::distance(_data.convH.begin(), maxItH);
            _data.bearingH = _degH[maxIndexH];   // Записываем направление по максимуму

            // Нормировка вертикальной свёртки и запись качества и направления
            const auto maxItV = std::max_element(_data.convV.begin(), _data.convV.end());
            const double maxValueV = *maxItV;
            for (auto & value : _data.convV) {
                value /= maxValueV;
            }
            _data.qualityV = maxValueV;   // Записываем это как качество IDK: Я не знаю как расчитать quality
            const auto maxIndexV = std::distance(_data.convV.begin(), maxItV);
            _data.bearingV = _degH[maxIndexV];   // Записываем направление по максимуму

            // Расчёт временной метки
            _data.timestamp = QDateTime::currentDateTimeUtc();

            // Далее копирование информации которую мы и так знаем

            _data.level = _config.level;
            _data.frequency = _config.frequency;
            _data.coordinate.setLatitude(_config.latitude);
            _data.coordinate.setLongitude(_config.longitude);
            _data.postName = _config.postName;

            _data.minAngleH = _config.minAngleH;
            _data.maxAngleH = _config.maxAngleH;
            _data.minAngleV = _config.minAngleV;
            _data.maxAngleV = _config.maxAngleV;
            _data.stepH = _config.stepH;
            _data.stepV = _config.stepV;

            newNextGenTime();   // Генерируем время, через которое произойдёт новая генерация

            emit signalSendData(_data);     // Отправляем сигнал
        }
    }
}

void LoadGenerator::Post::newNextGenTime()
{
    std::uniform_real_distribution<double> cooldownDistribution {
        static_cast<double>(_config.minPeriod), static_cast<double>(_config.maxPeriod)
    }; // Создаём закон равномерного распределения
    const double cooldownSeconds = cooldownDistribution(_rng);  // Генерируем кд в секундах
    const auto cooldownDuration = std::chrono::duration<double>(cooldownSeconds); // Кастим в duration

    // Кастим в steady_clock::duration и складываем со steady_clock_now
    _nextGenTime = Clock::now() + std::chrono::duration_cast<Clock::duration>(cooldownDuration);
}


LoadGenerator::LoadGenerator(std::vector<PostConfig> const &postConfigs, QObject *parent) :
    QObject(parent),
    _postConfigs(postConfigs)
{
    load();
}

std::vector<LoadGenerator::PostConfig> LoadGenerator::postConfigs() const
{
    return _postConfigs;
}

void LoadGenerator::setPostConfigs(const std::vector<PostConfig> &postConfigs)
{
    _postConfigs = postConfigs;
    load();
}

void LoadGenerator::load() {
    _posts.clear();
    for (const auto & postConfig : _postConfigs) {
        _posts.emplace_back(postConfig);    // Закидываем в вектор Postы, инициализованные postConfigом
    }
}
