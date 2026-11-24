#include "createconfigsubcommand.h"

namespace synconv
{
CreateConfigSubcommand::CreateConfigSubcommand(CLI::App* app)
    : Subcommand(app)
{
    m_subApp = app->add_subcommand("create-config", "Создать конфигурацию синтетической свёртки");
}
} // synconv