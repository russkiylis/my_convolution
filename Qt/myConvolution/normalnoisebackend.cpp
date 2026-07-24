#include "normalnoisebackend.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdnoreturn.h>

#include "utils.h"

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
    NormalNoise::NormalNoiseConfig *noiseConfig = noiseConfigAcess();
    if (noiseConfig == nullptr)
        return;

    bool ok = false;
    const double parsedMean = currentMean.toDouble(&ok);

    // Если строка пустая
    if (currentMean.trimmed().isEmpty()) {
        noiseConfig->mean = 0.0;
        return;
    }

    // Если строчка состоит из единственного минуса, то не меняем её и тупо ждём
    if (currentMean == "-")
        return;

    // Если вместо числа мусор, то ничего не трогаем и не позволяем трогать
    if (!ok || !std::isfinite(parsedMean)) {
        emit currentMeanChanged(Utils::doubleToString(noiseConfig->mean, 3));
        return;
    }

    // Если в конце точка, то ждем дальнейших цифр
    if (currentMean.endsWith('.'))
        return;

    const double mean = std::clamp(parsedMean, -1000.0, 1000.0);  // IDK: Какой минимум/максимум у среднего?
    if (parsedMean > mean) {
        noiseConfig->mean = mean;
        emit currentMeanChanged("1000");
        return;
    }
    if (parsedMean < mean) {
        noiseConfig->mean = mean;
        emit currentMeanChanged("-1000");
        return;
    }

    if (mean == noiseConfig->mean) {
        return;
    }

    noiseConfig->mean = mean;
    emit currentMeanChanged(Utils::doubleToString(mean, 3));
}

QString NormalNoiseBackend::currentSigma() const {
    return Utils::doubleToString(noiseConfigAcess()->sigma, 3);
}

void NormalNoiseBackend::setCurrentSigma(const QString &currentSigma) {
    NormalNoise::NormalNoiseConfig *noiseConfig = noiseConfigAcess();
    if (noiseConfig == nullptr)
        return;

    bool ok = false;
    const double parsedSigma = currentSigma.toDouble(&ok);

    // Если строка пустая
    if (currentSigma.trimmed().isEmpty()) {
        noiseConfig->sigma = 0.0;
        return;
    }

    // Если строчка состоит из единственного минуса, то не меняем её и тупо ждём
    if (currentSigma == "-")
        return;

    // Если вместо числа мусор, то ничего не трогаем и не позволяем трогать
    if (!ok || !std::isfinite(parsedSigma)) {
        emit currentSigmaChanged(Utils::doubleToString(noiseConfig->sigma, 3));
        return;
    }

    // Если в конце точка, то ждем дальнейших цифр
    if (currentSigma.endsWith('.'))
        return;

    const double sigma = std::clamp(parsedSigma, -1000.0, 1000.0);  // IDK: Какой минимум/максимум у СКО?
    if (parsedSigma > sigma) {
        noiseConfig->sigma = sigma;
        emit currentSigmaChanged("1000");
        return;
    }
    if (parsedSigma < sigma) {
        noiseConfig->sigma = sigma;
        emit currentSigmaChanged("-1000");
        return;
    }

    if (sigma == noiseConfig->sigma) {
        return;
    }

    noiseConfig->sigma = sigma;
    emit currentSigmaChanged(Utils::doubleToString(sigma, 3));
}

QString NormalNoiseBackend::currentMean() const {
    return Utils::doubleToString(noiseConfigAcess()->mean, 3);
}
