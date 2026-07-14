#include "noise.h"
#include <random>

AbstractNoise::AbstractNoise(unsigned int const &seed, QObject *parent)
    : QObject{parent},
    _seed(seed),
    _rng(seed)
{}

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

std::mt19937 AbstractNoise::rng() const {
    return _rng;
}

void AbstractNoise::setRng(const std::mt19937 &rng) {
    _rng = rng;
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

AbstractNoise::NoiseType NormalNoise::type() const {
    return NoiseType::Normal;
}

double NormalNoise::next() const {
    // TODO: Реализовать генерацию нормального шума
    return 0;
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

AbstractNoise::NoiseType UniformNoise::type() const {
    return NoiseType::Uniform;
}

double UniformNoise::next() const {
    // TODO: Реализовать генерацию равномерного шума
    return 0;
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

