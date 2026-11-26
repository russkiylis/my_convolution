#pragma once
#include "subcommand.h"

namespace synconv
{

class CreateConfigSubcommand : public Subcommand
{
public:
    explicit CreateConfigSubcommand(CLI::App* app);
};

} // synconv