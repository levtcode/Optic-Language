/* compiler_instance.cpp */

#include "compiler_instance.hpp"
#include "args.hpp"
#include "diagnostic_engine.hpp"
#include "preprocessor/preprocessor.hpp"

#include <cstddef>
#include <cstdlib>
#include <float.h>

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

bool File::open(const char *fname, const char* mode) {
    fp = fopen(fname, mode);
    return fp != nullptr;
}

void File::close() noexcept {
    if (!fp) return;

    fclose(fp);
    pos = 0;
    buffer = "";
    reached_eof = false;
}

size_t File::read() {
    if (!fp) return 0;

    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    rewind(fp);

    buffer.resize(size);
    return fread(buffer.data(), 1, buffer.size(), fp);
}

File& File::operator=(const char* fname) {
    FILE* p = fopen(fname, "r");

    if (!p) {
        fclose(p);
        return *this;
    }

    fp = p;
    return *this;
}

File& File::operator=(const std::pair<const char *, const char *> &f) {
    FILE* p = fopen(f.first, f.second);

    if (!p) {
        fclose(p);
        return *this;
    }

    fp = p;
    return *this;
}

File& File::operator=(const File f) {
    if (!f.fp) return *this;

    this->fp = f.fp;
    this->pos = f.pos;
    this->reached_eof = f.reached_eof;
    this->buffer = f.buffer;
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
            std::string msg = "Error: Cant open file '";
            msg += fname;
            msg += "' file does not exists.\n";

            diagnostic_engine.report(
                SourceLocation(SourceKind::Stdin, fname, "", 0, 0),
                msg,
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

    if (graph.has_cycles()) {
        // TODO
    }
}