#pragma once

#include <QObject>
#include <vector>

#include "noise.h"
#include "peak.h"

class LoadGenerator : public QObject
{
    Q_OBJECT
public:
    explicit LoadGenerator(AbstractNoise::NoiseConfig &noiseConfig, QObject *parent = nullptr);

};
