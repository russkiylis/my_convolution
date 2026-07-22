#pragma once

#include "abstractnoisebackend.h"

class NormalNoiseBackend : public AbstractNoiseBackend {
    Q_OBJECT

    Q_PROPERTY(QString currentMean READ currentMean WRITE setCurrentMean NOTIFY currentMeanChanged);
    Q_PROPERTY(QString currentSigma READ currentSigma WRITE setCurrentSigma NOTIFY currentSigmaChanged);

    NormalNoise::NormalNoiseConfig* noiseConfigAcess() const;
public:
    NormalNoiseBackend(std::vector<LoadGenerator::PostConfig> &config, int &postIndex, QObject *parent = nullptr);

    void qmlUpdate() override;

    // Получить текущее среднее значение
    QString currentMean() const;

    // Установить текущее среднее значение
    void setCurrentMean(const QString &currentMean);

    // Получить текущее СКО
    QString currentSigma() const;

    // Установить текущее СКО
    void setCurrentSigma(const QString &currentSigma);

signals:
    void currentMeanChanged(QString newMean);
    void currentSigmaChanged(QString newSigma);
};
