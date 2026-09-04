/* core.cpp */

#include "core.hpp"
#include "../compiler/driver/args.hpp"
#include "commands/source.hpp"

#include <cstdlib>
#include <array>

/* */
int OpticCore::dispatch_command(Command command, int argc, char **argv) {
    if (command == Command::Help) {
        call_usage();
        return EXIT_SUCCESS;
    }

    constexpr std::array<command_handler, static_cast<size_t>(Command::Count)> HANDLERS = {
        test_,
        install_,
        uninstall_,
        build_,
        run_,
        debug_
    };

    auto index = static_cast<size_t>(command);
    
    if (index < HANDLERS.size() && HANDLERS[index] != nullptr) {
        return HANDLERS[index](argc, argv);
    }

    return EXIT_FAILURE;
}

OpticCore::Command OpticCore::get_command(const char *current_arg) {
    auto it = command_table.find(current_arg);
    return (it != command_table.end()) ? it->second : Command::Count;
}

/* */
int OpticCore::run(int argc, char *argv[]) {
    Command main_command = get_command(argv[0]);

    if (main_command == Command::Count) {
        return EXIT_FAILURE;
    }

    return dispatch_command(main_command, argc, argv);
}