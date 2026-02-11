/* diagnostic_engine.cpp */

#include "diagnostic_engine.hpp"

#include <iostream>
#include <format>

/* */
const char* Diagnostic::severity_to_string(const DiagnosticLevel level) {
    switch (level) {
        case DiagnosticLevel::Hint: return "Hint";
        case DiagnosticLevel::Warning: return "Warning";
        case DiagnosticLevel::Error: return "Error";
        case DiagnosticLevel::FatalError: return "FatalError";
        case DiagnosticLevel::InternalError: return "InternalError";
    }
    return "Unknown";
}

/* */
void DiagnosticEngine::print_info(const Diagnostic &diag, const SourceKind &src_kind) noexcept {
    if (src_kind == SourceKind::Stdin) {
        std::cerr << std::format(
            "From <stdin>:\n" \
            "{}\n",
            diag.msg
        );
        return;
    }

    else if (src_kind == SourceKind::BufferNoAST) {
        std::cerr << std::format(
            "At line: {}, in column: {}\n" \
            "{}\n" \
            "In file: {}\n",
            diag.loc.line, diag.loc.column,
            diag.msg, diag.loc.file
        );
        return;
    }

    std::cerr << std::format(
        "On function: {}, at line: {}, in column: {}\n" \
        "{}\n" \
        "In file: {}\n",
        diag.loc.function_name, diag.loc.line, diag.loc.column,
        diag.msg, diag.loc.file
    );
}

/* */
std::string DiagnosticEngine::format_msg(const std::string &msg, const std::string &buffer_view, const unsigned line, const unsigned column, const std::string &pointer) noexcept {
    std::string padding(std::to_string(line).length(), ' ');
    std::string error_mark = std::format("{:>{}}", pointer, column);

    return std::format(
        "{}\n" \
        "{} |\n" \
        "{} |      {}\n" \
        "{} |      {}\n" \
        "{} |\n",
        msg,
        padding,
        line, buffer_view,
        padding, error_mark,
        padding
    );
}

/* */
void DiagnosticEngine::report(
    const SourceLocation &loc,
    const std::string &message,
    const std::string &solution,
    const std::string &more_info,
    const DiagnosticLevel lvl
) {
    diagnostics.emplace_back(loc, message, solution, more_info, lvl);

    if (lvl != DiagnosticLevel::Hint) {
        if (lvl == DiagnosticLevel::Warning) warnings_num++;
        else { errors_num++; __has_errors = true; }
    }
}

/* */
void DiagnosticEngine::show() noexcept {
    auto &diag = diagnostics[__actual_diag];
    print_info(diag, diag.loc.source_kind);
    __actual_diag++;
}

/* */
void DiagnosticEngine::show_all() noexcept {
    for (auto &diag : diagnostics) {
        print_info(diag, diag.loc.source_kind);
    }
}

/* */
void DiagnosticEngine::show_guide_engine_menu() noexcept {
    int user_option;

    while (true) {
        std::cout << "\nWhat do you want to do?:\n";
        std::cout << "1. Propose a solution.\n";
        std::cout << "2. More information.\n";
        std::cout << "3. Next diagnostic.\n";
        std::cout << "4. Skip and stop compilation\n\n";

        std::cout << "Select an option (1-4): ";

        if (!(std::cin >> user_option)) {
            std::cerr << "Error: The option must be a integer value.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        if (user_option < 1 || user_option > 4) {
            std::cerr << "Error: The option must be in the range [1-4].\n";
            continue;
        }
        
        break;
    }
    
    switch (user_option) {
        case 1: {
            if (diagnostics[__actual_diag].solution.empty()) {
                std::cout << "There is no solution available to display.\n";
                break;
            }
            std::cout << diagnostics[__actual_diag].solution;
            break;
        }
        case 2: {
            if (diagnostics[__actual_diag].more_info.empty()) {
                std::cout << "There is no more information available to display.\n";
                break;
            }
            std::cout << diagnostics[__actual_diag].more_info;
            break;
        }
        case 3: return;
        case 4:
            __actual_diag = diagnostics.size();
            return;
    }
}

/* */
void DiagnosticEngine::run_guide_engine() noexcept {
    // MUCH LATER: Run the guide engine in a dedicated GUI or use specific graphics to show the diagnostics.

    while (__actual_diag < diagnostics.size()) {
        show();
        show_guide_engine_menu();
    }
}