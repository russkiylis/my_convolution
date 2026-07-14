#pragma once

#include <QObject>

// Абстрактный класс пика функции правдоподобия
class AbstractPeak : public QObject
{
    Q_OBJECT

public:
    // Тип пика
    enum class PeakType
    {
        Gauss,
        Triangle,
        Rectangle
    };
    Q_ENUM(PeakType)

    explicit AbstractPeak(QObject *parent = nullptr);

    // Получить тип пика
    [[nodiscard]] virtual PeakType type() const = 0;

    // Получить посчитанное значение в неком градусе
    [[nodiscard]] virtual double valueAt(double const &deg) const = 0;

    // Получить центральный градус
    [[nodiscard]] double center() const;

    // Задать центральный градус
    void setCenter(const double &center);

    // Получить амплитуду
    [[nodiscard]] double amplitude() const;

    // Задать амплитуду
    void setAmplitude(const double &amplitude);

private:
    double _center = 180.0;     // Координата пика (в градусах)
    double _amplitude = 1.0;    // Амплитуда пика (в у.е.)
};

// Класс гауссовского пика функции правдоподобия
class GaussPeak final: public AbstractPeak
{
public:
    explicit GaussPeak(QObject *parent = nullptr);

    // Получить тип пика
    [[nodiscard]] PeakType type() const override;

    // Получить посчитанное значение в неком градусе
    [[nodiscard]] double valueAt(double const &deg) const override;

    // Получить значение СКО
    [[nodiscard]] double sigma() const;

    // Установить значение СКО
    void setSigma(double const &sigma);

private:
    double _sigma = 10.0;   // Среднеквадратическое отклонение (СКО)
};

// Класс треугольного пика функции правдоподобия
class TrianglePeak final: public AbstractPeak
{
public:
    explicit TrianglePeak(QObject *parent = nullptr);

    // Получить тип пика
    [[nodiscard]] PeakType type() const override;

    // Получить посчитанное значение в неком градусе
    [[nodiscard]] double valueAt(double const &deg) const override;

    // Получить значение полуширины
    [[nodiscard]] double halfWidth() const;

    // Установить значение полуширины
    void setHalfWidth(double const &halfWidth);

private:
    double _halfWidth = 10.0;   // Половина ширины
};

// Клас прямоугольного пика функции правдоподобия
class RectanglePeak final: public AbstractPeak
{
public:
    explicit RectanglePeak(QObject *parent = nullptr);

    // Получить тип пика
    [[nodiscard]] PeakType type() const override;

    // Получить посчитанное значение в неком градусе
    [[nodiscard]] double valueAt(double const &deg) const override;

    // Получить значение полуширины
    [[nodiscard]] double halfWidth() const;

    // Установить значение полуширины
    void setHalfWidth(double const &halfWidth);

private:
    double _halfWidth = 10.0;
};