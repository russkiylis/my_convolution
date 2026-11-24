#include "subcommand.h"

namespace synconv
{

Subcommand::Subcommand(CLI::App* app)
    : m_app(app)
{
}

} // synconv