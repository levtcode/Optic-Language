/* compiler.cpp */

#include "compiler_instance.hpp"

#ifdef OPTIC_DEBUG
// #include <chrono>
#endif

int main(int argc, char *argv[]) {
    CompilerInstance compiler_instance;
    return compiler_instance.run(argc, argv);
}