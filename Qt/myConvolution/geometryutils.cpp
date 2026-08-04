#include "geometryutils.h"
#include <QVector3D>
#include <cmath>
#include <stdexcept>

double GeometryUtils::normalizeAngle(const double angle, const double period)
{
    double normalized = std::fmod(angle, period);
    if (normalized < 0)
        normalized += period;
    return normalized;
}

GeometryUtils::LinearCoord3D::LinearCoord3D(const double x, const double y, const double z): m_x(x), m_y(y), m_z(z)
{
}

GeometryUtils::SphericalCoordDeg GeometryUtils::LinearCoord3D::toSphericalCoordDeg() const {
    SphericalCoordRad resultRad;
    const LinearCoord3D normalized = normalize();

    resultRad.setElevation(std::asin(normalized.z()));
    resultRad.setAzimuth(std::atan2(normalized.y(), normalized.x()));

    return resultRad.toSphericalCoordDeg();
}

GeometryUtils::SphericalCoordRad GeometryUtils::LinearCoord3D::toSphericalCoordRad() const {
    SphericalCoordRad resultRad;
    const LinearCoord3D normalized = normalize();

    resultRad.setElevation(std::asin(normalized.z()));
    resultRad.setAzimuth(std::atan2(normalized.y(), normalized.x()));

    return resultRad;
}

GeometryUtils::LinearCoord3D GeometryUtils::LinearCoord3D::normalize() const {
    LinearCoord3D result;

    const double len = length();

    if (len == 0.0)
        throw std::domain_error("Невозможно нормировать нулевой вектор.");

    result.m_x = m_x / len;
    result.m_y = m_y / len;
    result.m_z = m_z / len;

    return result;
}

double GeometryUtils::LinearCoord3D::length() const {
    return std::sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
}

double GeometryUtils::LinearCoord3D::x() const
{
    return m_x;
}

void GeometryUtils::LinearCoord3D::setX(const double x)
{
    m_x = x;
}

double GeometryUtils::LinearCoord3D::y() const
{
    return m_y;
}

void GeometryUtils::LinearCoord3D::setY(const double y)
{
    m_y = y;
}

double GeometryUtils::LinearCoord3D::z() const
{
    return m_z;
}

void GeometryUtils::LinearCoord3D::setZ(const double z)
{
    m_z = z;
}

GeometryUtils::SphericalCoordDeg::SphericalCoordDeg(const double azimuth, const double elevation) {
    m_azimuth = normalizeAngle(azimuth, 360.0);
    m_elevation = std::clamp(elevation, -90.0, 90.0);
}

GeometryUtils::LinearCoord3D GeometryUtils::SphericalCoordDeg::toLinearCoord(double length) const {
    LinearCoord3D result;
    const SphericalCoordRad rad = toSphericalCoordRad();

    result.setX(std::cos(rad.azimuth()) * std::cos(rad.elevation()) * length);
    result.setY(std::sin(rad.azimuth()) * std::cos(rad.elevation()) * length);
    result.setZ(std::sin(rad.elevation()) * length);

    return result;
}

GeometryUtils::SphericalCoordRad GeometryUtils::SphericalCoordDeg::toSphericalCoordRad() const {
    SphericalCoordRad result;
    result.setAzimuth(qDegreesToRadians(m_azimuth));
    result.setElevation(qDegreesToRadians(m_elevation));
    return result;
}

double GeometryUtils::SphericalCoordDeg::azimuth() const
{
    return m_azimuth;
}

void GeometryUtils::SphericalCoordDeg::setAzimuth(const double azimuth)
{
    m_azimuth = normalizeAngle(azimuth, 360.0);
}

double GeometryUtils::SphericalCoordDeg::elevation() const
{
    return m_elevation;
}

void GeometryUtils::SphericalCoordDeg::setElevation(const double elevation)
{
    m_elevation = std::clamp(elevation, -90.0, 90.0);
}

GeometryUtils::SphericalCoordRad::SphericalCoordRad(const double azimuth, const double elevation) {
    m_azimuth = normalizeAngle(azimuth, 2.0 * M_PI);
    m_elevation = std::clamp(elevation, -M_PI_2, M_PI_2);
}

GeometryUtils::LinearCoord3D GeometryUtils::SphericalCoordRad::toLinearCoord(const double length) const {
    LinearCoord3D result;

    result.setX(std::cos(m_azimuth) * std::cos(m_elevation) * length);
    result.setY(std::sin(m_azimuth) * std::cos(m_elevation) * length);
    result.setZ(std::sin(m_elevation) * length);

    return result;
}

GeometryUtils::SphericalCoordDeg GeometryUtils::SphericalCoordRad::toSphericalCoordDeg() const {
    SphericalCoordDeg result;
    result.setAzimuth(qRadiansToDegrees(m_azimuth));
    result.setElevation(qRadiansToDegrees(m_elevation));
    return result;
}

double GeometryUtils::SphericalCoordRad::azimuth() const
{
    return m_azimuth;
}

void GeometryUtils::SphericalCoordRad::setAzimuth(const double azimuth)
{
    m_azimuth = normalizeAngle(azimuth, 2.0 * M_PI);
}

double GeometryUtils::SphericalCoordRad::elevation() const
{
    return m_elevation;
}

void GeometryUtils::SphericalCoordRad::setElevation(const double elevation)
{
    m_elevation = std::clamp(elevation, -M_PI_2, M_PI_2);
}
