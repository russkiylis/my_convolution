#pragma once
#include <QObject>

#include "noise.h"
#include "loadgenerator.h"

class AbstractNoiseBackend : public QObject {
    Q_OBJECT

protected:
    std::vector<LoadGenerator::PostConfig> &m_config;    // Ссылка на вектор PostConfig
    int &m_postIndex;

public:
    explicit AbstractNoiseBackend(std::vector<LoadGenerator::PostConfig> &config, int &postIndex, QObject *parent = nullptr);
    ~AbstractNoiseBackend() override = default;

    // virtual std::unique_ptr<AbstractNoise::NoiseConfig> setNoiseConfig() = 0;

    virtual void emits() = 0;
};
