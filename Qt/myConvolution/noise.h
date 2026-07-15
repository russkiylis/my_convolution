#pragma once

#include <QObject>
#include <random>

class AbstractNoise : public QObject
{
    Q_OBJECT

public:
    // Тип шума
    enum class NoiseType
    {
        Normal,
        Uniform
    };
    Q_ENUM(NoiseType)

    // Конфигурация шума
    struct NoiseConfig
    {
        explicit NoiseConfig(unsigned int const &seed);
        explicit NoiseConfig();

        [[nodiscard]] virtual NoiseType noiseType() const = 0;
        virtual ~NoiseConfig() = default;

        unsigned int seed;  // Семечко для генерации
    };

    // Конструктор с указанием семечка
    explicit AbstractNoise(unsigned int const &seed, QObject *parent = nullptr);

    // Конструктор со случайным семечком
    explicit AbstractNoise(QObject *parent = nullptr);

    ~AbstractNoise() override = default;

    // Получить тип шума
    [[nodiscard]] virtual NoiseType type() const = 0;

    // Получить следующее значение шума
    [[nodiscard]] virtual double next() = 0;

    // Получить семечко
    [[nodiscard]] unsigned int seed() const;

    // Задать семечко
    void setSeed(unsigned int const &seed);

    // Получить движок рандомного генератора
    [[nodiscard]] std::mt19937 &rng();

    // Задать движок рандомного генератора
    void setRng(const std::mt19937 &rng);

protected:
    unsigned int _seed;   // Семечко для генератора
    std::mt19937 _rng;  // Движок рандомного генератора
};

class NormalNoise final: public AbstractNoise
{
public:
    struct NormalNoiseConfig final : public NoiseConfig
    {
        explicit NormalNoiseConfig(double const &mean, double const &sigma, unsigned int const &seed);
        explicit NormalNoiseConfig(double const &mean, double const &sigma);

        [[nodiscard]] NoiseType noiseType() const override;

        double mean;    // Математическое ожидание
        double sigma;   // Скреднекватдратическое отклонение
    };


    // Конструктор с указанием семечка
    explicit NormalNoise(double const &mean, double const &sigma, unsigned int const &seed, QObject *parent = nullptr);

    // Конструктор со случайным семечком
    explicit NormalNoise(double const &mean, double const &sigma, QObject *parent = nullptr);

    // Получить тип шума
    [[nodiscard]] NoiseType type() const override;

    // Получить следующее значение шума
    [[nodiscard]] double next() override;

    // Получить математическое ожидание
    [[nodiscard]] double mean() const;

    // Установить математическое ожидание
    void setMean(double const &mean);

    // Получить среднеквадратическое отклонение
    [[nodiscard]] double sigma() const;

    // Установить среднеквадратическое отклонение
    void setSigma(double const &sigma);

private:
    std::normal_distribution<double> _distribution; // Гауссово распределение. Туда суём движок и параметры

    double _mean;   // Математическое ожидание шума
    double _sigma;  // Среднеквадратическое отклонение шума
};

class UniformNoise final: public AbstractNoise
{
public:
    struct UniformNoiseConfig final : public NoiseConfig
    {
        explicit UniformNoiseConfig(double const &min, double const &max, unsigned int const &seed);
        explicit UniformNoiseConfig(double const &min, double const &max);

        [[nodiscard]] NoiseType noiseType() const override;

        double min;    // Минимальное значение
        double max;   // Максимальное значение
    };

    // Конструктор с указанием семечка
    explicit UniformNoise(double const &min, double const &max, unsigned int const &seed, QObject *parent = nullptr);

    // Конструктор со случайным семечком
    explicit UniformNoise(double const &min, double const &max, QObject *parent = nullptr);

    // Получить тип шума
    [[nodiscard]] NoiseType type() const override;

    // Получить следующее значение шума
    [[nodiscard]] double next() override;

    // Получить минимальное значение
    [[nodiscard]] double min() const;

    // Установить минимальное значение
    void setMin(double const &min);

    // Получить максимальное значение
    [[nodiscard]] double max() const;

    // Установить максимальное значение
    void setMax(double const &max);

private:
    std::uniform_real_distribution<double> _distribution; // Равномерное распределение. Туда суём движок и параметры

    double _min;    // Минимальное значение
    double _max;    // Максимальное значение
};

