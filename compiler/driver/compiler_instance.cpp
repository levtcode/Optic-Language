/* compiler_instance.cpp */

#include "compiler_instance.hpp"
#include "args.hpp"
#include "diagnostics_engine.hpp"
#include "preprocessor/preprocessor.hpp"
#include "lexer/lexer.hpp"

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <format>

/* */
bool get_data(const std::string fname, std::string &dest, DiagnosticsEngine &diagnostic_engine) noexcept {
    const SourceLocation srcloc(SourceKind::Stdin, "", "", 0, 0);
    FILE *f = fopen(fname.c_str(), "rb");

    if (!f) {
        diagnostic_engine.report(
            srcloc,
            std::format("Error: Cant open file '{}'. File does not exists.\n", fname),
            "Solution: Check if the file exists on your system path, if doesnt, create the file yourself.\n",

            "More information: This happens because the OS (Operating System) tries to access to the " \
            "specified path, but the resource does not exists in that path. So, the OS returns a null pointer " \
            "to that resource, causing it to be imposible to read or write in that resource.\n",
            DiagnosticsLevel::Error
        );
        fclose(f);
        return false;
    }

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    dest.resize(size);
    if ((fread(dest.data(), 1, size, f) != size) && ferror(f)) {
        diagnostic_engine.report(
            srcloc,
            std::format("Error: Failed to read file '{}', IO error.\n", fname),
            "Solution: Verify if your file contains a valid format (UTF-8 for example...) and is not a binary file.\n",
            "More information: ...",
            DiagnosticsLevel::Error
        );
        fclose(f);
        return false;
    }

    fclose(f);
    return true;
}

void CompilerInstance::call_usage(bool help_manual) noexcept {
    (void) help_manual;
}

inline void CompilerInstance::set_mode(const CompilerMode mode) {
    compiler_args.mode = mode;
}

inline void CompilerInstance::add_file(const std::string file) {
    modules.push_back(file);
}

void CompilerInstance::set_option(const std::string option_name, const ArgValue value) {
    (void) option_name;
    (void) value;
}

void CompilerInstance::set_flag(const std::string flag_name, const bool val) {
    (void) flag_name;
    (void) val;
}

int CompilerInstance::get_flag_value(const std::string flag_name) {
    (void) flag_name;
    return -1;
}

inline CompilerMode CompilerInstance::get_compiler_mode() {
    return compiler_args.mode;
}

ArgValue CompilerInstance::get_option_value(const std::string option_name) {
    (void) option_name;
    return -1;
}

inline std::vector<Module> CompilerInstance::get_files() {
    return modules;
}

/* */
[[noreturn]]
void CompilerInstance::stop(bool generate_core_dump) noexcept {
    diagnostics_engine.show_all();
    if (generate_core_dump)
        abort();
    else
        exit(1);
}

/* */
void CompilerInstance::cli_tooling(int argc, char **argv) noexcept {
    (void) argc;
    (void) argv;
}

/* */
void CompilerInstance::lexing() noexcept {
    const SourceLocation srcloc(SourceKind::Stdin, "", "", 0, 0);

#ifdef OPTIC_DEBUG
    printf("\n----- PRINTING TOKENS -----\n\n");
#endif

    for (size_t i = 0; i < compiler_args.files.size(); i++) {
        std::string fname = compiler_args.files[i];

        if (!fname.ends_with(optic_extension)) {
            diagnostics_engine.report(srcloc,
                std::format("Error: File '{}' is not a Optic file '{}', cant be processed.\n", fname, optic_extension),
                "",
                "",
                DiagnosticsLevel::Error
            );
            continue;
        }

        Module module(fname);
        if (!get_data(fname, module.get_buffer(), diagnostics_engine)) continue;

        Lexer lexer(&module, &diagnostics_engine);
        lexer.tokenize(module.get_tokens());

    #ifdef OPTIC_DEBUG
        lexer.print_tokens(module);
    #endif
        modules.push_back(std::move(module));
    }
}

/* */
void CompilerInstance::preprocess() noexcept {
    // TODO
}

/* */
void CompilerInstance::parsing() noexcept {
    // TODO
}

/* */
void CompilerInstance::semantic_analize() noexcept {
    // TODO
}

/* */
void CompilerInstance::optimize() noexcept {
    // TODO
}

/* */
void CompilerInstance::linking() noexcept {
    // TODO
}

/* */
void CompilerInstance::code_generation() noexcept {
    // TODO
}

/* */
[[nodiscard]]
int CompilerInstance::run(int argc, char *argv[], CompilerMode mode) noexcept {
    set_mode(mode);

    cli_tooling(argc, argv);
    lexing();
    preprocess();
    parsing();
    semantic_analize();
    optimize();
    linking();
    code_generation();

    diagnostics_engine.get_config().guide_engine ? diagnostics_engine.run_guide_engine() : diagnostics_engine.show_all();
    return (diagnostics_engine.has_errors()) ? EXIT_FAILURE : EXIT_SUCCESS;
}