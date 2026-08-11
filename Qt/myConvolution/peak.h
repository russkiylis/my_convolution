#pragma once

#include <memory>
#include <QObject>

#include "geometryutils.h"

// Абстрактный класс пика функции правдоподобия
class AbstractPeak
{
public:
    // Тип пика
    enum class PeakType
    {
        Gauss,
        Triangle,
        Rectangle
    };

    struct PeakConfig
    {
        explicit PeakConfig(GeometryUtils::SphericalCoordDeg const &center, double const &amplitude);

        [[nodiscard]] virtual PeakType type() const = 0;

        [[nodiscard]] virtual QString typeStr() const = 0;

        [[nodiscard]] virtual std::unique_ptr<PeakConfig> clone() const = 0;

        [[nodiscard]] virtual std::unique_ptr<AbstractPeak> createPeak() const = 0;

        virtual ~PeakConfig() = default;

        GeometryUtils::SphericalCoordDeg center;
        double amplitude;
    };

    explicit AbstractPeak(GeometryUtils::SphericalCoordDeg const &center = GeometryUtils::SphericalCoordDeg(180, 0), double const &amplitude = 1.0);
    explicit AbstractPeak(PeakConfig const &config);
    virtual ~AbstractPeak() = default;

    // Получить тип пика
    [[nodiscard]] virtual PeakType type() const = 0;

    // Получить посчитанное значение в неком градусе
    [[nodiscard]] virtual double valueAt(GeometryUtils::LinearCoord3D const &deg) const = 0;

    // Получить центральный градус
    [[nodiscard]] GeometryUtils::SphericalCoordDeg center() const;

    // Задать центральный градус
    void setCenter(const GeometryUtils::SphericalCoordDeg &center);

    // Получить амплитуду
    [[nodiscard]] double amplitude() const;

    // Задать амплитуду
    void setAmplitude(const double &amplitude);

protected:
    GeometryUtils::LinearCoord3D m_center;     // Единичный вектор на центр пика
    double m_amplitude;    // Амплитуда пика (в у.е.)
};

// Класс гауссовского пика функции правдоподобия
class GaussPeak final: public AbstractPeak
{
public:
    struct GaussPeakConfig final: PeakConfig
    {
        explicit GaussPeakConfig(GeometryUtils::SphericalCoordDeg const &center, double const &amplitude, double const &sigma);

        [[nodiscard]] std::unique_ptr<PeakConfig> clone() const override;

        [[nodiscard]] std::unique_ptr<AbstractPeak> createPeak() const override;

        [[nodiscard]] PeakType type() const override;

        [[nodiscard]] QString typeStr() const override;

        double sigma;   // СКО
    };

    explicit GaussPeak(
        GeometryUtils::SphericalCoordDeg const &center = GeometryUtils::SphericalCoordDeg(180, 0),
        double const &amplitude = 1.0,
        double const &sigma = 10.0
        );
    explicit GaussPeak(GaussPeakConfig const &config);

    // Получить тип пика
    [[nodiscard]] PeakType type() const override;

    // Получить посчитанное значение в неком градусе
    [[nodiscard]] double valueAt(GeometryUtils::LinearCoord3D const &deg) const override;

    // Получить значение СКО
    [[nodiscard]] double sigma() const;

    // Установить значение СКО
    void setSigma(double const &sigma);

private:
    double m_sigma;   // Среднеквадратическое отклонение (СКО)
};

// Класс треугольного пика функции правдоподобия
class TrianglePeak final: public AbstractPeak
{
public:
    struct TrianglePeakConfig final: PeakConfig
    {
        explicit TrianglePeakConfig(GeometryUtils::SphericalCoordDeg const &center, double const &amplitude, double const &halfWidth);

        [[nodiscard]] std::unique_ptr<AbstractPeak> createPeak() const override;

        [[nodiscard]] PeakType type() const override;

        [[nodiscard]] QString typeStr() const override;

        [[nodiscard]] std::unique_ptr<PeakConfig> clone() const override;

        double halfWidth;   // Половина ширины
    };


    explicit TrianglePeak(
        GeometryUtils::SphericalCoordDeg const &center = GeometryUtils::SphericalCoordDeg(180, 0),
        double const &amplitude = 1.0,
        double const &halfWidth = 10.0
        );
    explicit TrianglePeak(TrianglePeakConfig const &config);

    // Получить тип пика
    [[nodiscard]] PeakType type() const override;

    // Получить посчитанное значение в неком градусе
    [[nodiscard]] double valueAt(GeometryUtils::LinearCoord3D const &deg) const override;

    // Получить значение полуширины
    [[nodiscard]] double halfWidth() const;

    // Установить значение полуширины
    void setHalfWidth(double const &halfWidth);

private:
    double m_halfWidth;   // Половина ширины
};

// Клас прямоугольного пика функции правдоподобия
class RectanglePeak final: public AbstractPeak
{
public:
    struct RectanglePeakConfig final: PeakConfig
    {
        explicit RectanglePeakConfig(GeometryUtils::SphericalCoordDeg const &center, double const &amplitude, double const &halfWidth);

        [[nodiscard]] std::unique_ptr<AbstractPeak> createPeak() const override;

        [[nodiscard]] PeakType type() const override;

        [[nodiscard]] QString typeStr() const override;

        [[nodiscard]] std::unique_ptr<PeakConfig> clone() const override;

        double halfWidth;   // Половина ширины
    };


    explicit RectanglePeak(
    GeometryUtils::SphericalCoordDeg const &center = GeometryUtils::SphericalCoordDeg(180, 0),
    double const &amplitude = 1.0,
    double const &halfWidth = 10.0
    );
    explicit RectanglePeak(RectanglePeakConfig const &config);

    // Получить тип пика
    [[nodiscard]] PeakType type() const override;

    // Получить посчитанное значение в неком градусе
    [[nodiscard]] double valueAt(GeometryUtils::LinearCoord3D const &deg) const override;

    // Получить значение полуширины
    [[nodiscard]] double halfWidth() const;

    // Установить значение полуширины
    void setHalfWidth(double const &halfWidth);

private:
    double m_halfWidth;
};