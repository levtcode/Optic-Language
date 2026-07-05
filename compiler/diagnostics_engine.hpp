/* diagnostic_engine.hpp */

// MORE LATER: Create DiagnosticRange or similar, to measure the distance between 2 SourceLocation instances
// MORE LATER: Fix the format_msg() logic. Make that print renders '^~~~~'

#pragma once

#include <string>
#include <vector>

/* */
enum class DiagnosticsLevel {
    Hint,
    Warning,
    Error,
    FatalError,
    InternalError,
};

/* */
enum class SourceKind {
    Stdin,
    BufferAST,
    BufferNoAST,
};

/* */
struct SourceLocation {
    SourceKind source_kind = SourceKind::BufferNoAST;
    std::string file;
    std::string function_name;
    unsigned line = 1;
    unsigned column = 1;

    SourceLocation() = default;
    SourceLocation(SourceKind src_kind, const std::string &f, const std::string &func_name, 
        const unsigned ln, const unsigned col) :
        source_kind(src_kind), file(f), function_name(func_name), line(ln), column(col) {}
};

/* */
class SourceLocationRange {
    // TODO
};

/* */
struct DiagnosticsConfig {
    bool color_diagnostics = true;
    bool guide_engine = false;
    bool all_warnings = false;
    // More options soon
};

/* */
struct Diagnostic {
    SourceLocation loc;
    const std::string msg;
    const std::string solution;
    const std::string more_info;
    const DiagnosticsLevel level;

    Diagnostic(const SourceLocation &location,
        const std::string &message,
        const std::string &sol,
        const std::string &info,
        const DiagnosticsLevel lvl
    ) : loc(location), msg(message), solution(sol), more_info(info), level(lvl) {}

    [[nodiscard]] static const char *severity_to_string(const DiagnosticsLevel);
};

/* */
class DiagnosticsEngine {
private:
    unsigned warnings_num = 0;
    unsigned errors_num = 0;
    unsigned __actual_diag = 0;
    bool __has_errors = false;

    DiagnosticsConfig config;
    std::vector<Diagnostic> diagnostics;

    void print_info(const Diagnostic&, const SourceKind&) noexcept;
    void show_guide_engine_menu() noexcept;

public:
    void report(const SourceLocation&, const std::string &msg, const std::string &sol, const std::string &info, const DiagnosticsLevel);
    void show() noexcept;
    void show_all() noexcept;
    void run_guide_engine() noexcept;

    inline int warnings_count() const noexcept { return warnings_num; }
    inline int error_count() const noexcept { return errors_num; }
    inline bool has_errors() const noexcept { return __has_errors; }

    DiagnosticsConfig get_config() const noexcept { return config; }
    std::string format_msg(const std::string &msg, const std::string &buffer_view, const unsigned ln, const unsigned col, const std::string &pointer) noexcept;
    // more methods
};