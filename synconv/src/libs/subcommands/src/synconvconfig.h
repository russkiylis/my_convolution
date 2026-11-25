#pragma once
#include <string>
#include <vector>

using PeakTuple = std::tuple<std::string, double, double, double, double>;
using NoiseTuple = std::tuple<std::string, float, float>;

struct SynconvConfig
{
    // Путь к файлу конфигурации
    std::string configPath {"config.toml"};

    // Конфигурация пиков. Один пик содержит в себе следующие значения:
    // - Тип пика (std::string) [gauss/triangle/rectangle]
    // - Азимут пика (float) [0-360]
    // - Угол места пика (float) [-90-90]
    // - Амплитуда пика (float) [>0]
    // - СКО/полуширина пика (float) [>0]
    std::vector<PeakTuple> peaks;

    // Конфигурация шумов. Один шум содержит в себе следующие значения:
    // - Тип шума (std::string) [normal/uniform]
    // - Матожидание/минимум шума (float) [>0]
    // - СКО/максимум шума (float) [>0]
    std::vector<NoiseTuple> noises;

    // Границы по азимуту и углу места
    // - минимум азимута (int) [0-360]
    // - максимум азимута (int) [0-360]
    // - минимум угла места (int) [-90-90]
    // - максимум угла места (int) [-90-90]
    std::tuple<int, int, int, int> bounds;
};
