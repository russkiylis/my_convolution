#include "createconfigsubcommand.h"
#include "generatesubcommand.h"
#include "global.h"

#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <vector>

namespace synconv
{
namespace
{

std::string createConfig(const std::string& commandLine)
{
    g_synconvConfig = {};
    CLI::App app;
    CreateConfigSubcommand create(&app);
    app.parse(commandLine);
    return create.subApp()->config_to_str(true, true);
}

SynconvConfig readConfig(const std::string& text)
{
    g_synconvConfig = {};
    CLI::App app;
    GenerateSubcommand generate(&app);
    std::istringstream input(text);
    generate.subApp()->parse_from_stream(input);
    return g_synconvConfig;
}

TEST(ConfigOptionsTest, EmptyCreateConfigRoundTripsWithDefaultBounds)
{
    const auto config = readConfig(createConfig("create-config"));
    EXPECT_TRUE(config.peaks.empty());
    EXPECT_TRUE(config.noises.empty());
    EXPECT_EQ(config.bounds, SynconvConfig{}.bounds);
}

TEST(ConfigOptionsTest, ReadsMissingAndEmptyCollections)
{
    for (const std::string text : {"", "peak=[]\nnoise=[]", "peak=\"\"\nnoise=\"\""}) {
        SCOPED_TRACE(text);
        const auto config = readConfig(text);
        EXPECT_TRUE(config.peaks.empty());
        EXPECT_TRUE(config.noises.empty());
    }
}

TEST(ConfigOptionsTest, EmptyCollectionPreservesPopulatedCollection)
{
    const std::vector<PeakTuple> expectedPeaks{{"gauss", 180, 0, 25, 5}};
    const std::vector<NoiseTuple> expectedNoises{{"uniform", -1, 1}};
    for (const std::string emptyValue : {"[]", "\"\""}) {
        SCOPED_TRACE(emptyValue);
        const auto withPeak = readConfig("noise=" + emptyValue + "\npeak=[\"gauss\", 180, 0, 25, 5]");
        EXPECT_EQ(withPeak.peaks, expectedPeaks);
        EXPECT_TRUE(withPeak.noises.empty());

        const auto withNoise = readConfig("noise=[\"uniform\", -1, 1]\npeak=" + emptyValue);
        EXPECT_TRUE(withNoise.peaks.empty());
        EXPECT_EQ(withNoise.noises, expectedNoises);
    }
}

TEST(ConfigOptionsTest, MultipleTuplesAndExplicitBoundsRoundTrip)
{
    const auto config = readConfig(createConfig("create-config -p gauss 10 0 2 1 -p triangle 45 5 8 2 "
                                                "-n normal 0 2 -n uniform -1 1 --bounds 10 200 -30 40"));
    const std::vector<PeakTuple> expectedPeaks{{"gauss", 10, 0, 2, 1}, {"triangle", 45, 5, 8, 2}};
    const std::vector<NoiseTuple> expectedNoises{{"normal", 0, 2}, {"uniform", -1, 1}};
    EXPECT_EQ(config.peaks, expectedPeaks);
    EXPECT_EQ(config.noises, expectedNoises);
    EXPECT_EQ(config.bounds, std::make_tuple(10, 200, -30, 40));
}

TEST(ConfigOptionsTest, RejectsMalformedNonemptyTuples)
{
    for (const auto* text : {"peak=[\"gauss\", 0, 0, 1]", "noise=[\"normal\", 1]",
                             "peak=[\"\", 0, 0, 1, 1]", "noise=[\"\", 0, 1]",
                             "peak=[\"gauss\", 361, 0, 1, 1]", "noise=[\"uniform\", \"bad\", 1]"}) {
        SCOPED_TRACE(text);
        EXPECT_THROW(readConfig(text), CLI::ParseError);
    }
}

} // namespace
} // namespace synconv
