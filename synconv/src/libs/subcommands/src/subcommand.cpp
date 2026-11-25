#include "subcommand.h"

#include "global.h"

namespace synconv
{

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

} // synconv