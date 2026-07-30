/* command_manager.cpp */

#include "command_manager.hpp"

#include <string>
using namespace optic_cli;

/* */
void optic_cli::display_help(bool is_error) {
    // TODO
}

/* */
bool optic_cli::match_code(state_codes_ _code_) {
    switch (_code_) {
        case HELP_SIGNAL:
            display_help(false);
            return false;
    }
}

/* */
state_codes_ optic_cli::read_args(const int argc, const char *const argv[]) {
    for (int i = 1; i < argc; i++) {
        const std::string arg = argv[i];

        if (arg == "help" || arg == "--help") {
            display_help(false);
            return HELP_SIGNAL;
        }
    }
}