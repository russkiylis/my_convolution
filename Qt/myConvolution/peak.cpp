#include <cmath>
#include "peak.h"

AbstractPeak::PeakConfig::PeakConfig(double const &center, double const &amplitude) :
    center(center),
    amplitude(amplitude)
{
}

AbstractPeak::AbstractPeak(double const &center, double const &amplitude, QObject *parent)
    : QObject{parent},
    _center(center),
    _amplitude(amplitude)
{
}

AbstractPeak::AbstractPeak(PeakConfig const &config, QObject *parent) :
    QObject{parent},
    _center(config.center),
    _amplitude(config.amplitude)
{
}

double AbstractPeak::center() const
{
    return _center;
}

void AbstractPeak::setCenter(const double &center)
{
    _center = center;
}

double AbstractPeak::amplitude() const
{
    return _amplitude;
}

void AbstractPeak::setAmplitude(const double &amplitude)
{
    _amplitude = amplitude;
}

GaussPeak::GaussPeakConfig::GaussPeakConfig(double const &center, double const &amplitude, double const &sigma) :
    PeakConfig(center, amplitude),
    sigma(sigma)
{
}

std::unique_ptr<AbstractPeak::PeakConfig> GaussPeak::GaussPeakConfig::clone() const {
    return std::make_unique<GaussPeakConfig>(*this);
}

std::unique_ptr<AbstractPeak> GaussPeak::GaussPeakConfig::createPeak() const {
    return std::make_unique<GaussPeak>(*this);
}

AbstractPeak::PeakType GaussPeak::GaussPeakConfig::type() const {
    return PeakType::Gauss;
}

GaussPeak::GaussPeak(double const &center, double const &amplitude, double const &sigma, QObject *parent) :
    AbstractPeak(center, amplitude, parent),
    _sigma(sigma)
{
}

GaussPeak::GaussPeak(GaussPeakConfig const &config, QObject *parent) :
    AbstractPeak(config.center, config.amplitude),
    _sigma(config.sigma)
{
}

AbstractPeak::PeakType GaussPeak::type() const
{
    return PeakType::Gauss;
}

double GaussPeak::valueAt(double const &deg) const
{
    if (_sigma == 0) {
        if (deg == _center)
            return _amplitude;
        return 0;
    }

    double const exponent = std::exp(-0.5 * std::pow((deg - _center) / _sigma, 2));
    return _amplitude * exponent;
}

double GaussPeak::sigma() const
{
    return _sigma;
}

void GaussPeak::setSigma(double const &sigma)
{
    _sigma = sigma;
}

TrianglePeak::TrianglePeakConfig::TrianglePeakConfig(
    double const &center,
    double const &amplitude,
    double const &halfWidth
    ) :
    PeakConfig(center, amplitude),
    halfWidth(halfWidth)
{
}

std::unique_ptr<AbstractPeak> TrianglePeak::TrianglePeakConfig::createPeak() const {
    return std::make_unique<TrianglePeak>(*this);
}

AbstractPeak::PeakType TrianglePeak::TrianglePeakConfig::type() const {
    return PeakType::Triangle;
}

std::unique_ptr<AbstractPeak::PeakConfig> TrianglePeak::TrianglePeakConfig::clone() const {
    return std::make_unique<TrianglePeakConfig>(*this);
}

TrianglePeak::TrianglePeak(double const &center, double const &amplitude, double const &halfWidth, QObject *parent) :
    AbstractPeak(center, amplitude, parent),
    _halfWidth(halfWidth)
{
}

TrianglePeak::TrianglePeak(TrianglePeakConfig const &config, QObject *parent) :
AbstractPeak(config.center, config.amplitude, parent),
_halfWidth(config.halfWidth)
{
}

AbstractPeak::PeakType TrianglePeak::type() const {
    return PeakType::Triangle;
}

double TrianglePeak::valueAt(double const &deg) const {
    if (_halfWidth == 0) {
        if (deg == _center)
            return _amplitude;
        return 0;
    }

    double distance = std::abs(_center - deg);
    if (distance > _halfWidth) {
        return 0;
    }
    return _amplitude * (1 - (distance / _halfWidth));
}

double TrianglePeak::halfWidth() const {
    return _halfWidth;
}

void TrianglePeak::setHalfWidth(double const &halfWidth) {
    _halfWidth = halfWidth;
}

RectanglePeak::RectanglePeakConfig::RectanglePeakConfig(
    double const &center,
    double const &amplitude,
    double const &halfWidth
    ) :
    PeakConfig(center, amplitude),
    halfWidth(halfWidth)
{
}

std::unique_ptr<AbstractPeak> RectanglePeak::RectanglePeakConfig::createPeak() const {
    return std::make_unique<RectanglePeak>(*this);
}

AbstractPeak::PeakType RectanglePeak::RectanglePeakConfig::type() const {
    return PeakType::Rectangle;
}

std::unique_ptr<AbstractPeak::PeakConfig> RectanglePeak::RectanglePeakConfig::clone() const {
    return std::make_unique<RectanglePeakConfig>(*this);
}

RectanglePeak::RectanglePeak(double const &center, double const &amplitude, double const &halfWidth, QObject *parent) :
    AbstractPeak(center, amplitude, parent),
    _halfWidth(halfWidth)
{
}

RectanglePeak::RectanglePeak(RectanglePeakConfig const &config, QObject *parent) :
    AbstractPeak(config.center, config.amplitude, parent),
    _halfWidth(config.halfWidth)
{
}

AbstractPeak::PeakType RectanglePeak::type() const {
    return PeakType::Rectangle;
}

double RectanglePeak::valueAt(double const &deg) const {
    if (deg < (_center - _halfWidth) || deg > (_center + _halfWidth))
        return 0;
    return _amplitude;
}

double RectanglePeak::halfWidth() const {
    return _halfWidth;
}

void RectanglePeak::setHalfWidth(double const &halfWidth) {
    _halfWidth = halfWidth;
}
