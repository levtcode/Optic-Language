/* diagnostic_engine.cpp */

#include "diagnostic_engine.hpp"

#include <iostream>
#include <format>

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

void DiagnosticEngine::print_info(const Diagnostic &diag, const SourceKind &src_kind) {
    if (src_kind == SourceKind::Stdin) {
        std::cerr << std::format(
            "From <stdin>:\n" \
            "{}" \
            "With a severity of: {}, whose level: {}\n\n",
            diag.msg, Diagnostic::severity_to_string(diag.level), (int) diag.level
        );
        return;
    }

    std::cerr << std::format(
        "On function: {}, at line: {}, in column: {}\n" \
        "{}\n" \
        "In file: {}\n" \
        "With a severity of: {}, whose level: {}\n\n",
        diag.loc.file, diag.loc.line, diag.loc.column,
        diag.msg, diag.loc.file, Diagnostic::severity_to_string(diag.level),
        (int) diag.level
    );
}

void DiagnosticEngine::report(const SourceLocation &loc, const std::string &message, const DiagnosticLevel lvl) {
    diagnostics.emplace_back(loc, message, lvl, diagnostics.size());

    if (lvl != DiagnosticLevel::Hint) {
        if (lvl == DiagnosticLevel::Warning) warnings_num++;
        else errors_num++; __has_errors = true;
    }
}

void DiagnosticEngine::show() noexcept {
    for (auto &diag : diagnostics) {
        if (diag.id == __actual_diag) {
            print_info(diag, diag.loc.source_kind);
            break;
        }
    }
    __actual_diag++;
}

void DiagnosticEngine::show_all() noexcept {
    for (auto &diag : diagnostics) {
        print_info(diag, diag.loc.source_kind);
    }
}

[[noreturn]] void DiagnosticEngine::compiler_stop(bool generate_core_dump) noexcept {
    show_all();
    if (generate_core_dump)
        abort();
    else
        exit(1);
}