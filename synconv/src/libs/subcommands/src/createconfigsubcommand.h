#pragma once
#include "generator.h"
#include "subcommand.h"
#include "../../global/src/synconvconfig.h"

namespace synconv
{

class CreateConfigSubcommand : public Subcommand
{
public:
    explicit CreateConfigSubcommand(CLI::App* app);
};

} // synconv