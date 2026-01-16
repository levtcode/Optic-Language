/* args.cpp */

#include "args.hpp"

#include <iostream>
#include <format>

const SourceLocation loc(SourceKind::Stdin, "", "", 0, 0);

#ifdef OPTIC_DEBUG

void print_args_files(std::vector<std::string> &files) {
    std::cout << "Files:\n";
    for (const auto &f : files) {
        std::cout << "  " << f << "\n";
    }
}

void print_args_options(std::unordered_map<std::string, ArgValue> &options) {
    std::cout << "\nCompiler options:\n";
    for (const auto &[key, value] : options) {
        std::cout << "  " << key << " = ";

        std::visit([](const auto &v) {
            using T = std::decay_t<decltype(v)>;

            if constexpr (std::is_same_v<T, bool>) {
                std::cout << (v ? "true" : "false");
            } else {
                std::cout << v;
            }
        }, value);

        std::cout << "\n";
    }
}

void print_args_flags(std::unordered_map<std::string, bool> &flags) {
    std::cout << "\nCompiler flags:\n";
    for (const auto &[key, value] : flags) {
        std::cout << "  " << key << " -> ";

        if (value == true) {
            std::cout << "on";
        } else {
            std::cout << "off";
        }

        std::cout << "\n";
    }
    std::cout << "\n";
}

void print_args(CompilerArgs &args) {
    print_args_files(args.files);
    print_args_options(args.options);
    print_args_flags(args.flags);
}

#endif

/* */
ArgValue get_arg_value(const std::string &raw, int index, bool *invalid, DiagnosticEngine &diagnostic_engine) {
    std::string value = raw.substr(index + 1);

    if (value == "true" || value == "false") return value == "true";

    try {
        size_t pos;
        int n = std::stoi(value, &pos);
        if (pos == value.size()) return n;
    } catch (...) {}

    if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
        return value.substr(1, value.size() - 2);
    }

    diagnostic_engine.report(loc, std::format("Error: Invalid argument type: {}\n", value), DiagnosticLevel::Error);
    *invalid = true;
    return "";
}

/* */
std::string get_option_name(const std::string &arg, size_t *__index) {
    size_t index = 0;

    while (index < arg.size() && arg[index] != '=') index++; *__index = index;
    return arg.substr(0, index);
}

/* process_arg: In each iteration in get_args function, this function will process the argument */
void process_arg(const std::string &arg, CompilerInstance &compiler_instance) {
    size_t index;
    std::string option_name = get_option_name(arg, &index);

    if (compiler_instance.compiler_args.options.contains(option_name)) {
        bool is_invalid = false;
        ArgValue value = get_arg_value(arg, index, &is_invalid, compiler_instance.diagnostic_engine);

        if (is_invalid) return;

        compiler_instance.compiler_args.options[option_name] = value;
    }

    else if (compiler_instance.compiler_args.flags.contains(option_name)) {
        compiler_instance.compiler_args.flags[option_name] = !compiler_instance.compiler_args.flags.at(option_name);
    }

    else if (option_name == "--help") {
        call_usage(true);
    }

    else {
        compiler_instance.compiler_args.files.push_back(option_name);
    }
}

/* get_args: reads arguments from the command line and try to parse them to the compiler args */
void get_args(int argc, char *argv[], CompilerInstance &compiler_instance) {
    if (argc < 2) {
        compiler_instance.diagnostic_engine.report(
            loc,
            "Error: Not enough arguments provided in the command line\n",
            DiagnosticLevel::FatalError
        );
        compiler_instance.diagnostic_engine.compiler_stop();
    }

    int index = 1;

    while (index < argc) {
        process_arg(argv[index++], compiler_instance);
    }

#ifdef OPTIC_DEBUG
    print_args(compiler_instance.compiler_args);
#endif
}

/* call_usage: if something failed during parsing, then, usage method will be called to show the correct usage */
void call_usage(bool help_manual) {
    if (help_manual) {
        // TODO
    } else {
        std::cerr << "Usage: ..."; // TODO ...
    }
}