#include "generator.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <functional>
#include <numbers>
#include <random>
#include <stdexcept>

namespace synconv::generator
{
namespace
{

// Все допустимые шаги выражаются целым числом двадцатых долей градуса.
constexpr double ticksPerDegree = 20.0;
constexpr std::array allowedSteps{0.1, 0.25, 0.5, 1.0, 5.0};

double regularAngle(const Axis& axis, std::size_t index)
{
    if (index == 0)
        return axis.minDeg;
    return std::fma(static_cast<double>(index), axis.stepDeg * ticksPerDegree,
                    axis.minDeg * ticksPerDegree) / ticksPerDegree;
}

std::size_t sampleCount(const Axis& axis, double lowerBound, double upperBound)
{
    if (!(lowerBound <= axis.minDeg && axis.minDeg <= axis.maxDeg && axis.maxDeg <= upperBound))
        throw std::invalid_argument("Angular grid bounds outside the permitted range");
    if (std::find(allowedSteps.begin(), allowedSteps.end(), axis.stepDeg) == allowedSteps.end())
        throw std::invalid_argument("Angular step must be 0.1, 0.25, 0.5, 1 or 5 degrees");

    const auto steps = static_cast<std::size_t>((axis.maxDeg - axis.minDeg) / axis.stepDeg);
    return steps + 1 + (regularAngle(axis, steps) < axis.maxDeg ? 1 : 0);
}

double angleAt(const Axis& axis, std::size_t index, std::size_t count)
{
    if (index == count - 1)
        return axis.maxDeg;
    return regularAngle(axis, index);
}

bool isPole(double elevationDeg)
{
    return std::abs(elevationDeg) == 90.0;
}

struct Direction
{
    double x;
    double y;
    double z;
    double elevationDeg;

    bool operator==(const Direction&) const = default;
};

constexpr double radiansPerDegree = std::numbers::pi / 180.0;

Direction directionAt(double azimuthDeg, double elevationDeg)
{
    if (isPole(elevationDeg))
        return {0.0, 0.0, std::copysign(1.0, elevationDeg), elevationDeg};

    const double azimuth = (azimuthDeg == 360.0 ? 0.0 : azimuthDeg) * radiansPerDegree;
    const double elevation = elevationDeg * radiansPerDegree;
    const double horizontal = std::cos(elevation);
    return {std::cos(azimuth) * horizontal, std::sin(azimuth) * horizontal, std::sin(elevation), elevationDeg};
}

double distanceDeg(const Direction& a, const Direction& b)
{
    const double dot = a.x * b.x + a.y * b.y + a.z * b.z;
    const double crossLength = std::hypot(a.y * b.z - a.z * b.y,
                                          a.z * b.x - a.x * b.z,
                                          a.x * b.y - a.y * b.x);
    return std::atan2(crossLength, dot) / radiansPerDegree;
}

struct PreparedPeak
{
    PeakShape shape;
    Direction center;
    double amplitude;
    double widthDeg;

