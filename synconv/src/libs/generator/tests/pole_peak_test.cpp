#include "generator.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <cstddef>

namespace synconv::generator
{
namespace
{

TEST(PolarPeakTest, AllMeridiansHaveTheSameProfileAroundEitherPole)
{
    constexpr double amplitude = 16.0;
    constexpr double width = 0.5;
    constexpr std::size_t columns = 73; // 0..360 by 5 degrees
    Config config;
    config.horizontal = {0, 360, 5};

    for (const double pole : {-90.0, 90.0}) {
        SCOPED_TRACE(pole);
        config.vertical = pole < 0 ? Axis{-90, -89, 0.25} : Axis{89, 90, 0.25};

        for (const auto shape : {PeakShape::gaussian, PeakShape::triangle, PeakShape::rectangle}) {
            SCOPED_TRACE(static_cast<int>(shape));
            config.peaks = {{shape, 12, pole, amplitude, width}};
            const auto samples = generate(config);
            ASSERT_EQ(samples.size(), 5 * columns);

            config.peaks.front().azimuthDeg = 319;
            EXPECT_EQ(generate(config), samples);

            for (std::size_t row = 0; row < 5; ++row) {
                const double elevation = config.vertical.minDeg + row * 0.25;
                const double distance = std::abs(pole - elevation);
                double expected = 0.0;
                switch (shape) {
                case PeakShape::gaussian:
                    expected = amplitude * std::exp(-0.5 * std::pow(distance / width, 2));
                    break;
                case PeakShape::triangle:
                    expected = amplitude * std::max(0.0, 1.0 - distance / width);
                    break;
                case PeakShape::rectangle:
                    expected = distance <= width ? amplitude : 0.0;
                    break;
                }

                for (std::size_t h = 0; h < columns; ++h)
                    ASSERT_NEAR(samples[row * columns + h], expected, 1e-10)
                        << "elevation=" << elevation << " azimuth=" << h * 5;
            }
        }
    }
}

TEST(PolarPeakTest, ZeroWidthPeakCoversTheEntirePoleAndOnlyThePole)
{
    constexpr std::size_t columns = 73;
    Config config;
    config.horizontal = {0, 360, 5};
    for (const double pole : {-90.0, 90.0}) {
        SCOPED_TRACE(pole);
        config.vertical = pole < 0 ? Axis{-90, -89.5, 0.5} : Axis{89.5, 90, 0.5};
        for (const auto shape : {PeakShape::gaussian, PeakShape::triangle, PeakShape::rectangle}) {
            SCOPED_TRACE(static_cast<int>(shape));
            config.peaks = {{shape, 27, pole, 9, 0}};
            const auto samples = generate(config);
            ASSERT_EQ(samples.size(), 2 * columns);
            const auto poleStart = pole < 0 ? 0u : columns;
            for (std::size_t h = 0; h < columns; ++h) {
                ASSERT_EQ(samples[poleStart + h], 9);
                ASSERT_EQ(samples[columns - poleStart + h], 0);
            }
        }
    }
}

TEST(PolarPeakTest, OffPolePeakStillDistinguishesMeridians)
{
    Config config;
    config.horizontal = {0, 180, 5};
    for (const double elevation : {-89.5, 89.5}) {
        SCOPED_TRACE(elevation);
        config.vertical = {elevation, elevation, 1};
        config.peaks = {{PeakShape::rectangle, 0, elevation, 8, 0.25}};
        const auto samples = generate(config);
        ASSERT_EQ(samples.size(), 37u);
        EXPECT_EQ(samples.front(), 8);
        EXPECT_EQ(samples.back(), 0);
    }
}

TEST(PolarPeakTest, PoleOnDecimalEdgeOfAnOffPolePeakIsIncluded)
{
    Config config;
    config.horizontal = {0, 360, 5};
    for (const double pole : {-90.0, 90.0}) {
        config.vertical = {pole, pole, 0.1};
        config.peaks = {{PeakShape::rectangle, 217, std::copysign(89.8, pole), 8, 0.2}};
        const std::vector<double> expected(73, 8.0);
        EXPECT_EQ(generate(config), expected);
    }
}

} // namespace
} // namespace synconv::generator
