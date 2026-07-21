#pragma once

#include <QObject>
#include <random>

class AbstractNoise
{

public:
    // Тип шума
    enum class NoiseType
    {
        Normal,
        Uniform
    };

    // Конфигурация шума
    struct NoiseConfig
    {
        explicit NoiseConfig(unsigned int const &seed);
        explicit NoiseConfig();

        [[nodiscard]] virtual NoiseType noiseType() const = 0;

        [[nodiscard]] virtual std::unique_ptr<NoiseConfig> clone() = 0;

        virtual std::unique_ptr<AbstractNoise> createNoise() const = 0;

        virtual ~NoiseConfig() = default;

        unsigned int seed;  // Семечко для генерации
    };

    // Конструктор с указанием семечка
    explicit AbstractNoise(unsigned int const &seed);
    explicit AbstractNoise(NoiseConfig const &config);

    // Конструктор со случайным семечком
    explicit AbstractNoise();

    virtual ~AbstractNoise() = default;

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
    unsigned int m_seed;   // Семечко для генератора
    std::mt19937 m_rng;  // Движок рандомного генератора
};

class NormalNoise final: public AbstractNoise
{
public:
    struct NormalNoiseConfig final : public NoiseConfig
    {
        explicit NormalNoiseConfig(double const &mean, double const &sigma, unsigned int const &seed);
        explicit NormalNoiseConfig(double const &mean, double const &sigma);

        // Клонирование объекта с умным указателем
        [[nodiscard]] std::unique_ptr<NoiseConfig> clone() override;

        std::unique_ptr<AbstractNoise> createNoise() const override;

        [[nodiscard]] NoiseType noiseType() const override;

        double mean;    // Математическое ожидание
        double sigma;   // Скреднекватдратическое отклонение
    };


    // Конструктор с указанием семечка
    explicit NormalNoise(double const &mean, double const &sigma, unsigned int const &seed);

    // Конструктор со случайным семечком
    explicit NormalNoise(double const &mean, double const &sigma);

    explicit NormalNoise(NormalNoiseConfig const &config);

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
    std::normal_distribution<double> m_distribution; // Гауссово распределение. Туда суём движок и параметры

    double m_mean;   // Математическое ожидание шума
    double m_sigma;  // Среднеквадратическое отклонение шума
};

class UniformNoise final: public AbstractNoise
{
public:
    struct UniformNoiseConfig final : public NoiseConfig
    {
        explicit UniformNoiseConfig(double const &min, double const &max, unsigned int const &seed);
        explicit UniformNoiseConfig(double const &min, double const &max);

        [[nodiscard]] NoiseType noiseType() const override;

        std::unique_ptr<AbstractNoise> createNoise() const override;

        // Клонирование объекта с умным указателем
        [[nodiscard]] std::unique_ptr<NoiseConfig> clone() override;

        double min;    // Минимальное значение
        double max;   // Максимальное значение
    };

    // Конструктор с указанием семечка
    explicit UniformNoise(double const &min, double const &max, unsigned int const &seed);

    // Конструктор со случайным семечком
    explicit UniformNoise(double const &min, double const &max);

    explicit UniformNoise(UniformNoiseConfig const &config);

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
    std::uniform_real_distribution<double> m_distribution; // Равномерное распределение. Туда суём движок и параметры

    double m_min;    // Минимальное значение
    double m_max;    // Максимальное значение
};

