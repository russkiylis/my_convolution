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

    struct PeakConfig
    {
        explicit PeakConfig(double const &center, double const &amplitude);

        virtual PeakType type() const = 0;

        virtual std::unique_ptr<PeakConfig> clone() const = 0;

        virtual std::unique_ptr<AbstractPeak> createPeak() const = 0;

        virtual ~PeakConfig() = default;

        double center;
        double amplitude;
    };

    explicit AbstractPeak(double const &center = 180.0, double const &amplitude = 1.0, QObject *parent = nullptr);
    explicit AbstractPeak(PeakConfig const &config, QObject *parent = nullptr);
    ~AbstractPeak() override = default;

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

protected:
    double _center;     // Координата пика (в градусах)
    double _amplitude;    // Амплитуда пика (в у.е.)
};

// Класс гауссовского пика функции правдоподобия
class GaussPeak final: public AbstractPeak
{
public:
    struct GaussPeakConfig final: public PeakConfig
    {
        explicit GaussPeakConfig(double const &center, double const &amplitude, double const &sigma);

        std::unique_ptr<PeakConfig> clone() const override;

        std::unique_ptr<AbstractPeak> createPeak() const override;

        PeakType type() const override;

        double sigma;   // СКО
    };

    explicit GaussPeak(
        double const &center = 180.0,
        double const &amplitude = 1.0,
        double const &sigma = 10.0,
        QObject *parent = nullptr
        );
    explicit GaussPeak(GaussPeakConfig const &config, QObject *parent = nullptr);

    // Получить тип пика
    [[nodiscard]] PeakType type() const override;

    // Получить посчитанное значение в неком градусе
    [[nodiscard]] double valueAt(double const &deg) const override;

    // Получить значение СКО
    [[nodiscard]] double sigma() const;

    // Установить значение СКО
    void setSigma(double const &sigma);

private:
    double _sigma;   // Среднеквадратическое отклонение (СКО)
};

// Класс треугольного пика функции правдоподобия
class TrianglePeak final: public AbstractPeak
{
public:
    struct TrianglePeakConfig final: public PeakConfig
    {
        explicit TrianglePeakConfig(double const &center, double const &amplitude, double const &halfWidth);

        std::unique_ptr<AbstractPeak> createPeak() const override;

        PeakType type() const override;

        std::unique_ptr<PeakConfig> clone() const override;

        double halfWidth;   // Половина ширины
    };


    explicit TrianglePeak(
        double const &center = 180.0,
        double const &amplitude = 1.0,
        double const &halfWidth = 10.0,
        QObject *parent = nullptr
        );
    explicit TrianglePeak(TrianglePeakConfig const &config, QObject *parent = nullptr);

    // Получить тип пика
    [[nodiscard]] PeakType type() const override;

    // Получить посчитанное значение в неком градусе
    [[nodiscard]] double valueAt(double const &deg) const override;

    // Получить значение полуширины
    [[nodiscard]] double halfWidth() const;

    // Установить значение полуширины
    void setHalfWidth(double const &halfWidth);

private:
    double _halfWidth;   // Половина ширины
};

// Клас прямоугольного пика функции правдоподобия
class RectanglePeak final: public AbstractPeak
{
public:
    struct RectanglePeakConfig final: public PeakConfig
    {
        explicit RectanglePeakConfig(double const &center, double const &amplitude, double const &halfWidth);

        std::unique_ptr<AbstractPeak> createPeak() const override;

        PeakType type() const override;

        std::unique_ptr<PeakConfig> clone() const override;

        double halfWidth;   // Половина ширины
    };


    explicit RectanglePeak(
    double const &center = 180.0,
    double const &amplitude = 1.0,
    double const &halfWidth = 10.0,
    QObject *parent = nullptr
    );
    explicit RectanglePeak(RectanglePeakConfig const &config, QObject *parent = nullptr);

    // Получить тип пика
    [[nodiscard]] PeakType type() const override;

    // Получить посчитанное значение в неком градусе
    [[nodiscard]] double valueAt(double const &deg) const override;

    // Получить значение полуширины
    [[nodiscard]] double halfWidth() const;

    // Установить значение полуширины
    void setHalfWidth(double const &halfWidth);

private:
    double _halfWidth;
};