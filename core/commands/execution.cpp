/* execution.cpp */

#include "source.hpp"
#include "../../compiler/driver/compiler_instance.hpp"

int build_(int argc, char **argv) {
    CompilerInstance compiler;
    return compiler.run(argc, argv);
}

int run_(int argc, char **argv) {
    (void) argc;
    (void) argv;
    return EXIT_FAILURE;
}

int debug_(int argc, char **argv) {
    (void) argc;
    (void) argv;
    return EXIT_FAILURE;
}