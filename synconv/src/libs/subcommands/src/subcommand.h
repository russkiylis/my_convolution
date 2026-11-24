#pragma once
#include "CLI11.hpp"

namespace synconv
{

class Subcommand
{

public:
    explicit Subcommand(CLI::App* app);

    virtual ~Subcommand() = default;

protected:
    CLI::App* m_app;
    CLI::App* m_subApp;
};

} // synconv