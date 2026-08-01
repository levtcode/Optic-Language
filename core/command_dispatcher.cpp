/* command_dispatcher.cpp */

#include "command_dispatcher.hpp"


/* */
void OpticCLI::display_help(bool is_error) {
    flag = is_error;
    // TODO
}

/* */
void OpticCLI::dispatch_task(Command command) {
    switch (command) {
    case (Command::Help):
        display_help(false);
        break;
    case (Command::Test):
        // TODO
        break;
    case (Command::Install):
        // TODO
        break;
    case (Command::Uninstall):
        // TODO
        break;
    case (Command::Build):
        // TODO
        break;
    case (Command::Run):
        // TODO
        break;
    }
}

/* */
void OpticCLI::_init(int argc, char *argv[]) {
    if (argc < 2) {
        display_help(true);
        return;
    }

    const char *main_command = argv[0];
    const auto _arg = command_table.find(main_command);

    if (_arg != command_table.end()) {
        dispatch_task(_arg->second);
    } else {
        display_help(true);
        return;
    }
}