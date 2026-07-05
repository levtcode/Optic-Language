/* lexer.hpp */

#pragma once

#include "../diagnostics_engine.hpp"
#include "../module.hpp"
#include "tokens.hpp"

#include <vector>
#include <string>

/* */
class Lexer {
private:
    Module *module;
    DiagnosticsEngine *diagnostic_engine;
    size_t pos = 0;

    inline int get() noexcept {
        int c = (pos < module->get_buffer().size()) ? module->get_buffer()[pos++] : EOF;

        if (c == '\n') {
            module->loc().line++;
            module->loc().column = 1;
        } else {
            module->loc().column++;
        }

        return c;
    }

    inline int peek() const noexcept {
        return (pos < module->get_buffer().size()) ? module->get_buffer()[pos] : EOF;
    }

public:
    Lexer(Module *mod, DiagnosticsEngine *diag_engine) : module(mod), diagnostic_engine(diag_engine) {}

    token_t read_identifier() noexcept;
    token_t read_string() noexcept;
    token_t read_number() noexcept;
    token_t read_operator() noexcept;

    inline bool is_symbol(const int) const noexcept;
    inline bool is_operator(const int) const noexcept;

    char read_escape(int) const noexcept;

    void read_octal_escape(std::string&) noexcept;
    void read_hexadecimal_escape(std::string&) noexcept;
    void read_unicode_escape(std::string&, const int) noexcept;
    void ignore_comment() noexcept;

    void tokenize(std::vector<token_t> &tokens) noexcept;
#ifdef OPTIC_DEBUG
    void print_tokens(Module&) noexcept;
#endif
};

#ifdef OPTIC_DEBUG
const char *to_string_token(TokenType type);
#endif