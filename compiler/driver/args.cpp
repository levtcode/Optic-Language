/* args.cpp */

#include "args.hpp"

#include <iostream>
#include <format>

const SourceLocation loc(SourceKind::Stdin, "", "", 0, 0);

/* */
void print_args_files(std::vector<std::string> &files) {
    std::cout << "Files:\n";
    for (const auto &f : files) {
        std::cout << "  " << f << "\n";
    }
}

/* */
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

/* */
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

/* */
void print_args(CompilerArgs &args) {
    std::cout << "----- PRINTING COMPILER ARGUMENTS -----\n\n";
    print_args_files(args.files);
    print_args_options(args.options);
    print_args_flags(args.flags);
}