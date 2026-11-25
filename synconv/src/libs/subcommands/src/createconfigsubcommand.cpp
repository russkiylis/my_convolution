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
    m_subApp->add_option("-o,--out", m_config.configPath, "Куда сохранить файл")->capture_default_str()->configurable(false);

    // Добавление опции для создания пика
    m_subApp->add_option_function<std::vector<PeakTuple>>("-p,--peak",
        [this](std::vector<PeakTuple> const &peaks) // <--- Принимаем вектор
        {
            for (const auto &peak : peaks) {        // <--- Перебираем все переданные пики
                m_config.peaks.push_back(peak);

                if (g_verbosity > 1) {
                    std::cerr << "Добавлен пик [тип:азимут:угол места:амплитуда:СКО/полуширина]:["
                    << std::get<0>(peak) << ":"
                    << std::get<1>(peak) << ":"
                    << std::get<2>(peak) << ":"
                    << std::get<3>(peak) << ":"
                    << std::get<4>(peak) << "]"
                    << std::endl;
                }
            }
        },
        "Добавить пик. Пример: -p gauss 180 0 25 5")
    ->type_name("[тип] [азимут] [угол места] [амплитуда] [СКО/полуширина]")
    ->allow_extra_args(false)
    ->group("Настройка пиков и шума")
    ->check(CLI::IsMember({"gauss", "triangle", "rectangle"}, CLI::ignore_case, CLI::ignore_underscore).application_index(0).description("Тип пика [gauss, triangle, rectangle]"))
    ->check(CLI::Range(0,360).application_index(1).description("Азимут в пределах [0 360]"))
    ->check(CLI::Range(-90, 90).application_index(2).description("Угол места в пределах [-90 90]"))
    ->check(CLI::PositiveNumber.application_index(3).description("Амплитуда >0"))
    ->check(CLI::PositiveNumber.application_index(4).description("СКО/полуширина >0"))
    ->description("Добавить пик в конфигурацию");


    // Добавление опции для создания шума
    m_subApp->add_option_function<std::vector<NoiseTuple>>("-n,--noise",
        [this](std::vector<NoiseTuple> const &noises)
        {
            for (const auto &noise : noises) {
                m_config.noises.push_back(noise);

                if (g_verbosity > 1) {
                    std::cerr << "Добавлен шум [тип:матожидание/минимум:СКО/максимум]:["
                    << std::get<0>(noise) << ":"
                    << std::get<1>(noise) << ":"
                    << std::get<2>(noise) << "]"
                    << std::endl;
                }
            }
        },
        "Добавить шум. Пример: -n normal 2 5")
        ->type_name("[тип] [матожидание/минимум] [СКО/максимум]")
        ->allow_extra_args(false)
        ->group("Настройка пиков и шума")
        ->check(CLI::IsMember({"normal", "uniform"}, CLI::ignore_case, CLI::ignore_underscore).application_index(0).description("Тип шума [normal, uniform]"))
        ->description("Добавить шум в конфигурацию");

    // Добавление границ
    m_subApp->add_option_function<std::tuple<int, int, int, int>>("--bounds",
        [this](std::tuple<int, int, int, int> const &bounds)
        {
            m_config.bounds = bounds;
            if (g_verbosity > 1) {
                std::cerr << "Добавлены границы [minАзимут:maxАзимут:minУМ:maxУМ]:["
                << std::get<0>(bounds) << ":"
                << std::get<1>(bounds) << ":"
                << std::get<2>(bounds) << ":"
                << std::get<3>(bounds) << "]"
                << std::endl;
            }
        },
        "Добавить границы. Пример: --bounds 10 200 -30 30")
    ->type_name("[minАзимут] [maxАзимут] [minУМ] [maxУМ]")
    ->allow_extra_args(false)
    ->group("Настройка сетки")
    ->check(CLI::Range(0,360).application_index(0).description("minАзимут в пределах [0 360]"))
    ->check(CLI::Range(0,360).application_index(0).description("maxАзимут в пределах [0 360]"))
    ->check(CLI::Range(-90,90).application_index(0).description("minУМ в пределах [-90 90]"))
    ->check(CLI::Range(-90,90).application_index(0).description("minУМ в пределах [-90 90]"))
    ->description("Границы по азимуту и углу места");

}

SynconvConfig CreateConfigSubcommand::config()
{
    if (g_verbosity > 3) {
        std::cerr << "CreateConfigSubcommand::config()" << std::endl;
    }

    return m_config;
}
} // synconv