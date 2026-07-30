/* core.cpp */

#include "core.hpp"
#include "command_manager.hpp"

#include <cstdlib>

/* */
void OpticCore::init_compiler() {
    // TODO
}

/* */
int OpticCore::run(int argc, char *argv[]) {
    if (argc < 2) {
        optic_cli::display_help(true);
        return EXIT_FAILURE;
    }

    optic_cli::state_codes_ _code_ = optic_cli::read_args(--argc, ++argv);
    if (_code_ != 0) {
        return optic_cli::match_code(_code_);
    }

    // Load Optic modules (compiler, runtime, etc...)

    return EXIT_SUCCESS;
}