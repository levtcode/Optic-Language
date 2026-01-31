/* compiler_instance.cpp */

#include "compiler_instance.hpp"
#include "args.hpp"
#include "diagnostic_engine.hpp"
#include "preprocessor/preprocessor.hpp"

#include <cstddef>
#include <cstdlib>
#include <format>

// ========== Definitions for class methods ==========

int File::get() noexcept{
    if (pos >= buffer.size()) {
        reached_eof = true;
        return EOF;
    }
    return buffer[pos++];
}

bool File::seek(const size_t &__pos) noexcept {
    if (__pos >= buffer.size()) {
        return false;
    }
    pos = __pos;
    return true;
}

bool File::open(const char *fname, const char* mode) noexcept {
    FilePtr p(fopen(fname, mode), fclose);
    if (!p) return false;
    reset();
    fp = std::move(p);
    return true;
}

void File::close() noexcept {
    if (fp) {
        fclose(fp.release());
    }
    reset();
}

void File::reset() noexcept {
    buffer = "";
    pos = 0;
    reached_eof = 0;
}

size_t File::read() {
    if (!fp) return 0;

    fseek(fp.get(), 0, SEEK_END);
    size_t size = ftell(fp.get());
    rewind(fp.get());

    buffer.resize(size);
    return fread(buffer.data(), 1, buffer.size(), fp.get());
}

File& File::operator=(const char* fname) {
    if (!open(fname, "r")) return *this;
    return *this;
}

File& File::operator=(const std::pair<const char *, const char *> &f) {
    open(f.first, f.second);
    return *this;
}

File& File::operator=(File &&other) noexcept {
    if (this != &other) {
        close();
        this->buffer = other.buffer;
        this->pos = other.pos;
        this->reached_eof = other.reached_eof;
        this->fp = std::move(other.fp);
    }

    return *this;
}

// ========== Compiler phases ==========

int CompilerInstance::run(int argc, char *argv[]) {
    get_args(argc, argv, *this);
    preprocess();

    diagnostic_engine.show_all();
    return (diagnostic_engine.has_errors()) ? EXIT_FAILURE : EXIT_SUCCESS;
}

void CompilerInstance::preprocess() {
    DependencyGraph graph;
    
    for (size_t i = 0; i < compiler_args.files.size(); i++) {
        const char *fname = compiler_args.files[i].c_str();
        File file(fname, "r");

        if (!file.is_open()) {
            diagnostic_engine.report(
                SourceLocation(SourceKind::Stdin, "", "", 0, 0),
                std::format("Error: Cant open file '{}' file does not exists.\n", fname),
                DiagnosticLevel::Error
            );
            continue;
        }

        if (!file.read()) {
            diagnostic_engine.report(
                SourceLocation(SourceKind::Stdin, "", "", 0, 0), 
                std::format("Error: Failed to read file '{}', IO error.\n", fname),
                DiagnosticLevel::Error
            );
            continue;
        }

        Preprocessor preprocessor(Module(fname), graph);
        preprocessor.analyze(file.data());
    }

#ifdef OPTIC_DEBUG
    graph.print_nodes();
#endif
}