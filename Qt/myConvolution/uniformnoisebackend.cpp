#include "uniformnoisebackend.h"
#include "utils.h"

UniformNoise::UniformNoiseConfig * UniformNoiseBackend::noiseConfigAcess() const {
    return dynamic_cast<UniformNoise::UniformNoiseConfig *>(m_config[m_postIndex].noiseConfig.get());
}

UniformNoiseBackend::UniformNoiseBackend(std::vector<LoadGenerator::PostConfig> &config, int &postIndex, QObject *parent):
    AbstractNoiseBackend(config, postIndex, parent)
{
    qDebug() << "UniformNoiseBackend прогружен.";
}

void UniformNoiseBackend::qmlUpdate() {
    emit currentMinChanged(currentMin());
    emit currentMaxChanged(currentMax());
}

QString UniformNoiseBackend::currentMin() const {
    return Utils::doubleToString(noiseConfigAcess()->min, 3);
}

void UniformNoiseBackend::setCurrentMin(const QString &currentMin)
{
    // if (double min = currentMin.toDouble(); min < std::numeric_limits<float>::min()) {
    //     min = std::numeric_limits<float>::min();
    //     if (min == currentMin.toDouble())
    //         return;
    //     m_config[m_postIndex].noiseConfig = std::make_unique<UniformNoise::UniformNoiseConfig>(min, currentMax().toDouble());
    //     emit currentMinChanged(QString::number(min));
    // } else if (min > std::numeric_limits<float>::max() - 1) {
    //     min = std::numeric_limits<float>::max() - 1;
    //     if (min == currentMin.toDouble())
    //         return;
    //     m_config[m_postIndex].noiseConfig = std::make_unique<UniformNoise::UniformNoiseConfig>(min, currentMax().toDouble());
    //     emit currentMinChanged(QString::number(min));
    // } else {
    //     if (min == currentMin.toDouble())
    //         return;
    //     m_config[m_postIndex].noiseConfig = std::make_unique<UniformNoise::UniformNoiseConfig>(min, currentMax().toDouble());
    //     emit currentMinChanged(currentMin);
    // }
    UniformNoise::UniformNoiseConfig *noiseConfig = noiseConfigAcess();
    constexpr double lowerBound = -1000.0;
    constexpr double upperBound = 999.0;
    if (noiseConfig == nullptr)
        return;

    bool ok = false;
    const double parsedMin = currentMin.toDouble(&ok);

    // Если строка пустая
    if (currentMin.trimmed().isEmpty()) {
        noiseConfig->min = 0.0;
        return;
    }

    // Если строчка состоит из единственного минуса, то не меняем её и тупо ждём
    if (currentMin == "-")
        return;

    // Если вместо числа мусор, то ничего не трогаем и не позволяем трогать
    if (!ok || !std::isfinite(parsedMin)) {
        emit currentMinChanged(Utils::doubleToString(noiseConfig->min, 3));
        return;
    }

    // Если в конце точка, то ждем дальнейших цифр
    if (currentMin.endsWith('.'))
        return;
    
    const double currentMax = noiseConfig->max;
    const double allowedUpperBound = std::min(upperBound, currentMax - 1);

    const double min = std::clamp(parsedMin, lowerBound, allowedUpperBound);  // IDK: Какой минимум у минимального значения?
    if (parsedMin > min) {
        noiseConfig->min = min;
        emit currentMinChanged(Utils::doubleToString(allowedUpperBound, 3));
        return;
    }
    if (parsedMin < min) {
        noiseConfig->min = min;
        emit currentMinChanged(Utils::doubleToString(lowerBound, 3));
        return;
    }

    if (min == noiseConfig->min) {
        return;
    }

    noiseConfig->min = min;
    emit currentMinChanged(Utils::doubleToString(min, 3));
}

QString UniformNoiseBackend::currentMax() const {
    return QString::number(noiseConfigAcess()->max);
}

void UniformNoiseBackend::setCurrentMax(const QString &currentMax)
{
    // if (double max = currentMax.toDouble(); max < std::numeric_limits<float>::max()) {
    //     max = std::numeric_limits<float>::max();
    //     if (max == currentMax.toDouble())
    //         return;
    //     m_config[m_postIndex].noiseConfig = std::make_unique<UniformNoise::UniformNoiseConfig>(currentMin().toDouble(), max);
    //     emit currentMaxChanged(QString::number(max));
    // } else if (max > std::numeric_limits<float>::max() - 1) {
    //     max = std::numeric_limits<float>::max() - 1;
    //     if (max == currentMax.toDouble())
    //         return;
    //     m_config[m_postIndex].noiseConfig = std::make_unique<UniformNoise::UniformNoiseConfig>(currentMin().toDouble(), max);
    //     emit currentMaxChanged(QString::number(max));
    // } else {
    //     if (max == currentMax.toDouble())
    //         return;
    //     m_config[m_postIndex].noiseConfig = std::make_unique<UniformNoise::UniformNoiseConfig>(currentMin().toDouble(), max);
    //     emit currentMaxChanged(currentMax);
    // }
    UniformNoise::UniformNoiseConfig *noiseConfig = noiseConfigAcess();
    constexpr double lowerBound = -999.0;
    constexpr double upperBound = 1000.0;
    const double currentMin = noiseConfig->min;
    const double allowedLowerBound = std::max(lowerBound, currentMin + 1);

    if (noiseConfig == nullptr)
        return;

    bool ok = false;
    const double parsedMax = currentMax.toDouble(&ok);

    // Если строка пустая
    if (currentMax.trimmed().isEmpty()) {
        noiseConfig->max = 0.0;
        return;
    }

    // Если строчка состоит из единственного минуса, то не меняем её и тупо ждём
    if (currentMax == "-")
        return;

    // Если вместо числа мусор, то ничего не трогаем и не позволяем трогать
    if (!ok || !std::isfinite(parsedMax)) {
        emit currentMaxChanged(Utils::doubleToString(noiseConfig->max, 3));
        return;
    }

    // Если в конце точка, то ждем дальнейших цифр
    if (currentMax.endsWith('.'))
        return;

    if (allowedLowerBound > upperBound)
        return;

    const double max = std::clamp(parsedMax, allowedLowerBound, upperBound);  // IDK: Какой максимум у максимума?
    if (parsedMax > max) {
        noiseConfig->max = max;
        emit currentMaxChanged(Utils::doubleToString(upperBound, 3));
        return;
    }
    if (parsedMax < max) {
        noiseConfig->max = max;
        emit currentMaxChanged(Utils::doubleToString(allowedLowerBound, 3));
        return;
    }

    if (max == noiseConfig->max) {
        return;
    }

    noiseConfig->max = max;
    emit currentMaxChanged(Utils::doubleToString(max, 3));
}
