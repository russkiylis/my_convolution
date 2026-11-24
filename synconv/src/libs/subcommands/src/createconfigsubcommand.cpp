#include "createconfigsubcommand.h"

namespace synconv
{
CreateConfigSubcommand::CreateConfigSubcommand(CLI::App* app)
    : Subcommand(app)
{
    m_subApp = app->add_subcommand("create-config", "Создать конфигурацию синтетической свёртки");

    m_subApp->add_option("-p,--peak", m_config.peaks, "Добавить пик.. \n"
                                                      "Пример: -p gauss 180 0 25 5")
    ->type_name("[gauss/triangle/square] [азимут] [угол места] [амплитуда] [СКО (полуширина)]")
    ->allow_extra_args(false)
    ->group("Настройка пиков и шума");
}
} // synconv