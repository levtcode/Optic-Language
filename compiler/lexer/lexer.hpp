/* lexer.hpp */

#pragma once

#include "../compiler_instance.hpp"
#include "../diagnostic_engine.hpp"

#include <vector>
#include <string>

class Lexer {
    const std::vector<uint8_t> *buffer;
    const std::vector<SourceLocationRange> *location_ranges;
    DiagnosticEngine *diagnostic_engine;
    size_t pos = 0;
    bool reached_eof = false;

    int get() noexcept;
    int next() const noexcept;
    int peek() const noexcept;
    bool iseof() const noexcept;

public:
    Lexer(
        const std::vector<uint8_t> *buff,
        const std::vector<SourceLocationRange> *loc_ranges,
        DiagnosticEngine *diag_engine
    ) : buffer(buff), location_ranges(loc_ranges), diagnostic_engine(diag_engine) {}

    token_t read_identifier(SourceLocation&) noexcept;
    token_t read_string(SourceLocation&) noexcept;
    token_t read_number(SourceLocation&) noexcept;
    token_t read_operator(SourceLocation&) noexcept;

    bool is_symbol(const int) const noexcept;
    bool is_operator(const int) const noexcept;

    char read_escape(int) const noexcept;

    void read_octal_escape(std::string&, SourceLocation&) noexcept;
    void read_hexadecimal_escape(std::string&, SourceLocation&) noexcept;
    void read_unicode_escape(std::string&, const int) noexcept;

    void tokenize(std::vector<token_t>&) noexcept;
#ifdef OPTIC_DEBUG
    void print_tokens(const std::vector<token_t>&) noexcept;
#endif
};

#ifdef OPTIC_DEBUG
const char *to_string_token(TokenType &type);
#endif