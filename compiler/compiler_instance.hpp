/* compiler_instance.hpp */

#pragma once
#include "diagnostic_engine.hpp"

#include <vector>
#include <unordered_map>
#include <variant>
#include <memory>
#include <cstring>

constexpr const char *optic_extension = ".optic";

using FilePtr = std::unique_ptr<FILE, decltype(&fclose)>;
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
    FilePtr fp{nullptr, fclose};
    size_t pos = 0;
    bool reached_eof = false;

    bool is_optic_file(const std::string fname) const noexcept {
        return fname.ends_with(optic_extension);
    }

    void reset() noexcept;

public:
    File() = default;

    File(const char *fname, const char *mode) {
        if (!is_optic_file(fname)) return;
        fp = FilePtr(fopen(fname, mode), fclose);
    }

    File(const File&) = delete;
    File& operator=(const File&) = delete;

    int get() noexcept;
    inline void unget() noexcept { if (pos > 0) --pos; }
    inline int peek() const noexcept { if (pos < buffer.size()) return buffer[pos]; return EOF; }

    inline bool is_open() const noexcept { return fp.get() != nullptr; }
    inline bool iseof() const noexcept { return reached_eof; }

    bool seek(const size_t&) noexcept;
    bool open(const char *, const char *) noexcept;

    void close() noexcept;

    inline size_t get_pos() const noexcept { return pos; }
    size_t read();

    [[nodiscard]] inline std::string& data() { return buffer; }

    File& operator=(const char*);
    File& operator=(const std::pair<const char *, const char *>&);
    File& operator=(File&&) noexcept;
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