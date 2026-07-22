#include <cmath>
#include "peak.h"


AbstractPeak::PeakConfig::PeakConfig(double const &center, double const &amplitude) :
    center(center),
    amplitude(amplitude)
{
}

AbstractPeak::AbstractPeak(double const &center, double const &amplitude)
    :
    m_center(center),
    m_amplitude(amplitude)
{
}

AbstractPeak::AbstractPeak(PeakConfig const &config) :
    m_center(config.center),
    m_amplitude(config.amplitude)
{
}

double AbstractPeak::center() const
{
    return m_center;
}

void AbstractPeak::setCenter(const double &center)
{
    m_center = center;
}

double AbstractPeak::amplitude() const
{
    return m_amplitude;
}

void AbstractPeak::setAmplitude(const double &amplitude)
{
    m_amplitude = amplitude;
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

GaussPeak::GaussPeak(double const &center, double const &amplitude, double const &sigma) :
    AbstractPeak(center, amplitude),
    m_sigma(sigma)
{
}

GaussPeak::GaussPeak(GaussPeakConfig const &config) :
    AbstractPeak(config.center, config.amplitude),
    m_sigma(config.sigma)
{
}

AbstractPeak::PeakType GaussPeak::type() const
{
    return PeakType::Gauss;
}

double GaussPeak::valueAt(double const &deg) const
{
    if (m_sigma == 0) {
        if (deg == m_center)
            return m_amplitude;
        return 0;
    }

    double const exponent = std::exp(-0.5 * std::pow((deg - m_center) / m_sigma, 2));
    return m_amplitude * exponent;
}

double GaussPeak::sigma() const
{
    return m_sigma;
}

void GaussPeak::setSigma(double const &sigma)
{
    m_sigma = sigma;
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

TrianglePeak::TrianglePeak(double const &center, double const &amplitude, double const &halfWidth) :
    AbstractPeak(center, amplitude),
    m_halfWidth(halfWidth)
{
}

TrianglePeak::TrianglePeak(TrianglePeakConfig const &config) :
AbstractPeak(config.center, config.amplitude),
m_halfWidth(config.halfWidth)
{
}

AbstractPeak::PeakType TrianglePeak::type() const {
    return PeakType::Triangle;
}

double TrianglePeak::valueAt(double const &deg) const {
    if (m_halfWidth == 0) {
        if (deg == m_center)
            return m_amplitude;
        return 0;
    }

    double distance = std::abs(m_center - deg);
    if (distance > m_halfWidth) {
        return 0;
    }
    return m_amplitude * (1 - (distance / m_halfWidth));
}

double TrianglePeak::halfWidth() const {
    return m_halfWidth;
}

void TrianglePeak::setHalfWidth(double const &halfWidth) {
    m_halfWidth = halfWidth;
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

RectanglePeak::RectanglePeak(double const &center, double const &amplitude, double const &halfWidth) :
    AbstractPeak(center, amplitude),
    m_halfWidth(halfWidth)
{
}

RectanglePeak::RectanglePeak(RectanglePeakConfig const &config) :
    AbstractPeak(config.center, config.amplitude),
    m_halfWidth(config.halfWidth)
{
}

AbstractPeak::PeakType RectanglePeak::type() const {
    return PeakType::Rectangle;
}

double RectanglePeak::valueAt(double const &deg) const {
    if (deg < (m_center - m_halfWidth) || deg > (m_center + m_halfWidth))
        return 0;
    return m_amplitude;
}

double RectanglePeak::halfWidth() const {
    return m_halfWidth;
}

void RectanglePeak::setHalfWidth(double const &halfWidth) {
    m_halfWidth = halfWidth;
}
