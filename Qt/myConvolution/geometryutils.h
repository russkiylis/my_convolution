#pragma once
#include "abstractnoisebackend.h"


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

        // Длина вектора
        [[nodiscard]] double length() const;

        [[nodiscard]] double x() const;
        void setX(double x);
        [[nodiscard]] double y() const;
        void setY(double y);
        [[nodiscard]] double z() const;
        void setZ(double z);

    private:
        double m_x;
        double m_y;
        double m_z;
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

    // TODO: Добавить расчёт углового расстояния
};

