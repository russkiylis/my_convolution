#include "subcommand.h"

#include "global.h"

#include <algorithm>

namespace synconv
{

namespace
{
class SynconvConfigTOML final : public CLI::ConfigTOML
{
public:
    std::vector<CLI::ConfigItem> from_config(std::istream& input) const override
    {
        auto items = CLI::ConfigTOML::from_config(input);
        std::erase_if(items, [](const CLI::ConfigItem& item) {
            const bool isPeakOrNoise = item.name == "peak" || item.name == "noise" ||
                                       item.name == "p" || item.name == "n";
            // Для [] CLI11 возвращает пустой список, для старой записи "" — одну пустую строку.
            return item.parents.empty() && isPeakOrNoise &&
                   (item.inputs.empty() || (item.inputs.size() == 1 && item.inputs.front().empty()));
        });
        return items;
    }
};
} // namespace

Subcommand::Subcommand(CLI::App* app)
    : m_app(app)
{
    if (g_verbosity > 3) {
        std::cerr << "Subcommand::Subcommand(CLI::App* app)";
    }
}

CLI::App* Subcommand::app() const
{
    if (g_verbosity > 3) {
        std::cerr << "Subcommand::app()" << std::endl;
    }
    return m_app;
}

CLI::App* Subcommand::subApp() const
{
    if (g_verbosity > 3) {
        std::cerr << "Subcommand::subApp()" << std::endl;
    }
    return m_subApp;
}

void Subcommand::registerConfigOptions(CLI::App& options)
{
    options.config_formatter(std::make_shared<SynconvConfigTOML>());

    // Добавление опции для создания пика
    options.add_option_function<std::vector<PeakTuple>>("-p,--peak",
        [](std::vector<PeakTuple> const &peaks)
        {
            for (const auto &peak : peaks) {
                g_synconvConfig.peaks.push_back(peak);

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
    options.add_option_function<std::vector<NoiseTuple>>("-n,--noise",
        [](std::vector<NoiseTuple> const &noises)
        {
            for (const auto &noise : noises) {
                g_synconvConfig.noises.push_back(noise);

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
    options.add_option_function<std::tuple<int, int, int, int>>("--bounds",
        [](std::tuple<int, int, int, int> const &bounds)
        {
            g_synconvConfig.bounds = bounds;
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
    ->description("Границы по азимуту и углу места")
    ->default_val(g_synconvConfig.bounds);
}

} // synconv