    double valueAt(const Direction& direction) const
    {
        if (widthDeg == 0.0)
            return center == direction ? amplitude : 0.0;

        double distance;
        if (isPole(center.elevationDeg) || isPole(direction.elevationDeg)) {
            const double pole = isPole(center.elevationDeg) ? center.elevationDeg : direction.elevationDeg;
            const double other = isPole(center.elevationDeg) ? direction.elevationDeg : center.elevationDeg;
            const double towardPole = pole > 0.0 ? other : -other;
            // Сравнение с широтой края включает, например, 89.8° при полуширине 0.2°.
            if (shape == PeakShape::rectangle)
                return towardPole >= 90.0 - widthDeg ? amplitude : 0.0;
            distance = 90.0 - towardPole;
        } else {
            distance = distanceDeg(center, direction);
        }
        switch (shape) {
        case PeakShape::gaussian: {
            const double scaled = distance / widthDeg;
            return amplitude * std::exp(-0.5 * scaled * scaled);
        }
        case PeakShape::triangle:
            return amplitude * std::max(0.0, 1.0 - distance / widthDeg);
        case PeakShape::rectangle:
            return distance <= widthDeg ? amplitude : 0.0;
        }
        throw std::invalid_argument("Unknown peak shape");
    }
};

using NoiseSampler = std::function<double(std::mt19937&)>;

NoiseSampler makeSampler(const Noise& noise)
{
    if (const auto* normal = std::get_if<NormalNoise>(&noise)) {
        if (!std::isfinite(normal->mean) || !std::isfinite(normal->sigma) || normal->sigma < 0)
            throw std::invalid_argument("Invalid normal noise parameters");
        if (normal->sigma == 0.0)
            return [mean = normal->mean](std::mt19937&) { return mean; };
        return std::normal_distribution<double>(normal->mean, normal->sigma);
    }

    const auto& uniform = std::get<UniformNoise>(noise);
    if (!std::isfinite(uniform.max - uniform.min) || uniform.max < uniform.min)
        throw std::invalid_argument("Invalid uniform noise bounds");
    if (uniform.min == uniform.max)
        return [value = uniform.min](std::mt19937&) { return value; };
    return std::uniform_real_distribution<double>(uniform.min, uniform.max);
}

} // namespace

std::vector<double> generate(const Config& config)
{
    const auto countH = sampleCount(config.horizontal, 0.0, 360.0);
    const auto countV = sampleCount(config.vertical, -90.0, 90.0);
    std::vector<double> samples;

    std::vector<PreparedPeak> peaks;
    for (const auto& peak : config.peaks) {
        if (!(0.0 <= peak.azimuthDeg && peak.azimuthDeg <= 360.0 &&
              -90.0 <= peak.elevationDeg && peak.elevationDeg <= 90.0) ||
            !std::isfinite(peak.amplitude) || !std::isfinite(peak.widthDeg) || peak.widthDeg < 0)
            throw std::invalid_argument("Invalid peak parameters");
        peaks.push_back({peak.shape, directionAt(peak.azimuthDeg, peak.elevationDeg),
                         peak.amplitude, peak.widthDeg});
    }

    std::vector<NoiseSampler> noises;
    for (const auto& noise : config.noises)
        noises.push_back(makeSampler(noise));

    std::mt19937 rng;
    if (!noises.empty())
        rng.seed(config.seed ? *config.seed : std::random_device{}());

    samples.reserve(countV * countH);
    const bool fullAzimuth = config.horizontal.minDeg == 0.0 && config.horizontal.maxDeg == 360.0;
    double maximum = 0.0;
    for (std::size_t v = 0; v < countV; ++v) {
        const double elevation = angleAt(config.vertical, v, countV);
        const auto rowStart = samples.size();
        const auto distinctDirections = isPole(elevation) ? 1 : countH - (fullAzimuth ? 1 : 0);
        for (std::size_t h = 0; h < distinctDirections; ++h) {
            double value = 0.0;
            for (auto& noise : noises)
                value += noise(rng);
            if (!peaks.empty()) {
                const double azimuth = angleAt(config.horizontal, h, countH);
                const auto direction = directionAt(azimuth, elevation);
                for (const auto& peak : peaks)
                    value += peak.valueAt(direction);
            }

            if (!std::isfinite(value))
                throw std::overflow_error("Convolution sample exceeds double range");
            samples.push_back(std::abs(value));
            maximum = std::max(maximum, samples.back());
        }
        // На полюсе все азимуты совпадают, а на полном обороте 360° повторяет 0°.
        const double first = samples[rowStart];
        samples.insert(samples.end(), countH - distinctDirections, first);
    }
    constexpr double outputLimit = 100.0;
    if (maximum > outputLimit) {
        for (double& value : samples)
            value = (value / maximum) * outputLimit;
    }
    return samples;
}

} // namespace synconv::generator
