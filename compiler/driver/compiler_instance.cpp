/* compiler_instance.cpp */

#include "compiler_instance.hpp"
#include "args.hpp"
#include "diagnostics_engine.hpp"
#include "../preprocessor/preprocessor.hpp"
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

/* */
[[nodiscard]]
int CompilerInstance::run(int argc, char *argv[]) noexcept {
    get_args(argc, argv, *this);
    lexing();
    preprocess();

    // More phases soon...

    diagnostic_engine.get_config().guide_engine ? diagnostic_engine.run_guide_engine() : diagnostic_engine.show_all();
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
void CompilerInstance::preprocess() noexcept {
    // TODO
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
            diagnostic_engine.report(srcloc,
                std::format("Error: File '{}' is not a Optic file '{}', cant be processed.\n", fname, optic_extension),
                "",
                "",
                DiagnosticsLevel::Error
            );
            continue;
        }

        Module module(fname);
        if (!get_data(fname, module.get_buffer(), diagnostic_engine)) continue;

        Lexer lexer(&module, &diagnostic_engine);
        lexer.tokenize(module.get_tokens());
        
    #ifdef OPTIC_DEBUG
        lexer.print_tokens(module);
    #endif
        modules.push_back(std::move(module));
    }
}