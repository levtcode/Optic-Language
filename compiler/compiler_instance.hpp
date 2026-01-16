/* compiler_instance.hpp */

#pragma once
#include "diagnostic_engine.hpp"

#include <vector>
#include <unordered_map>
#include <variant>

using ArgValue = std::variant<int, bool, std::string>;

struct CompilerArgs {
    std::vector<std::string> files;

    std::unordered_map<std::string, ArgValue> options = {
        {"--create_disk_binary", false},
        {"--color_diagnostics", true},
        // more options soon
    };

    std::unordered_map<std::string, bool> flags = {
        {"-g", false},
        // more flags soon
    };
};

struct TargetInfo {
    // TODO
};

class File {
    std::string buffer;
    FILE *fp;
    size_t pos = 0;
    bool reached_eof = false;

public:
    File() = default;

    File(const char *fname, const char* mode) {
        open(fname, mode);
        read();
    }

    int get() noexcept;
    inline void unget() noexcept { --pos; }

    inline bool is_open() const noexcept { return fp != nullptr; }
    inline bool iseof() const noexcept { return reached_eof; }

    bool seek(const size_t&) noexcept;
    bool open(const char *, const char *);

    void reset() noexcept;
    void close() noexcept;

    inline size_t get_pos() const noexcept { return pos; }
    size_t read();

    File& operator=(const char*);
    File& operator=(const std::pair<const char *, const char *>&);
    File& operator=(const File&);
};

class CompilerInstance {
    void preprocess();
    void lexing();
    void parsing();
    // more methods soon

public:
    [[nodiscard]] int run(int, char **);

    CompilerArgs compiler_args;
    DiagnosticEngine diagnostic_engine;
    // TargetInfo target_info;
};