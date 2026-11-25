#include <iostream>

#include "CLI11.hpp"
#include "generatesubcommand.h"
#include "createconfigsubcommand.h"
#include "subcommand.h"
#include "global.h"

int main(const int argc, char** argv) {

    CLI::App app{"CLI-программа для создания синтетических свёрток. Made with love by russkiylis, ChatGPT Astra 6.2 & Gemini 4 Hydrogen, copyleft (ɔ) 2026"};
    argv = app.ensure_utf8(argv);
    app.require_subcommand(1);

    // Базовые флаги
    app.set_help_flag("-h,--help", "Показать эту справку и выйти")->group("Базовые флаги");
    app.set_version_flag("--version", "Версия самая лучшая: " APP_VERSION, "Отобразить версию программы и выйти")->group("Базовые флаги");
    app.add_flag_function("-v,--verbose",
    [](const int value)
    {
        g_verbosity = value;
        std::cerr << "Подробность вывода изменена на: " << value << std::endl;
    },
    "Увеличить подробность вывода"
    )->group("Базовые флаги");

    // Добавление субкомманд
    std::vector<std::unique_ptr<synconv::Subcommand>> subcommands;
    subcommands.push_back(std::make_unique<synconv::CreateConfigSubcommand>(&app));
    subcommands.push_back(std::make_unique<synconv::GenerateSubcommand>(&app));

    const auto createConfigSubcommand = dynamic_cast<synconv::CreateConfigSubcommand *>(subcommands[0].get());
    const auto generateSubcommand = dynamic_cast<synconv::GenerateSubcommand *>(subcommands[1].get());

    CLI11_PARSE(app, argc, argv);

    // Дополнительные проверки шума
    for (auto &noise : createConfigSubcommand->config().noises) {

        if (g_verbosity > 2) {
            std::cerr << "Проверка шума [тип:матожидание/минимум:СКО/максимум]:["
            << std::get<0>(noise) << ":"
            << std::get<1>(noise) << ":"
            << std::get<2>(noise) << "]"
            << std::endl;
        }

        if (const auto& [noiseType, arg1, arg2] = noise; noiseType == "normal") {
            if (arg2 < 0)
                throw CLI::ValidationError("-n", "СКО не может быть меньше 0!");
        } else if (noiseType == "uniform") {
            if (arg2 < arg1)
                throw CLI::ValidationError("-n", "Минимум шума не может быть больше максимума!");
        } else {
            throw CLI::ValidationError("-n", "Неправильный тип шума!");
        }
    }

    // Дополнительные проверки границ
    const auto& bounds = createConfigSubcommand->config().bounds;
    if (g_verbosity > 2) {
        std::cerr << "Проверка границ [minАзимут:maxАзимут:minУМ:maxУМ]:["
        << std::get<0>(bounds) << ":"
        << std::get<1>(bounds) << ":"
        << std::get<2>(bounds) << ":"
        << std::get<3>(bounds) << "]"
        << std::endl;
    }

    const auto& [minBearing, maxBearing, minAngle, maxAngle] = bounds;
    if (minBearing > maxBearing)
        throw CLI::ValidationError("--bounds", "Минимальный азимут не может быть больше максимального!");
    if (minAngle > maxAngle)
        throw CLI::ValidationError("--bounds", "Минимальный угол места не может быть больше максимального!");

    if (createConfigSubcommand->subApp()->parsed()) {
        std::ofstream out("file.toml");
        // Вызываем config_to_str именно у init_cmd, чтобы не было секции [init]
        out << createConfigSubcommand->subApp()->config_to_str(true, true);

        if (g_verbosity > 1) {
            std::cerr << "Файл конфигурации сохранён." << std::endl;
        }
    }

    return 0;
}
