/* compiler_instance.hpp */

#pragma once

#include "diagnostics_engine.hpp"
#include "module.hpp"

#include <vector>
#include <unordered_map>
#include <variant>

using ArgValue = std::variant<int, bool, std::string>;

enum class CompilerMode {
    Build,  // Builds the source code into binary format
    Run,    // Builds and executes the source code in a runtime engine
    Debug   // Builds and outputs debugging messages to stdout
};

/* */
struct CompilerArgs {
    CompilerMode mode = CompilerMode::Build;
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
    DiagnosticsEngine diagnostics_engine;

    std::vector<Module> modules;

public:
    [[nodiscard]] int run(int argc, char **argv, CompilerMode) noexcept;
    [[noreturn]] void stop(bool core_dump=false) noexcept; 

    inline CompilerArgs get_compiler_args() { return compiler_args; }
    inline DiagnosticsEngine get_diagnostics_engine() { return diagnostics_engine; }

    inline void set_compiler_args(CompilerArgs &args) { compiler_args = args; }
    inline void set_diagnostics_engine(DiagnosticsEngine &__diagnostics_engine) { diagnostics_engine = __diagnostics_engine; }
};