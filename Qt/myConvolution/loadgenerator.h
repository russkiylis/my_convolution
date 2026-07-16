#pragma once

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QtPositioning>
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
        bool enabled;       // Генерирует ли пост
        QString postName;   // Имя поста
        double latitude;    // Широта (градусы)
        double longitude;   // Долгота (градусы)
        double frequency;   // Частота (Гц)
        double level;       // Амплитуда (дБ) (Гауссово распределение)
        double levelSigma;  // СКО амплитуды (Гауссово распределение)

        int minAngleH;               // Минимальный угол (горизонтальный)
        int maxAngleH;               // Максимальный угол (горизонтальный)
        int minAngleV;               // Минимамльный угол (вертикальный)
        int maxAngleV;               // Максимальный угол (вертикальный)
        double stepH;                // Шаг (горизонтальный)
        double stepV;                // Шаг (вертикальный)
        unsigned int minPeriod;      // Минимальный период между отправками
        unsigned int maxPeriod;      // Максимальный период между отправками

        std::unique_ptr<AbstractNoise::NoiseConfig> noiseConfig;                // Шум
        std::vector<std::unique_ptr<AbstractPeak::PeakConfig>> peakConfigsV;    // Пики по вертикали
        std::vector<std::unique_ptr<AbstractPeak::PeakConfig>> peakConfigsH;   // Пики по горизонтали

        PostConfig(const PostConfig &other);
        PostConfig & operator=(const PostConfig &other);
    };

    // Пакет данных
    struct DataPackage
    {
        QDateTime timestamp;        // Временная метка
        double bearingH;            // Направление по горизонтали
        double bearingV;            // Направление по вертикали
        double qualityH;            // Качество свёртки по горизонтали
        double qualityV;            // Качество свёртки по вертикали
        double level;               // Мощность
        double frequency;           // Частота
        QGeoCoordinate coordinate;  // Координата
        QString postName;           // Имя поста

        int minAngleH;               // Минимальный угол (горизонтальный)
        int maxAngleH;               // Максимальный угол (горизонтальный)
        int minAngleV;               // Минимамльный угол (вертикальный)
        int maxAngleV;               // Максимальный угол (вертикальный)
        double stepH;                // Шаг (горизонтальный)
        double stepV;                // Шаг (вертикальный)

        std::vector<double> convH;   // Свёртка по горизонтали
        std::vector<double> convV;   // Свёртка по вертикали
    };

    // Класс одного поста
    class Post
    {
        using Clock = std::chrono::steady_clock;
        using TimePoint = Clock::time_point;

    public:
        explicit Post(PostConfig const &config);

    private:
        PostConfig _config;
        DataPackage _data;

        std::mt19937 _rng;       // Движок случайной генерации
        TimePoint _nextGenTime;     // Следующее время генерации
        void newNextGenTime();  // Создать следующее время генерации

        std::vector<double> _degH;  // Углы по горизонтали
        std::vector<double> _degV;  // Углы по вертикали

        std::unique_ptr<AbstractNoise> _noise;              // Шум (мы не знаем что это за шум)
        std::vector<std::unique_ptr<AbstractPeak>> _peaksV; // Вектор пиков по вертикали (мы не знаем что это за пики)
        std::vector<std::unique_ptr<AbstractPeak>> _peaksH; // Вектор пиков по горизонтали

        // Генерация сигнала (если пришло время) и отправка пакета данных
        void call(TimePoint const &now);
    signals:
        void signalSendData(DataPackage const &data);
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
