/* lexer.cpp */

#include "lexer.hpp"
#include "trie_operators.hpp"
#include "tables.hpp"

int Lexer::get() noexcept {
    if (pos >= buffer->size()) {
       reached_eof = true;
       return EOF;
    }
   
    return buffer->at(pos++);
}

int Lexer::next() const noexcept {
    return (pos + 1 < buffer->size()) ? buffer->at(pos + 1) : EOF;
}

int Lexer::peek() const noexcept {
    return (pos < buffer->size()) ? buffer->at(pos) : EOF;
}

bool Lexer::iseof() const noexcept {
    return reached_eof;
}

bool is_octal(int c) {
    return (c >= '0' && c <= '7');
}

bool is_numeric(int c) {
    return (isdigit(c) || (c == '.') || 
            (c == 'e') || (c == 'E') || 
            (c == '+') || (c == '-'));
}

bool is_prefix_of_operator(const std::string &s) {
    for (const auto &[op, _] : operator_table) {
        if (op.starts_with(s)) return true;
    }
    return false;
}

char Lexer::read_escape(int c) const noexcept {
    switch (c) {
        case 'a': return '\a';
        case 'b': return '\b';
        case 't': return '\t';
        case 'n': return '\n';
        case 'f': return '\f';
        case 'r': return '\r';
        case '"': return '\"';
        case '\'': return '\'';
        default: return c;
    }
}

/* */
void Lexer::read_octal_escape(std::string &str, SourceLocation &loc) noexcept {
    std::string tmp_buf;
    int n, c = get();

    do {
        tmp_buf += c;
        loc.column++;
    } while (!iseof() && is_octal(c = get()));
    --pos;

    if ((n = std::stoi(tmp_buf, nullptr, 8)) > 255) {
        // Error
    }

    str += n;
}

/* */
void Lexer::read_hexadecimal_escape(std::string &str, SourceLocation &loc) noexcept {
    std::string tmp_buf;
    short int n, c = get();

    do {
        tmp_buf += c;
        loc.column++;
    } while (!iseof() && isxdigit(c = get()));
    --pos;

    if ((n = std::stoi(tmp_buf, nullptr, 16)) > 255) {
        // Error
    }

   str += n;
}

/* */
void Lexer::read_unicode_escape(std::string &str, int bytes) noexcept {
    return;
}

/* */
token_t Lexer::read_identifier(SourceLocation &loc) noexcept {
    SourceLocation _start;
    std::string word;
    int c = get();

    _start.column = loc.column;

    do {
        word += c;
        loc.column++;
    } while (!iseof() && (isalnum(c = get()) || c == '_'));
    --pos;

    if (keyword_table.find(word) != keyword_table.end()) return token_t(keyword_table.at(word), word, _start);
    return token_t(TokenType::Identifier, word, loc);
}

/* */
token_t Lexer::read_string(SourceLocation &loc) noexcept {
    SourceLocation _start = loc;
    std::string str;
    int quote = get(), tmp = get();

    _start.column = loc.column;

    str += tmp;
    loc.column += 2;

    while (!iseof() && (tmp = get()) != quote) {
        if (tmp == '\\') {
            tmp = get();
            loc.column++;

            if (is_octal(tmp)) {
                --pos;
                read_octal_escape(str, loc);
                continue;
            }

            if (tmp == 'x') {
                loc.column++;
                read_hexadecimal_escape(str, loc);
                continue;
            }

            if (tmp == 'u' || tmp == 'U') {
                // TODO
                continue;
            }

            else {
                str += read_escape(tmp);
                loc.column += 2;
                continue;
            }
        }

        str += tmp;
        loc.column++;
    }
   
    if (tmp != quote) {
        // Error
    }

    return token_t(TokenType::String, str, _start);
}

/* */
token_t Lexer::read_number(SourceLocation &loc) noexcept {
    SourceLocation _start = loc;
    std::string number;
    int c = get();
    bool flag = false;

    _start.column = loc.column;

    number += c;
    loc.column++;

    while (!iseof() && is_numeric(c = get())) {
        if (c == '.' || c == 'e' || c == 'E') flag = true;
        number += c;
        loc.column++;
    }
    --pos;

    return token_t((flag) ? TokenType::Float : TokenType::Int, number, _start);
}

/* */
token_t Lexer::read_operator(SourceLocation &loc) noexcept {
    return token_t(TokenType::Return, "", loc);
}

bool Lexer::is_symbol(const int c) const noexcept {
    return ((c == '(') || (c == ')') ||
            (c == '{') || (c == '}') ||
            (c == '[') || (c == ']') ||
            (c == ';') || (c == ':') ||
            (c == ',') || (c == '.'));
}

bool Lexer::is_operator(const int c) const noexcept {
    return ((c == '=') || (c == '+') || (c == '-') ||
            (c == '*') || (c == '%') || (c == '&') ||
            (c == '|') || (c == '~') || (c == '^') ||
            (c == '<') || (c == '>') || (c == '!'));
}

/* */
void Lexer::tokenize(std::vector<token_t> &tokens) noexcept {
    SourceLocation loc;
    int c;

    while (!iseof()) {
        c = get();

        if (c == ' ' || c == '\t') {
            loc.column++;
            continue;
        }

        if (c == '\n') {
            loc.line++;
            loc.column = 1;
            continue;
        }

        if (is_symbol(c)) {
            tokens.emplace_back(
                symbol_table.at(c),
                std::string(1, static_cast<char>(c)),
                loc
            );
            loc.column++;
            continue;
        }

        if (c == '\'' || c == '"') {
            --pos;
            tokens.emplace_back(read_string(loc));
            loc.column++;
            continue;
        }

        if (c == '/') {
            // TODO: APPEND OPERATOR DIV '/'
        }
   
        if (is_operator(c)) {
            if (c == '+' || c == '-') {
                if (isdigit(peek())) {
                    --pos;
                    tokens.emplace_back(read_number(loc));
                    continue;
                }
            }
           
            --pos;
            tokens.emplace_back(read_operator(loc));
            continue;
        }

        if (isdigit(c)) {
            --pos;
            tokens.emplace_back(read_number(loc));
            continue;
        }

        if (isalpha(c) || c == '_') {
            if (c == 'f' && (peek() == '"' || peek() == '\'')) {
                tokens.emplace_back(TokenType::FString, "f", loc);
                loc.column++;
                continue;
            }

            --pos;
            tokens.emplace_back(read_identifier(loc));
            continue;
        }

        else {
            // Error
        }
    }

    tokens.emplace_back(TokenType::EndOfFile, "", loc);
}

#ifdef OPTIC_DEBUG

#include <iostream>

/* */
const char *to_string_token(const TokenType &type) {
    switch (type) {
#define X(name) case TokenType::name: return #name;
#include "tokens_types.def"
#undef X
        default: return "Unknown";
    }
}

/* */
void Lexer::print_tokens(const std::vector<token_t> &tokens) noexcept {
    std::cout << "----- PRINTING TOKENS -----\n\n";

    for (auto &token : tokens) {
        std::cout << "Token type: " << to_string_token(token.type) << "\n";
        std::cout << "Token value: '" << token.value << "'\n";
        std::cout << "At line: " << token.loc.line << ", in column: " << token.loc.column << "\n";
        std::cout << "In file: " << token.loc.file << "\n";
    }
}

#else

// Code here

#endif