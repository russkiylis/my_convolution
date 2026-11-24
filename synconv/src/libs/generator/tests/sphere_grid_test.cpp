#include "generator.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <cstddef>
#include <span>

namespace synconv::Generator
{
namespace
{

struct FullSphereGrid
{
    double step;
    std::size_t countH;
    std::size_t countV;
};

class FullSphereGridTest : public ::testing::TestWithParam<FullSphereGrid> {};

TEST_P(FullSphereGridTest, IncludesBothPolesAndClosesEveryAzimuthRowWithNoise)
{
    const auto [step, countH, countV] = GetParam();
    Config config;
    config.horizontal = {0, 360, step};
    config.vertical = {-90, 90, step};
    config.peaks = {
        {PeakShape::rectangle, 37, -90, 20, 0},
        {PeakShape::rectangle, 251, 90, 50, 0},
        {PeakShape::rectangle, 0, 0, 70, 0}
    };
    config.noises = {UniformNoise{1, 2}};
    config.seed = 42;

    const auto samples = generate(config);
    ASSERT_EQ(samples.size(), countV * countH);
    const auto row = [&](std::size_t v) {
        return std::span<const double>(samples).subspan(v * countH, countH);
    };
    const auto south = row(0);
    const auto north = row(countV - 1);
    EXPECT_GE(south.front(), 21);
    EXPECT_LE(south.front(), 22);
    EXPECT_GE(north.front(), 51);
    EXPECT_LE(north.front(), 52);
    for (const auto pole : {south, north})
        EXPECT_TRUE(std::all_of(pole.begin(), pole.end(), [&](double x) { return x == pole.front(); }));

    for (const auto nextToPole : {row(1), row(countV - 2)}) {
        EXPECT_TRUE(std::all_of(nextToPole.begin(), nextToPole.end(), [](double x) { return x >= 1 && x <= 2; }));
        EXPECT_TRUE(std::any_of(nextToPole.begin(), nextToPole.end(), [&](double x) { return x != nextToPole.front(); }));
    }
    const auto equator = row(countV / 2);
    EXPECT_GE(equator.front(), 71);
    EXPECT_LE(equator.front(), 72);
    EXPECT_TRUE(std::all_of(equator.begin() + 1, equator.end() - 1,
                            [](double x) { return x >= 1 && x <= 2; }));
    for (std::size_t v = 1; v + 1 < countV; ++v)
        ASSERT_EQ(row(v).front(), row(v).back()) << "v = " << v;
}

INSTANTIATE_TEST_SUITE_P(
    AngularSteps, FullSphereGridTest,
    ::testing::Values(
        FullSphereGrid{0.1, 3601, 1801},
        FullSphereGrid{0.25, 1441, 721},
        FullSphereGrid{0.5, 721, 361},
        FullSphereGrid{1, 361, 181},
        FullSphereGrid{5, 73, 37}));

} // namespace
} // namespace synconv::generator
