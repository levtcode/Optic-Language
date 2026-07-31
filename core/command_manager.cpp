/* command_manager.cpp */

#include "command_manager.hpp"

#include <string_view>
using namespace optic_cli;

void optic_cli::display_help(bool is_error) {
    // TODO
}

/* */
void optic_cli::handle_signal(const CommandSignal signal) {
    switch (signal) {
    case (CommandSignal::ShowHelp):
        display_help(false);
        break;
    case (CommandSignal::ShowErrorHelp):
        display_help(true);
        break;
    }
}

bool optic_cli::match_arg(const std::string_view &arg) {
    return command_table.find(arg) != command_table.end();
}

/* */
CommandSignal optic_cli::read_args(const int argc, const char *const argv[]) {
    for (int i = 1; i < argc; i++) {
        const std::string_view arg = argv[i];

        if (match_arg(arg)) {
            // TODO
        } else {
            return CommandSignal::ShowErrorHelp;
        }
    }

    return CommandSignal::Continue;
}