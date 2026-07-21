#include "abstractnoisebackend.h"

AbstractNoiseBackend::AbstractNoiseBackend(std::vector<LoadGenerator::PostConfig> &config, int &postIndex, QObject *parent) :
    m_config(config),
    m_postIndex(postIndex),
    QObject{parent}
{
}
