/* compiler_instance.cpp */

#include "compiler_instance.hpp"
#include "args.hpp"
// #include "preprocessor/preprocessor.hpp"

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
    
    if (!fp) {
        return false;
    }

    return true;
}

void File::close() noexcept {
    if (!fp) {
        return;
    }

    fclose(fp);
    pos = 0;
    buffer = "";
    reached_eof = false;
}

size_t File::read() {
    if (!fp) {
        return 0;
    }

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
        delete fname;
        return *this;
    }

    fp = p;
    return *this;
}

File& File::operator=(const std::pair<const char *, const char *> &f) {
    FILE* p = fopen(f.first, f.second);

    if (!p) {
        delete[] f.first;
        delete[] f.second;
        fclose(p);
        return *this;
    }

    fp = p;
    return *this;
}

File& File::operator=(const File& f) {
    if (!f.fp) return *this;

    fp = f.fp;
    buffer = f.buffer;
    pos = f.pos;
    reached_eof = f.reached_eof;
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
    // TODO
}