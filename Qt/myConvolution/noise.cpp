#include "noise.h"
#include <random>

AbstractNoise::NoiseConfig::NoiseConfig(unsigned int const &seed) :
    seed(seed)
{
}

AbstractNoise::NoiseConfig::NoiseConfig() :
    seed(std::random_device{}())
{
}

AbstractNoise::AbstractNoise(unsigned int const &seed, QObject *parent)
    : QObject{parent},
    _seed(seed),
    _rng(seed)
{}

AbstractNoise::AbstractNoise(NoiseConfig const &config, QObject *parent)  :
    QObject{parent},
    _seed(config.seed),
    _rng(config.seed)
{
}

AbstractNoise::AbstractNoise(QObject *parent)
    : QObject{parent},
      _seed(std::random_device{}()),
      _rng(_seed)
{
}

unsigned int AbstractNoise::seed() const {
    return _seed;
}

void AbstractNoise::setSeed(unsigned int const &seed) {
    _seed = seed;
}

std::mt19937 &AbstractNoise::rng()
{
    return _rng;
}

void AbstractNoise::setRng(const std::mt19937 &rng) {
    _rng = rng;
}

NormalNoise::NormalNoiseConfig::NormalNoiseConfig(double const &mean, double const &sigma, unsigned int const &seed) :
    NoiseConfig(seed),
    mean(mean),
    sigma(sigma)
{
}

NormalNoise::NormalNoiseConfig::NormalNoiseConfig(double const &mean, double const &sigma) :
    mean(mean),
    sigma(sigma)
{
}

std::unique_ptr<AbstractNoise::NoiseConfig> NormalNoise::NormalNoiseConfig::clone() const {
    return std::make_unique<NormalNoiseConfig>(*this);
}

std::unique_ptr<AbstractNoise> NormalNoise::NormalNoiseConfig::createNoise() const {
    return std::make_unique<NormalNoise>(*this);
}

AbstractNoise::NoiseType NormalNoise::NormalNoiseConfig::noiseType() const {
    return NoiseType::Normal;
}

NormalNoise::NormalNoise(double const &mean, double const &sigma, unsigned int const &seed, QObject *parent) :
    AbstractNoise(seed, parent),
    _mean(mean),
    _sigma(sigma)
{
}

NormalNoise::NormalNoise(double const &mean, double const &sigma, QObject *parent) :
    AbstractNoise(parent),
    _mean(mean),
    _sigma(sigma)
{
}

NormalNoise::NormalNoise(NormalNoiseConfig const &config, QObject *parent) :
    AbstractNoise(config.seed, parent),
    _mean(config.mean),
    _sigma(config.sigma)
{
}

AbstractNoise::NoiseType NormalNoise::type() const {
    return NoiseType::Normal;
}

double NormalNoise::next()
{
    // Чтобы не писать долгую строчку
    using genParam = std::normal_distribution<double>::param_type;

    return _distribution(_rng, genParam(_mean, _sigma));
}

double NormalNoise::mean() const {
    return _mean;
}

void NormalNoise::setMean(double const &mean) {
    _mean = mean;
}

double NormalNoise::sigma() const {
    return _sigma;
}

void NormalNoise::setSigma(double const &sigma) {
    _sigma = sigma;
}

UniformNoise::UniformNoiseConfig::UniformNoiseConfig(double const &min, double const &max, unsigned int const &seed)
    : NoiseConfig(seed),
    min(min),
    max(max)
{
}

UniformNoise::UniformNoiseConfig::UniformNoiseConfig(double const &min, double const &max) :
    min(min),
    max(max)
{
}

AbstractNoise::NoiseType UniformNoise::UniformNoiseConfig::noiseType() const {
    return NoiseType::Uniform;
}

std::unique_ptr<AbstractNoise> UniformNoise::UniformNoiseConfig::createNoise() const {
    return std::make_unique<UniformNoise>(*this);
}

std::unique_ptr<AbstractNoise::NoiseConfig> UniformNoise::UniformNoiseConfig::clone() const {
    return std::make_unique<UniformNoiseConfig>(*this);
}

UniformNoise::UniformNoise(double const &min, double const &max, unsigned int const &seed, QObject *parent) :
    AbstractNoise(seed, parent),
    _min(min),
    _max(max)
{
}

UniformNoise::UniformNoise(double const &min, double const &max, QObject *parent) :
    AbstractNoise(parent),
    _min(min),
    _max(max)
{
}

UniformNoise::UniformNoise(UniformNoiseConfig const &config, QObject *parent) :
    AbstractNoise(config.seed, parent),
    _min(config.min),
    _max(config.max)
{
}

AbstractNoise::NoiseType UniformNoise::type() const {
    return NoiseType::Uniform;
}

double UniformNoise::next()
{
    // Чтобы не писать долгую строчку
    using genParam = std::uniform_real_distribution<double>::param_type;

    return _distribution(_rng, genParam(_min, _max));
}

double UniformNoise::min() const {
    return _min;
}

void UniformNoise::setMin(double const &min) {
    _min = min;
}

double UniformNoise::max() const {
    return _max;
}

void UniformNoise::setMax(double const &max) {
    _max = max;
}

