#pragma once

#include <cstdint>
#include <optional>
#include <variant>
#include <vector>

namespace synconv::generator
{

struct Axis
{
    double minDeg;
    double maxDeg;
    double stepDeg;
};

enum class PeakShape { gaussian, triangle, rectangle };

struct Peak
{
    PeakShape shape;
    double azimuthDeg;
    double elevationDeg;
    double amplitude;
    double widthDeg; // СКО для гауссовского пика, полуширина для остальных.
};

struct NormalNoise
{
    double mean;
    double sigma;
};

struct UniformNoise
{
    double min;
    double max;
};

using Noise = std::variant<NormalNoise, UniformNoise>;

struct Config
{
    Axis horizontal{0.0, 360.0, 0.1};
    Axis vertical{-45.0, 45.0, 1.0};
    std::vector<Peak> peaks;
    std::vector<Noise> noises;
    std::optional<std::uint32_t> seed; // Задай seed для повторяемого шума.
};

// Для границ и центров пиков: H от 0° до 360°, V от -90° до 90° включительно.
// Допустимые шаги каждой оси: 0.1°, 0.25°, 0.5°, 1°, 5°. Ошибка параметров — std::invalid_argument.
// Обе границы каждой оси входят в сетку. Если диапазон не кратен шагу,
// последний интервал короче. При minDeg == maxDeg на оси один отсчёт.
// Индекс отсчёта = v * countH + h. Каждый отсчёт — модуль суммы пиков и шумов.
// На полюсе вся строка содержит один и тот же отсчёт, включая шум.
// В полном азимутальном диапазоне отсчёт 360° совпадает с отсчётом 0°.
// Если максимум превышает 100, вся сетка пропорционально масштабируется до максимума 100.
[[nodiscard]] std::vector<double> generate(const Config& config);

} // namespace synconv::generator
