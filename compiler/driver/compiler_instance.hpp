/* compiler_instance.hpp */

#pragma once

#include "diagnostics_engine.hpp"
#include "module.hpp"

#include <vector>
#include <unordered_map>
#include <variant>

using ArgValue = std::variant<int, bool, std::string>;

enum class CompilerMode {
    Build,  // Only builds the source file into binary code
    Run,    // Build and executes in a runtime engine the source code
    Debug   // Build and prints debugging messages to the stdout
};

/* */
struct CompilerArgs {
    CompilerMode mode;
    std::vector<std::string> files;

    std::unordered_map<std::string, ArgValue> options = {
        {"--color_diagnostics", "auto"},    // [auto, enable, disable] default=auto
        // more options soon
    };

    std::unordered_map<std::string, bool> flags = {
        {"-Wall", false},
        {"-g", false},
        // more flags soon
    };
};

/* */
struct TargetInfo {
    // TODO
};

/* */
class CompilerInstance {
private:
    void lexing() noexcept;
    void preprocess() noexcept;
    void parsing() noexcept;
    // more methods soon

    CompilerArgs compiler_args;
    DiagnosticsEngine diagnostic_engine;

    std::vector<Module> modules;

public:
    [[nodiscard]] int run(int argc, char **argv, CompilerMode) noexcept;
    [[noreturn]] void stop(bool=false) noexcept; 

    inline CompilerArgs &get_compiler_args() { return compiler_args; }
    inline DiagnosticsEngine &get_diagnostic_engine() { return diagnostic_engine; }
};