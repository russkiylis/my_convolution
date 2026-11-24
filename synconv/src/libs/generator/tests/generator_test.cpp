#include "generator.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace synconv::Generator
{
namespace
{

class GeneratorTest : public ::testing::Test
{
protected:
    Config config{{0, 10, 5}, {0, 0, 1}, {}, {}, 31};
};

TEST_F(GeneratorTest, StoresHorizontalSamplesConsecutivelyAndIncludesPartialFinalSteps)
{
    config.horizontal = {0, 17, 5};
    config.vertical = {0, 12, 5};
    config.peaks = {
        {PeakShape::rectangle, 5, 0, -2, 1},
        {PeakShape::rectangle, 10, 5, 4, 1},
        {PeakShape::rectangle, 15, 10, 6, 0},
        {PeakShape::rectangle, 17, 12, 8, 0}
    };
    const std::vector<double> expected{
        0, 2, 0, 0, 0,
        0, 0, 4, 0, 0,
        0, 0, 0, 6, 0,
        0, 0, 0, 0, 8
    };
    EXPECT_EQ(generate(config), expected);
}

TEST_F(GeneratorTest, GaussianUsesStandardDeviationInDegrees)
{
    config.peaks = {{PeakShape::gaussian, 5, 0, 2, 5}};
    const auto result = generate(config);
    ASSERT_EQ(result.size(), 3u);
    EXPECT_NEAR(result[0], 2.0 * std::exp(-0.5), 1e-10);
    EXPECT_DOUBLE_EQ(result[1], 2.0);
    EXPECT_NEAR(result[2], 2.0 * std::exp(-0.5), 1e-10);
}

TEST_F(GeneratorTest, TriangleCrossesAzimuthWrap)
{
    config.peaks = {{PeakShape::triangle, 355, 0, 1, 15}};
    const auto result = generate(config);
    ASSERT_EQ(result.size(), 3u);
    EXPECT_NEAR(result[0], 2.0 / 3.0, 1e-8);
    EXPECT_NEAR(result[1], 1.0 / 3.0, 1e-8);
    EXPECT_NEAR(result[2], 0.0, 1e-8);
}

TEST_F(GeneratorTest, RectangleUsesGreatCircleDistance)
{
    // At 60 degrees elevation a quarter turn in azimuth spans about 41.4 degrees.
    config.horizontal = {0, 360, 5};
    config.vertical = {60, 60, 1};
    config.peaks = {{PeakShape::rectangle, 0, 60, 10, 45}};
    const auto result = generate(config);
    ASSERT_EQ(result.size(), 73u);
    EXPECT_EQ(result[0], 10);
    EXPECT_EQ(result[18], 10); // h = 90°
    EXPECT_EQ(result[36], 0);  // h = 180°
    EXPECT_EQ(result[54], 10); // h = 270°
    EXPECT_EQ(result[72], 10);
}

TEST_F(GeneratorTest, ZeroWidthProducesOnlyTheCenterSample)
{
    const std::vector<double> expected{0, 5, 0};
    for (const auto shape : {PeakShape::gaussian, PeakShape::triangle, PeakShape::rectangle}) {
        SCOPED_TRACE(static_cast<int>(shape));
        config.peaks = {{shape, 5, 0, 5, 0}};
        EXPECT_EQ(generate(config), expected);
    }
}

TEST_F(GeneratorTest, AddsOverlappingPeaksAndBothNoisesBeforeTakingMagnitude)
{
    config.noises = {NormalNoise{-2, 0}, UniformNoise{3, 3}};
    config.peaks = {
        {PeakShape::rectangle, 5, 0, -7, 1},
        {PeakShape::rectangle, 5, 0, 2, 1}
    };
    const std::vector<double> expected{1, 4, 1};
    EXPECT_EQ(generate(config), expected);
}

TEST_F(GeneratorTest, PreservesMagnitudesAtTheLimit)
{
    config.peaks = {
        {PeakShape::rectangle, 5, 0, -100, 1},
        {PeakShape::rectangle, 10, 0, 27.5, 1}
    };
    const std::vector<double> expected{0, 100, 27.5};
    EXPECT_EQ(generate(config), expected);
}

TEST_F(GeneratorTest, ScalesAllMagnitudesWhenTheirMaximumExceedsTheLimit)
{
    config.noises = {UniformNoise{40, 40}};
    config.peaks = {
        {PeakShape::rectangle, 5, 0, -240, 1},
        {PeakShape::rectangle, 10, 0, 60, 1}
    };
    const std::vector<double> expected{20, 100, 50};
    EXPECT_EQ(generate(config), expected);
}

TEST_F(GeneratorTest, SeedReproducesEachNoiseDistribution)
{
    config.horizontal = {0, 32, 1};
    for (const auto& noise : std::vector<Noise>{NormalNoise{0, 1}, UniformNoise{-2, 3}}) {
        SCOPED_TRACE(noise.index());
        config.noises = {noise};
        config.seed = 37;
        const auto result = generate(config);
        ASSERT_EQ(result.size(), 33u);
        EXPECT_EQ(generate(config), result);
        EXPECT_TRUE(std::all_of(result.begin(), result.end(), [](double v){ return v >= 0 && v <= 100; }));
        config.seed = 73;
        EXPECT_NE(generate(config), result);
    }
}

TEST_F(GeneratorTest, ReturnsZerosForZeroSignal)
{
    const std::vector<double> expected(3, 0);
    EXPECT_EQ(generate(config), expected);
}

TEST_F(GeneratorTest, FullAzimuthSweepIncludesZeroAnd360Degrees)
{
    config.horizontal = {0, 360, 1};
    config.peaks = {{PeakShape::rectangle, 0, 0, 7, 0}};
    const auto result = generate(config);
    ASSERT_EQ(result.size(), 361u);
    EXPECT_DOUBLE_EQ(result.front(), 7);
    EXPECT_DOUBLE_EQ(result.back(), 7);
    EXPECT_EQ(std::count(result.begin(), result.end(), 7.0), 2);
}

TEST_F(GeneratorTest, IncludesDecimalEndpointExactlyOnce)
{
    // Both ranges have three steps, even though floating-point division rounds differently.
    for (const Axis axis : {Axis{0.0, 0.3, 0.1}, Axis{0.1, 0.4, 0.1}}) {
        config.horizontal = axis;
        config.peaks = {{PeakShape::rectangle, axis.maxDeg, 0, 7, 0}};
        const std::vector<double> expected{0, 0, 0, 7};
        EXPECT_EQ(generate(config), expected);
    }
}

TEST_F(GeneratorTest, IncludesBothBoundsWhenStepExceedsRange)
{
    config.horizontal = {0, 0, 1};
    config.vertical = {-1, 0, 5};
    config.peaks = {
        {PeakShape::rectangle, 0, -1, 4, 0},
        {PeakShape::rectangle, 0, 0, 7, 0}
    };
    const std::vector<double> expected{4, 7};
    EXPECT_EQ(generate(config), expected);
}

TEST_F(GeneratorTest, RejectsInvalidAxes)
{
    const double nan = std::numeric_limits<double>::quiet_NaN();
    for (const auto axis : std::vector<Axis>{{0, 1, 0}, {0, 1, -1}, {1, 0, 1}, {0, nan, 1}}) {
        config.vertical = axis;
        EXPECT_THROW((void)generate(config), std::invalid_argument);
    }
}

TEST_F(GeneratorTest, RejectsUnsupportedStepsOnEitherAxis)
{
    const double infinity = std::numeric_limits<double>::infinity();
    for (auto axis : {&config.horizontal, &config.vertical}) {
        for (double allowed : {0.1, 0.25, 0.5, 1.0, 5.0}) {
            for (double neighbor : {std::nextafter(allowed, 0.0), std::nextafter(allowed, infinity)}) {
                axis->stepDeg = neighbor;
                SCOPED_TRACE(::testing::Message() << "step near " << allowed);
                EXPECT_THROW((void)generate(config), std::invalid_argument);
            }
        }
        for (double step : {0.2, 2.0, 10.0, infinity, std::numeric_limits<double>::quiet_NaN()}) {
            axis->stepDeg = step;
            EXPECT_THROW((void)generate(config), std::invalid_argument);
        }
        axis->stepDeg = 1.0;
    }
}

TEST_F(GeneratorTest, RejectsAxisBoundsOutsideTheClosedAngularDomain)
{
    for (const auto axis : {Axis{std::nextafter(0.0, -1.0), 0, 1},
                            Axis{360, std::nextafter(360.0, 361.0), 1}}) {
        config.horizontal = axis;
        EXPECT_THROW((void)generate(config), std::invalid_argument);
    }
    config.horizontal = {0, 0, 1};
    for (const auto axis : {Axis{std::nextafter(-90.0, -91.0), -90, 1},
                            Axis{90, std::nextafter(90.0, 91.0), 1}}) {
        config.vertical = axis;
        EXPECT_THROW((void)generate(config), std::invalid_argument);
    }
}

TEST_F(GeneratorTest, RejectsPeakCentersOutsideTheClosedAngularDomain)
{
    for (double h : {std::nextafter(0.0, -1.0), std::nextafter(360.0, 361.0)}) {
        config.peaks = {{PeakShape::gaussian, h, 90, 1, 1}};
        EXPECT_THROW((void)generate(config), std::invalid_argument);
    }
    for (double v : {std::nextafter(-90.0, -91.0), std::nextafter(90.0, 91.0)}) {
        config.peaks = {{PeakShape::triangle, 360, v, 1, 1}};
        EXPECT_THROW((void)generate(config), std::invalid_argument);
    }
}

TEST_F(GeneratorTest, RejectsNegativeOrNonFinitePeakParameters)
{
    config.peaks = {{PeakShape::gaussian, 0, 0, 1, -1}};
    EXPECT_THROW((void)generate(config), std::invalid_argument);
    config.peaks = {{PeakShape::triangle, 0, 0, std::numeric_limits<double>::infinity(), 10}};
    EXPECT_THROW((void)generate(config), std::invalid_argument);
}

TEST_F(GeneratorTest, RejectsInvalidNoiseParameters)
{
    config.noises = {NormalNoise{2, -1}};
    EXPECT_THROW((void)generate(config), std::invalid_argument);
    config.noises = {UniformNoise{1, 0}};
    EXPECT_THROW((void)generate(config), std::invalid_argument);
}

} // namespace
} // namespace synconv::generator
