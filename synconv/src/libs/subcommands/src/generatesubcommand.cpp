#include "generatesubcommand.h"

namespace synconv
{
GenerateSubcommand::GenerateSubcommand(CLI::App* app)
    : Subcommand(app)
{
    m_subApp = app->add_subcommand("generate", "Сгенерировать синтетическую свёртку");
    registerConfigOptions(*m_subApp);
}
} // synconv