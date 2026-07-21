#include "normalnoisebackend.h"

NormalNoiseBackend::NormalNoiseBackend(std::vector<LoadGenerator::PostConfig> &config, int &postIndex, QObject *parent)
: AbstractNoiseBackend(config, postIndex, parent)
{
    qDebug() << "NormalNoiseBackend прогружен.";
}

void NormalNoiseBackend::emits() {
    emit currentMeanChanged(currentMean());
    emit currentSigmaChanged(currentSigma());
}

NormalNoise::NormalNoiseConfig* NormalNoiseBackend::noiseConfigAcess() const
{
    return dynamic_cast<NormalNoise::NormalNoiseConfig*>(m_config[m_postIndex].noiseConfig.get());
}

void NormalNoiseBackend::setCurrentMean(const QString &currentMean) {
    m_config[m_postIndex].noiseConfig = std::make_unique<NormalNoise::NormalNoiseConfig>(currentMean.toDouble(), currentSigma().toDouble());
    emit currentMeanChanged(currentMean);
}

QString NormalNoiseBackend::currentSigma() const {
    return QString::number(noiseConfigAcess()->sigma);
}

void NormalNoiseBackend::setCurrentSigma(const QString &currentSigma) {
    m_config[m_postIndex].noiseConfig = std::make_unique<NormalNoise::NormalNoiseConfig>(currentMean().toDouble(), currentSigma.toDouble());
    emit currentSigmaChanged(currentSigma);
}

QString NormalNoiseBackend::currentMean() const {
    return QString::number(noiseConfigAcess()->mean);
}
