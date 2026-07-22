#include "normalnoisebackend.h"

#include <algorithm>
#include <cmath>
#include <limits>

NormalNoiseBackend::NormalNoiseBackend(std::vector<LoadGenerator::PostConfig> &config, int &postIndex, QObject *parent)
: AbstractNoiseBackend(config, postIndex, parent)
{
    qDebug() << "NormalNoiseBackend прогружен.";
}

void NormalNoiseBackend::qmlUpdate() {
    emit currentMeanChanged(currentMean());
    emit currentSigmaChanged(currentSigma());
}

NormalNoise::NormalNoiseConfig* NormalNoiseBackend::noiseConfigAcess() const
{
    return dynamic_cast<NormalNoise::NormalNoiseConfig*>(m_config[m_postIndex].noiseConfig.get());
}

void NormalNoiseBackend::setCurrentMean(const QString &currentMean) {
    bool ok = false;
    const double mean = currentMean.toDouble(&ok);

    // IDK: Какие могут быть средние?
    if (!ok || !std::isfinite(mean))
        return;

    NormalNoise::NormalNoiseConfig *noiseConfig = noiseConfigAcess();
    if (noiseConfig == nullptr || mean == noiseConfig->mean)
        return;

    noiseConfig->mean = mean;
    emit currentMeanChanged(QString::number(mean));
}

QString NormalNoiseBackend::currentSigma() const {
    return QString::number(noiseConfigAcess()->sigma);
}

void NormalNoiseBackend::setCurrentSigma(const QString &currentSigma) {
    bool ok = false;
    const double parsedSigma = currentSigma.toDouble(&ok);

    if (!ok || !std::isfinite(parsedSigma))
        return;

    // IDK: А каков разброс?
    const double sigma = std::clamp(
        parsedSigma,
        static_cast<double>(std::numeric_limits<float>::min()),
        static_cast<double>(std::numeric_limits<float>::max())
    );

    NormalNoise::NormalNoiseConfig *noiseConfig = noiseConfigAcess();
    if (noiseConfig == nullptr || sigma == noiseConfig->sigma)
        return;

    noiseConfig->sigma = sigma;
    emit currentSigmaChanged(QString::number(sigma));
}

QString NormalNoiseBackend::currentMean() const {
    return QString::number(noiseConfigAcess()->mean);
}
