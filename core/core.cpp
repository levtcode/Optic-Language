/* core.cpp */

#include "core.hpp"
#include "command_dispatcher.hpp"

#include <cstdlib>

/* */
void OpticCore::init_compiler() {
    // TODO
}

/* */
int OpticCore::run(int argc, char *argv[]) {
    OpticCLI optic_cli(argc, argv);
    if (optic_cli.bad_()) {
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}