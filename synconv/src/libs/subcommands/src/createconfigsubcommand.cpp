#include "createconfigsubcommand.h"
#include "global.h"

namespace synconv
{
CreateConfigSubcommand::CreateConfigSubcommand(CLI::App* app)
    : Subcommand(app)
{
    if (g_verbosity > 3) {
        std::cerr << "CreateConfigSubcommand::CreateConfigSubcommand(CLI::App* app)";
    }

    m_subApp = app->add_subcommand("create-config", "Создать конфигурацию синтетической свёртки");
    m_subApp->fallthrough(true);

    // Добавление опции выбора куда сохранить конфиг
    m_subApp->add_option("-o,--out", g_synconvConfig.configPath, "Куда сохранить файл")->capture_default_str()->configurable(false);

    registerConfigOptions(*m_subApp);
}

} // synconv