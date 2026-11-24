#include <iostream>

#include "CLI11.hpp"
#include "generatesubcommand.h"
#include "libs/subcommands/src/createconfigsubcommand.h"
#include "libs/subcommands/src/subcommand.h"

int main(const int argc, char** argv) {

    CLI::App app{"CLI-программа для создания синтетических свёрток. Made with love by russkiylis, ChatGPT Astra 6.2 & Gemini 4 Hydrogen, copyleft (ɔ) 2026"};
    argv = app.ensure_utf8(argv);
    app.require_subcommand(1);

    std::vector<std::unique_ptr<synconv::Subcommand>> subcommands;
    subcommands.push_back(std::make_unique<synconv::CreateConfigSubcommand>(&app));
    subcommands.push_back(std::make_unique<synconv::GenerateSubcommand>(&app));

    CLI11_PARSE(app, argc, argv);

    return 0;
}
