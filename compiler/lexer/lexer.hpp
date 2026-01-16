/* lexer.hpp */

#pragma once
#include "trie_operators.hpp"
#include "../diagnostic_engine.hpp"

#include <vector>
#include <string>

struct token_t {
    TokenType type;
    std::string value;
    SourceLocation loc;
    
    token_t(const TokenType typ, const std::string &val, const SourceLocation &location) : 
    type(typ), value(val), loc(location) {}
};

class Lexer {
    std::string_view buffer;
    SourceLocation loc;
    size_t pos = 0;
    bool reached_eof = false;

    int get() noexcept;
    int next() const noexcept;
    int peek() const noexcept;
    bool iseof() const noexcept;

public:
    Lexer(const std::string &f, const char *fn) : buffer(f) { loc.file = fn; }

    token_t read_identifier();
    token_t read_string();
    token_t read_number();
    token_t read_operator();

    bool is_symbol(const int);
    bool is_operator(const int);

    char read_escape(int);

    void ignore_comment();
    void read_octal_escape(std::string &);
    void read_hexadecimal_escape(std::string &);
    void read_unicode_escape(std::string &, const int);

    void tokenize();
#ifdef OPTIC_DEBUG
    void print();
#else
    // Code here
#endif
};

extern std::vector<token_t> tokens;

#ifdef OPTIC_DEBUG
const char *to_string_token(TokenType &type);
#endif