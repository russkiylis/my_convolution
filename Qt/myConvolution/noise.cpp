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

AbstractNoise::AbstractNoise(unsigned int const &seed)
    :
    m_seed(seed),
    m_rng(seed)
{}

AbstractNoise::AbstractNoise(NoiseConfig const &config)  :
    m_seed(config.seed),
    m_rng(config.seed)
{
}

AbstractNoise::AbstractNoise()
    :
      m_seed(std::random_device{}()),
      m_rng(m_seed)
{
}

unsigned int AbstractNoise::seed() const {
    return m_seed;
}

void AbstractNoise::setSeed(unsigned int const &seed) {
    m_seed = seed;
}

std::mt19937 &AbstractNoise::rng()
{
    return m_rng;
}

void AbstractNoise::setRng(const std::mt19937 &rng) {
    m_rng = rng;
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

std::unique_ptr<AbstractNoise::NoiseConfig> NormalNoise::NormalNoiseConfig::clone() {
    return std::make_unique<NormalNoiseConfig>(*this);
}

std::unique_ptr<AbstractNoise> NormalNoise::NormalNoiseConfig::createNoise() const {
    return std::make_unique<NormalNoise>(*this);
}

AbstractNoise::NoiseType NormalNoise::NormalNoiseConfig::noiseType() const {
    return NoiseType::Normal;
}

NormalNoise::NormalNoise(double const &mean, double const &sigma, unsigned int const &seed) :
    AbstractNoise(seed),
    m_mean(mean),
    m_sigma(sigma)
{
}

NormalNoise::NormalNoise(double const &mean, double const &sigma) :
    AbstractNoise(),
    m_mean(mean),
    m_sigma(sigma)
{
}

NormalNoise::NormalNoise(NormalNoiseConfig const &config) :
    AbstractNoise(config.seed),
    m_mean(config.mean),
    m_sigma(config.sigma)
{
}

AbstractNoise::NoiseType NormalNoise::type() const {
    return NoiseType::Normal;
}

double NormalNoise::next()
{
    // Чтобы не писать долгую строчку
    using genParam = std::normal_distribution<double>::param_type;

    return m_distribution(m_rng, genParam(m_mean, m_sigma));
}

double NormalNoise::mean() const {
    return m_mean;
}

void NormalNoise::setMean(double const &mean) {
    m_mean = mean;
}

double NormalNoise::sigma() const {
    return m_sigma;
}

void NormalNoise::setSigma(double const &sigma) {
    m_sigma = sigma;
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

std::unique_ptr<AbstractNoise::NoiseConfig> UniformNoise::UniformNoiseConfig::clone() {
    return std::make_unique<UniformNoiseConfig>(*this);
}

UniformNoise::UniformNoise(double const &min, double const &max, unsigned int const &seed) :
    AbstractNoise(seed),
    m_min(min),
    m_max(max)
{
}

UniformNoise::UniformNoise(double const &min, double const &max) :
    AbstractNoise(),
    m_min(min),
    m_max(max)
{
}

UniformNoise::UniformNoise(UniformNoiseConfig const &config) :
    AbstractNoise(config.seed),
    m_min(config.min),
    m_max(config.max)
{
}

AbstractNoise::NoiseType UniformNoise::type() const {
    return NoiseType::Uniform;
}

double UniformNoise::next()
{
    // Чтобы не писать долгую строчку
    using genParam = std::uniform_real_distribution<double>::param_type;

    return m_distribution(m_rng, genParam(m_min, m_max));
}

double UniformNoise::min() const {
    return m_min;
}

void UniformNoise::setMin(double const &min) {
    m_min = min;
}

double UniformNoise::max() const {
    return m_max;
}

void UniformNoise::setMax(double const &max) {
    m_max = max;
}
