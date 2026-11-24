#pragma once
#include "subcommand.h"

namespace synconv
{

class GenerateSubcommand : public Subcommand
{
public:
    explicit GenerateSubcommand(CLI::App* app);

};

} // synconv