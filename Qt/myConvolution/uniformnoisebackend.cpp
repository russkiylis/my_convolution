#include "uniformnoisebackend.h"

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
    return QString::number(noiseConfigAcess()->min);
}

void UniformNoiseBackend::setCurrentMin(const QString &currentMin)
{
    if (double min = currentMin.toDouble(); min < std::numeric_limits<float>::min()) {
        min = std::numeric_limits<float>::min();
        if (min == currentMin.toDouble())
            return;
        m_config[m_postIndex].noiseConfig = std::make_unique<UniformNoise::UniformNoiseConfig>(min, currentMax().toDouble());
        emit currentMinChanged(QString::number(min));
    } else if (min > std::numeric_limits<float>::max() - 1) {
        min = std::numeric_limits<float>::max() - 1;
        if (min == currentMin.toDouble())
            return;
        m_config[m_postIndex].noiseConfig = std::make_unique<UniformNoise::UniformNoiseConfig>(min, currentMax().toDouble());
        emit currentMinChanged(QString::number(min));
    } else {
        if (min == currentMin.toDouble())
            return;
        m_config[m_postIndex].noiseConfig = std::make_unique<UniformNoise::UniformNoiseConfig>(min, currentMax().toDouble());
        emit currentMinChanged(currentMin);
    }
}

QString UniformNoiseBackend::currentMax() const {
    return QString::number(noiseConfigAcess()->max);
}

void UniformNoiseBackend::setCurrentMax(const QString &currentMax)
{
    if (double max = currentMax.toDouble(); max < std::numeric_limits<float>::max()) {
        max = std::numeric_limits<float>::max();
        if (max == currentMax.toDouble())
            return;
        m_config[m_postIndex].noiseConfig = std::make_unique<UniformNoise::UniformNoiseConfig>(currentMin().toDouble(), max);
        emit currentMaxChanged(QString::number(max));
    } else if (max > std::numeric_limits<float>::max() - 1) {
        max = std::numeric_limits<float>::max() - 1;
        if (max == currentMax.toDouble())
            return;
        m_config[m_postIndex].noiseConfig = std::make_unique<UniformNoise::UniformNoiseConfig>(currentMin().toDouble(), max);
        emit currentMaxChanged(QString::number(max));
    } else {
        if (max == currentMax.toDouble())
            return;
        m_config[m_postIndex].noiseConfig = std::make_unique<UniformNoise::UniformNoiseConfig>(currentMin().toDouble(), max);
        emit currentMaxChanged(currentMax);
    }
}
