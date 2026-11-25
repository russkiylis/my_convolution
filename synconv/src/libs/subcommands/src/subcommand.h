#pragma once
#include "CLI11.hpp"

namespace synconv
{

class Subcommand
{

public:
    explicit Subcommand(CLI::App* app);

    virtual ~Subcommand() = default;

    CLI::App* app() const;
    CLI::App* subApp() const;

protected:
    CLI::App* m_app;
    CLI::App* m_subApp;
};

} // synconv