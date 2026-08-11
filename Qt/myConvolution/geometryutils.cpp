#include "geometryutils.h"
#include <cmath>
#include <stdexcept>

#include "utils.h"

double GeometryUtils::normalizeAngle(const double angle, const double period)
{
    double normalized = std::fmod(angle, period);
    if (normalized < 0)
        normalized += period;
    return normalized;
}

GeometryUtils::LinearCoord3D::LinearCoord3D(const double x, const double y, const double z): m_x(x), m_y(y), m_z(z)
{
    setLength();
}

GeometryUtils::SphericalCoordDeg GeometryUtils::LinearCoord3D::toSphericalCoordDeg() const {
    return toSphericalCoordRad().toSphericalCoordDeg();
}

GeometryUtils::SphericalCoordRad GeometryUtils::LinearCoord3D::toSphericalCoordRad() const {
    const double horizontal = std::hypot(m_x, m_y);

    if (horizontal == 0.0 && m_z == 0.0)
        throw std::domain_error("Нулевой вектор не имеет направления.");

    const double elevation = std::atan2(m_z, horizontal);
    const double azimuth =
        horizontal == 0.0 ? 0.0 : std::atan2(m_y, m_x);

    SphericalCoordRad resultRad;

    resultRad.setElevation(elevation);
    resultRad.setAzimuth(azimuth);

    return resultRad;
}

GeometryUtils::LinearCoord3D GeometryUtils::LinearCoord3D::normalize() const {

    if (m_length == 0.0)
        throw std::domain_error("Невозможно нормировать нулевой вектор.");

    return LinearCoord3D(
        m_x / m_length,
        m_y / m_length,
        m_z / m_length
    );
}

double GeometryUtils::LinearCoord3D::length() const {
    return m_length;
}

void GeometryUtils::LinearCoord3D::setLength() {
    m_length = std::hypot(m_x, m_y, m_z);
}

double GeometryUtils::LinearCoord3D::x() const
{
    return m_x;
}

void GeometryUtils::LinearCoord3D::setX(const double x)
{
    m_x = x;
    setLength();
}

double GeometryUtils::LinearCoord3D::y() const
{
    return m_y;
}

void GeometryUtils::LinearCoord3D::setY(const double y)
{
    m_y = y;
    setLength();
}

double GeometryUtils::LinearCoord3D::z() const
{
    return m_z;
}

void GeometryUtils::LinearCoord3D::setZ(const double z)
{
    m_z = z;
    setLength();
}

bool GeometryUtils::LinearCoord3D::operator==(const LinearCoord3D &other) const noexcept{
    return m_x == other.m_x
        && m_y == other.m_y
        && m_z == other.m_z;
}

GeometryUtils::SphericalCoordDeg::SphericalCoordDeg(const double azimuth, const double elevation) {
    m_azimuth = normalizeAngle(azimuth, 360.0);
    m_elevation = std::clamp(elevation, -90.0, 90.0);
}

GeometryUtils::LinearCoord3D GeometryUtils::SphericalCoordDeg::toLinearCoord(const double length) const {
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
    constexpr double doublePi = 2.0 * M_PI;
    m_azimuth = normalizeAngle(azimuth, doublePi);
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
    constexpr double doublePi = 2.0 * M_PI;
    m_azimuth = normalizeAngle(azimuth, doublePi);
}

double GeometryUtils::SphericalCoordRad::elevation() const
{
    return m_elevation;
}

void GeometryUtils::SphericalCoordRad::setElevation(const double elevation)
{
    m_elevation = std::clamp(elevation, -M_PI_2, M_PI_2);
}

double GeometryUtils::scalarProduct(LinearCoord3D const &a, LinearCoord3D const &b) {
    return a.x() * b.x() + a.y() * b.y() + a.z() * b.z();
}

GeometryUtils::LinearCoord3D GeometryUtils::vectorProduct(LinearCoord3D const &a, LinearCoord3D const &b) {
    LinearCoord3D result;
    result.setX(a.y() * b.z() - a.z() * b.y());
    result.setY(a.z() * b.x() - a.x() * b.z());
    result.setZ(a.x() * b.y() - a.y() * b.x());
    return result;
}

double GeometryUtils::degBetweenCoord3D(LinearCoord3D a, LinearCoord3D b) {
    if (a.length() != 1.0)
        a = a.normalize();

    if (b.length() != 1.0)
        b = b.normalize();

    const double cosine = std::clamp(scalarProduct(a, b), -1.0, 1.0);
    return qRadiansToDegrees(std::acos(cosine));
}

double GeometryUtils::radBetweenCoord3D(LinearCoord3D a, LinearCoord3D b) {
    if (a.length() != 1.0)
        a = a.normalize();

    if (b.length() != 1.0)
        b = b.normalize();

    const double cosine = std::clamp(scalarProduct(a, b), -1.0, 1.0);
    return std::acos(cosine);
}

double GeometryUtils::degBetweenSphericalCoords(SphericalCoordDeg const &a, SphericalCoordDeg const &b) {
    const LinearCoord3D aLinear = a.toLinearCoord();
    const LinearCoord3D bLinear = b.toLinearCoord();

    return degBetweenCoord3D(aLinear, bLinear);
}

double GeometryUtils::radBetweenSphericalCoords(SphericalCoordRad const &a, SphericalCoordRad const &b) {
    const LinearCoord3D aLinear = a.toLinearCoord();
    const LinearCoord3D bLinear = b.toLinearCoord();

    return radBetweenCoord3D(aLinear, bLinear);
}
