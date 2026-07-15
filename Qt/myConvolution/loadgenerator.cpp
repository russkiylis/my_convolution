#include "loadgenerator.h"

LoadGenerator::PostConfig::PostConfig(const PostConfig &other):
    postName(other.postName),
    latitude(other.latitude),
    longitude(other.longitude),
    frequency(other.frequency),
    level(other.level),
    levelSigma(other.levelSigma),
    min_angle(other.min_angle),
    max_angle(other.max_angle),
    step(other.step),
    minPeriod(other.minPeriod),
    maxPeriod(other.maxPeriod),
    noiseConfig(other.noiseConfig->clone())
{
    // Проходим по вектору умных указателей и копируем их
    for (const auto & peakConfig : other.peakConfigs) {
        peakConfigs.push_back(peakConfig->clone());
    }
}

LoadGenerator::PostConfig & LoadGenerator::PostConfig::operator=(const PostConfig &other) {
    if (this == &other)
        return *this;
    postName = other.postName;
    latitude = other.latitude;
    longitude = other.longitude;
    frequency = other.frequency;
    level = other.level;
    levelSigma = other.levelSigma;
    min_angle = other.min_angle;
    max_angle = other.max_angle;
    step = other.step;
    minPeriod = other.minPeriod;
    maxPeriod = other.maxPeriod;
    noiseConfig = other.noiseConfig->clone();
    // Проходим по вектору умных указателей и копируем их
    for (const auto & peakConfig : other.peakConfigs) {
        peakConfigs.push_back(peakConfig->clone());
    }
    return *this;
}

LoadGenerator::Post::Post(PostConfig const &config) :
    _config(config)
{
    _noise = _config.noiseConfig->createNoise(); // В зависимости от типа конфига нам выдадут разные шумы
    for (const auto & peakConfig : _config.peakConfigs) {
        _peaks.push_back(peakConfig->createPeak()); // В зависимости от типа конфига нам выдадут разные пики
    }
}

LoadGenerator::LoadGenerator(std::vector<PostConfig> const &postConfigs, QObject *parent) :
    QObject(parent),
    _postConfigs(postConfigs)
{

}

std::vector<LoadGenerator::PostConfig> LoadGenerator::postConfigs() const
{
    return _postConfigs;
}

void LoadGenerator::setPostConfigs(const std::vector<PostConfig> &postConfigs)
{
    _postConfigs = postConfigs;
}

void LoadGenerator::load() {
    for (const auto & postConfig : _postConfigs) {
        _posts.emplace_back(postConfig);
    }
}
