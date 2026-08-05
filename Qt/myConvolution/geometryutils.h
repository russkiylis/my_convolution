#pragma once
#include <cmath>
#include <qmath.h>

class GeometryUtils
{
public:
    struct SphericalCoordDeg;
    struct SphericalCoordRad;

    static double normalizeAngle(double angle, double period);

    // Структура 3D-координаты
    struct LinearCoord3D {
        explicit LinearCoord3D(double x = 0, double y = 0, double z = 0);

        // Перевод из линейной координаты в сферическую
        [[nodiscard]] SphericalCoordDeg toSphericalCoordDeg() const;

        // Перевод из линейной координаты в сферическую
        [[nodiscard]] SphericalCoordRad toSphericalCoordRad() const;

        // Нормирование вектора
        [[nodiscard]] LinearCoord3D normalize() const;

        [[nodiscard]] double length() const;

        [[nodiscard]] double x() const;
        void setX(double x);
        [[nodiscard]] double y() const;
        void setY(double y);
        [[nodiscard]] double z() const;
        void setZ(double z);

        bool operator==(const LinearCoord3D & other) const noexcept;

    private:
        double m_x;
        double m_y;
        double m_z;

        double m_length{};

        // Длина вектора
        void setLength();
    };

    struct SphericalCoordDeg
    {
        explicit SphericalCoordDeg(double azimuth = 180.0, double elevation = 0);

        [[nodiscard]] LinearCoord3D toLinearCoord(double length = 1) const;
        [[nodiscard]] SphericalCoordRad toSphericalCoordRad() const;

        [[nodiscard]] double azimuth() const;
        void setAzimuth(double azimuth);
        [[nodiscard]] double elevation() const;
        void setElevation(double elevation);

    private:
        double m_azimuth;
        double m_elevation;
    };

    struct SphericalCoordRad
    {
        explicit SphericalCoordRad(double azimuth = M_PI, double elevation = 0);

        [[nodiscard]] LinearCoord3D toLinearCoord(double length = 1) const;
        [[nodiscard]] SphericalCoordDeg toSphericalCoordDeg() const;

        [[nodiscard]] double azimuth() const;
        void setAzimuth(double azimuth);
        [[nodiscard]] double elevation() const;
        void setElevation(double elevation);

    private:
        double m_azimuth;
        double m_elevation;
    };

    // Скалярное произведение
    static double scalarProduct(LinearCoord3D const & a, LinearCoord3D const & b);

    // Векторное произведение
    static LinearCoord3D vectorProduct(LinearCoord3D const & a, LinearCoord3D const & b);

    // Угол между векторами (градусы)
    static double degBetweenCoord3D(LinearCoord3D a, LinearCoord3D b);

    // Угол между векторами (радианы)
    static double radBetweenCoord3D(LinearCoord3D a, LinearCoord3D b);

    // Угол между угловыми координатами (градусы)
    static double degBetweenSphericalCoords(SphericalCoordDeg const & a, SphericalCoordDeg const & b);

    // Угол между угловыми координатами (радианы)
    static double radBetweenSphericalCoords(SphericalCoordRad const & a, SphericalCoordRad const & b);

};

