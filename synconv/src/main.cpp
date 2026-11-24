#include <iostream>

#include "CLI11.hpp"
#include "generatesubcommand.h"
#include "libs/subcommands/src/createconfigsubcommand.h"
#include "libs/subcommands/src/subcommand.h"

inline int g_verbosity = 0;

int main(const int argc, char** argv) {

    CLI::App app{"CLI-программа для создания синтетических свёрток. Made with love by russkiylis, ChatGPT Astra 6.2 & Gemini 4 Hydrogen, copyleft (ɔ) 2026"};
    argv = app.ensure_utf8(argv);
    app.require_subcommand(1);

    // Базовые флаги
    app.set_help_flag("-h,--help", "Показать эту справку и выйти")->group("Базовые флаги");
    app.set_version_flag("--version", "Версия самая лучшая: " APP_VERSION, "Отобразить версию программы и выйти")->group("Базовые флаги");
    app.add_flag("-v", g_verbosity, "Увеличить подробность вывода")->group("Базовые флаги");

    // Добавление субкомманд
    std::vector<std::unique_ptr<synconv::Subcommand>> subcommands;
    subcommands.push_back(std::make_unique<synconv::CreateConfigSubcommand>(&app));
    subcommands.push_back(std::make_unique<synconv::GenerateSubcommand>(&app));

    CLI11_PARSE(app, argc, argv);

    if (g_verbosity >= 1)
        std::cerr << "Установлен уровень подробности: " << g_verbosity << std::endl;

    return 0;
}
