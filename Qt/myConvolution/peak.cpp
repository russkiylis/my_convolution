#include <cmath>
#include "peak.h"

#include "utils.h"

AbstractPeak::PeakConfig::PeakConfig(GeometryUtils::SphericalCoordDeg const &center, double const &amplitude) :
    center(center),
    amplitude(amplitude)
{
}

AbstractPeak::AbstractPeak(GeometryUtils::SphericalCoordDeg const &center, double const &amplitude)
    :
    m_center(center.toLinearCoord()),
    m_amplitude(amplitude)
{
}

AbstractPeak::AbstractPeak(PeakConfig const &config) :
    m_center(config.center.toLinearCoord()),
    m_amplitude(config.amplitude)
{
}

GeometryUtils::SphericalCoordDeg AbstractPeak::center() const
{
    return m_center.toSphericalCoordDeg();
}

void AbstractPeak::setCenter(const GeometryUtils::SphericalCoordDeg &center)
{
    m_center = center.toLinearCoord();
}

double AbstractPeak::amplitude() const
{
    return m_amplitude;
}

void AbstractPeak::setAmplitude(const double &amplitude)
{
    m_amplitude = amplitude;
}

GaussPeak::GaussPeakConfig::GaussPeakConfig(GeometryUtils::SphericalCoordDeg const &center, double const &amplitude, double const &sigma) :
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

QString GaussPeak::GaussPeakConfig::typeStr() const {
    return "Гауссовский";
}

GaussPeak::GaussPeak(GeometryUtils::SphericalCoordDeg const &center, double const &amplitude, double const &sigma) :
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

double GaussPeak::valueAt(GeometryUtils::LinearCoord3D const &deg) const
{
    if (m_sigma == 0) {
        if (deg == m_center)
            return m_amplitude;
        return 0;
    }

    // double const exponent = std::exp(-0.5 * std::pow((deg - m_center) / m_sigma, 2));
    double const exponent = std::exp(-0.5 * std::pow(GeometryUtils::degBetweenCoord3D(deg, m_center), 2) / pow(m_sigma, 2));
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
    GeometryUtils::SphericalCoordDeg const &center,
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

QString TrianglePeak::TrianglePeakConfig::typeStr() const {
    return "Треугольный";
}

std::unique_ptr<AbstractPeak::PeakConfig> TrianglePeak::TrianglePeakConfig::clone() const {
    return std::make_unique<TrianglePeakConfig>(*this);
}

TrianglePeak::TrianglePeak(GeometryUtils::SphericalCoordDeg const &center, double const &amplitude, double const &halfWidth) :
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

double TrianglePeak::valueAt(GeometryUtils::LinearCoord3D const &deg) const {
    if (m_halfWidth == 0) {
        if (deg == m_center)
            return m_amplitude;
        return 0;
    }

    const double distance = GeometryUtils::degBetweenCoord3D(m_center, deg);
    if (distance > m_halfWidth) {
        return 0;
    }
    return m_amplitude * (1 - distance / m_halfWidth);
}

double TrianglePeak::halfWidth() const {
    return m_halfWidth;
}

void TrianglePeak::setHalfWidth(double const &halfWidth) {
    m_halfWidth = halfWidth;
}

RectanglePeak::RectanglePeakConfig::RectanglePeakConfig(
    GeometryUtils::SphericalCoordDeg const &center,
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

QString RectanglePeak::RectanglePeakConfig::typeStr() const {
    return "Прямоугольный";
}

std::unique_ptr<AbstractPeak::PeakConfig> RectanglePeak::RectanglePeakConfig::clone() const {
    return std::make_unique<RectanglePeakConfig>(*this);
}

RectanglePeak::RectanglePeak(GeometryUtils::SphericalCoordDeg const &center, double const &amplitude, double const &halfWidth) :
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

double RectanglePeak::valueAt(GeometryUtils::LinearCoord3D const &deg) const {
    if (GeometryUtils::degBetweenCoord3D(m_center, deg) > m_halfWidth)
        return 0;
    return m_amplitude;
}

double RectanglePeak::halfWidth() const {
    return m_halfWidth;
}

void RectanglePeak::setHalfWidth(double const &halfWidth) {
    m_halfWidth = halfWidth;
}
