/* build.cpp */

#include "source.hpp"
#include "../../compiler/driver/compiler_instance.hpp"

int build_(int argc, char **argv) {
    CompilerInstance compiler;
    return compiler.run(argc, argv);
}