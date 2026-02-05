/* diagnostic_engine.hpp */

#pragma once

#include <string>
#include <vector>

enum class DiagnosticLevel {
    Hint,
    Warning,
    Error,
    FatalError,
    InternalError,
};

enum class SourceKind {
    Stdin,
    File,
};

struct SourceLocation {
    SourceKind source_kind = SourceKind::File;
    std::string file;
    std::string function_name;
    unsigned line = 1;
    unsigned column = 1;

    SourceLocation() = default;
    SourceLocation(SourceKind src_kind, const std::string &f, const std::string &func_name, 
        const unsigned ln, const unsigned col) :
        source_kind(src_kind), file(f), function_name(func_name), line(ln), column(col) {}
};

struct DiagnosticConfig {
    bool color_diagnostics = true;
    bool guide_engine = false;
    bool all_warnings = false;
    // More options soon
};

struct Diagnostic {
    SourceLocation loc;
    const std::string msg;
    const DiagnosticLevel level;
    const unsigned id;

    Diagnostic(const SourceLocation &location, const std::string &message, const DiagnosticLevel lvl, const unsigned ID) :
    loc(location), msg(message), level(lvl), id(ID) {}

    [[nodiscard]] static const char *severity_to_string(const DiagnosticLevel);
};

class DiagnosticEngine {
    unsigned warnings_num = 0;
    unsigned errors_num = 0;
    unsigned __actual_diag = 0;
    bool __has_errors = false;

    DiagnosticConfig config;
    std::vector<Diagnostic> diagnostics;

    void print_info(const Diagnostic&, const SourceKind&);
public:
    void report(const SourceLocation&, const std::string&, const DiagnosticLevel);
    void show() noexcept;
    void show_all() noexcept;

    [[noreturn]] void compiler_stop(bool=false) noexcept;

    inline int warnings_count() const noexcept { return warnings_num; }
    inline int error_count() const noexcept { return errors_num; }
    inline bool has_errors() const noexcept { return __has_errors; }

    // more methods
};