#pragma once
#include <string>
#include <vector>

struct SynconvConfig
{
    std::vector<std::tuple<std::string, float, float, float, float>> peaks;
};
