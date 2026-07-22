#include "uniformnoisebackend.h"

UniformNoise::UniformNoiseConfig * UniformNoiseBackend::noiseConfigAcess() const {
    return dynamic_cast<UniformNoise::UniformNoiseConfig *>(m_config[m_postIndex].noiseConfig.get());
}

UniformNoiseBackend::UniformNoiseBackend(std::vector<LoadGenerator::PostConfig> &config, int &postIndex, QObject *parent):
    AbstractNoiseBackend(config, postIndex, parent)
{
    qDebug() << "UniformNoiseBackend прогружен.";
}

void UniformNoiseBackend::emits() {
    emit currentMinChanged(currentMin());
    emit currentMaxChanged(currentMax());
}

QString UniformNoiseBackend::currentMin() const {
    return QString::number(noiseConfigAcess()->min);
}

void UniformNoiseBackend::setCurrentMin(const QString &currentMin)
{
    m_config[m_postIndex].noiseConfig = std::make_unique<UniformNoise::UniformNoiseConfig>(currentMin.toDouble(), currentMax().toDouble());
    emit currentMinChanged(this->currentMin());
}

QString UniformNoiseBackend::currentMax() const {
    return QString::number(noiseConfigAcess()->max);
}

void UniformNoiseBackend::setCurrentMax(const QString &currentMax)
{
    m_config[m_postIndex].noiseConfig = std::make_unique<UniformNoise::UniformNoiseConfig>(currentMin().toDouble(), currentMax.toDouble());
    emit currentMaxChanged(this->currentMax());
}
