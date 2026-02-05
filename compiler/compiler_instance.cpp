/* compiler_instance.cpp */

#include "compiler_instance.hpp"
#include "args.hpp"
#include "diagnostic_engine.hpp"
#include "preprocessor/preprocessor.hpp"

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <format>

#ifdef OPTIC_DEBUG

/* */
void print_combined_buffer(const std::vector<uint8_t> &output) noexcept {
    printf("----- PRINTING COMBINED BUFFER -----\n\n");

    for (size_t i = 0; i < output.size(); i++) {
        printf("%c", output[i]);
    }
    printf("\n");
}

#endif

/* */
bool get_data(const std::string fname, std::vector<uint8_t> &dest, DiagnosticEngine &diagnostic_engine) noexcept {
    const SourceLocation srcloc(SourceKind::Stdin, "", "", 0, 0);
    FILE *f = fopen(fname.c_str(), "rb");

    if (!f) {
        diagnostic_engine.report(
            srcloc,
            std::format("Error: Cant open file '{}'. File does not exists.\n", fname),
            DiagnosticLevel::Error
        );
        return false;
    }

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    dest.resize(size);
    if ((fread(dest.data(), sizeof(uint8_t), size, f) != size) && ferror(f)) {
        diagnostic_engine.report(
            srcloc,
            std::format("Error: Failed to read file '{}', IO error.\n", fname),
            DiagnosticLevel::Error
        );
        fclose(f);
        return false;
    }

    fclose(f);
    return true;
}

/* */
int CompilerInstance::run(int argc, char *argv[]) noexcept {
    CompilerContext compiler_context;

    get_args(argc, argv, *this);
    preprocess(compiler_context);

    diagnostic_engine.show_all();
    return (diagnostic_engine.has_errors()) ? EXIT_FAILURE : EXIT_SUCCESS;
}

/* */
[[noreturn]]
void CompilerInstance::stop(bool generate_core_dump) noexcept {
    diagnostic_engine.show_all();
    if (generate_core_dump)
        abort();
    else
        exit(1);
}

/* */
void CompilerInstance::preprocess(CompilerContext &compiler_context) noexcept {
    const SourceLocation srcloc(SourceKind::Stdin, "", "", 0, 0);

    std::unordered_map<std::string, PreprocessedModule> processed_modules;
    DependencyGraph graph;
    PreprocessorContext pr_ctx(&diagnostic_engine, &graph);
    Preprocessor preprocessor(pr_ctx);
    
    for (size_t i = 0; i < compiler_args.files.size(); i++) {
        std::string fname = compiler_args.files[i];
        std::vector<uint8_t> data;
        
        if (!fname.ends_with(optic_extension)) {
            diagnostic_engine.report(srcloc,
                std::format("Error: File '{}' is not a Optic file '{}', cant be processed.\n", fname, optic_extension),
                DiagnosticLevel::Error
            );
            continue;
        }

        if (!get_data(fname, data, diagnostic_engine)) continue;

        Module module(fname);

        if (!processed_modules.contains(module.module_name)) {
            processed_modules[module.module_name] = preprocessor.analyze(module.module_name, data);
        } else {
            diagnostic_engine.report(
                srcloc,
                std::format("Warning: This module is already processed: '{}'. Skipping this module.\n", module.module_name),
                DiagnosticLevel::Warning
            );
        }
    }

    preprocessor.sort_modules();

    std::vector<uint8_t> output;
    preprocessor.combine_modules(output, processed_modules);

#ifdef OPTIC_DEBUG
    preprocessor.print_modules();
    print_combined_buffer(output);
#endif
    compiler_context.combined_buffer = std::move(output);
}