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
class CompilerArgs {
private:
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

    friend class CompilerInstance;

public:
    void set_mode  (const CompilerMode mode);
    void add_file  (const std::string str);
    void set_option(const std::string option_name, const ArgValue val);
    void set_flag  (const std::string flag_name, const bool val);

    bool                     get_flag_value(const std::string flag_name);
    CompilerMode             get_compiler_mode();
    ArgValue                 get_option_value(const std::string option_name);
    std::vector<std::string> get_files();
};

/* */
struct TargetInfo {
    // TODO
};

/* */
class CompilerInstance {
private:
    void lexing()     noexcept;
    void preprocess() noexcept;
    void parsing()    noexcept;
    // more methods soon

    CompilerArgs        compiler_args;
    DiagnosticsEngine   diagnostics_engine;
    std::vector<Module> modules;

public:
    [[nodiscard]] int run(int argc, char **argv, CompilerMode) noexcept;
    [[noreturn]] void stop(bool core_dump=false) noexcept;
};