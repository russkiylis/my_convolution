#pragma once
#include "generator.h"
#include "subcommand.h"
#include "synconvconfig.h"

namespace synconv
{

class CreateConfigSubcommand : public Subcommand
{
public:
    explicit CreateConfigSubcommand(CLI::App* app);

private:
    SynconvConfig m_config;
};

} // synconv