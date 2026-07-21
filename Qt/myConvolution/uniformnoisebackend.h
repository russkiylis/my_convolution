#pragma once
#include "abstractnoisebackend.h"

class UniformNoiseBackend final: public AbstractNoiseBackend {
    Q_OBJECT

    Q_PROPERTY(QString currentMin READ currentMin WRITE setCurrentMin NOTIFY currentMinChanged);
    Q_PROPERTY(QString currentMax READ currentMax WRITE setCurrentMax NOTIFY currentMaxChanged);

    UniformNoise::UniformNoiseConfig* noiseConfigAcess() const;
public:
    UniformNoiseBackend(std::vector<LoadGenerator::PostConfig> &config, int &postIndex, QObject *parent = nullptr);

    void emits() override;

    // Получить текущее среднее значение
    QString currentMin() const;

    // Установить текущее среднее значение
    void setCurrentMin(const QString &currentMin) const;

    // Получить текущее СКО
    QString currentMax() const;

    // Установить текущее СКО
    void setCurrentMax(const QString &currentMax) const;

signals:
    void currentMinChanged(QString newMin);
    void currentMaxChanged(QString newMax);
};
