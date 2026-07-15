#pragma once

#include <QObject>
#include <QString>
#include <vector>

#include "noise.h"
#include "peak.h"

class LoadGenerator : public QObject
{
    Q_OBJECT

public:
    // Конфигурация поста
    struct PostConfig
    {
        // TODO: Добавить угол места
        bool enabled;       // Генерирует ли пост
        QString postName;   // Имя поста
        double latitude;    // Широта (градусы)
        double longitude;   // Долгота (градусы)
        double frequency;   // Частота (Гц)
        double level;       // Амплитуда (дБ) (Гауссово распределение)
        double levelSigma;  // СКО амплитуды (Гауссово распределение)

        int min_angle;               // Минимальный угол
        int max_angle;               // Максимальный угол
        double step;                 // Шаг
        std::chrono::seconds minPeriod;      // Минимальный период между отправками
        std::chrono::seconds maxPeriod;      // Максимальный период между отправками

        std::unique_ptr<AbstractNoise::NoiseConfig> noiseConfig;                // Шум
        std::vector<std::unique_ptr<AbstractPeak::PeakConfig>> peakConfigs;     // Пики

        PostConfig(const PostConfig &other);
        PostConfig & operator=(const PostConfig &other);
    };

    // Класс одного поста
    class Post
    {
    public:
        explicit Post(PostConfig const &config);

    private:
        PostConfig _config;

        std::unique_ptr<AbstractNoise> _noise;              // Шум (мы не знаем что это за шум)
        std::vector<std::unique_ptr<AbstractPeak>> _peaks;  // Вектор пиков (мы не знаем что это за пики)
    };

    // Конструктор, когда нет пиков
    explicit LoadGenerator(std::vector<PostConfig> const &postConfigs, QObject *parent = nullptr);

    // Получить конфиг постов
    [[nodiscard]] std::vector<PostConfig> postConfigs() const;

    // Задать конфиг постов
    void setPostConfigs(const std::vector<PostConfig> &postConfigs);

private:

    // Прогрузить новые Post
    void load();

    std::vector<PostConfig> _postConfigs;   // Набор конфигов постов
    std::vector<Post> _posts;               // Набор постов
};
