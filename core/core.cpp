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

    
    return EXIT_SUCCESS;
}