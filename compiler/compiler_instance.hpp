/* compiler_instance.hpp */

#pragma once
#include "diagnostic_engine.hpp"

#include <vector>
#include <unordered_map>
#include <variant>
#include <cstdint>

constexpr const char *optic_extension = ".optic";
using ArgValue = std::variant<int, bool, std::string>;

/* */
struct CompilerArgs {
    std::vector<std::string> files;

    std::unordered_map<std::string, ArgValue> options = {
        {"--color_diagnostics", "auto"}, // [auto, enable, disable]
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
struct CompilerContext {
    std::vector<uint8_t> combined_buffer;
};

/* */
class CompilerInstance {
    void preprocess(CompilerContext&) noexcept;
    void lexing();
    void parsing();
    // more methods soon

public:
    [[nodiscard]] int run(int, char **) noexcept;
    [[noreturn]] void stop(bool=false) noexcept; 

    CompilerArgs compiler_args;
    DiagnosticEngine diagnostic_engine;
    // TargetInfo target_info;
};