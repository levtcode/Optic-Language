/* lexer.cpp */

// TODO: Implement a function that reads multiline strings

#include "lexer.hpp"
#include "tables.hpp"
#include "trie_operators.hpp"

inline bool is_octal(int c) noexcept { return (c >= '0' && c <= '7'); }

inline bool is_numeric(int c) noexcept {
    return (isdigit(c) || (c == '.') || 
            (c == 'e') || (c == 'E'));
}



inline bool Lexer::is_symbol(const int c) const noexcept {
    return ((c == '(') || (c == ')') ||
            (c == '{') || (c == '}') ||
            (c == '[') || (c == ']') ||
            (c == ';') || (c == ':') ||
            (c == ',') || (c == '.'));
}

inline bool Lexer::is_operator(const int c) const noexcept {
    return ((c == '=') || (c == '+') || (c == '-') ||
            (c == '*') || (c == '%') || (c == '&') ||
            (c == '|') || (c == '~') || (c == '^') ||
            (c == '<') || (c == '>') || (c == '!'));
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
void Lexer::read_octal_escape(std::string &str) noexcept {
    std::string tmp_buf;
    int n, c = get();

    do {
        tmp_buf += c;
    } while ((c = get()) != EOF && is_octal(c));
    --pos;

    if ((n = std::stoi(tmp_buf, nullptr, 8)) > 255) {
        // Error
    }

    str += n;
}

/* */
void Lexer::read_hexadecimal_escape(std::string &str) noexcept {
    std::string tmp_buf;
    short int n, c = get();

    do {
        tmp_buf += c;
    } while ((c = get()) != EOF && isxdigit(c));
    --pos;

    if ((n = std::stoi(tmp_buf, nullptr, 16)) > 255) {
        // Error
    }

   str += n;
}

// TODO:
// /* */
// void Lexer::read_unicode_escape(std::string &str, int bytes) noexcept {
//     return;
// }

/* */
token_t Lexer::read_identifier() noexcept {
    std::string word;
    size_t _start_col = module->loc().column;
    int c = get();

    do {
        word += c;
        
    } while ((c = get()) != EOF && (isalnum(c) || c == '_'));
    if (c != EOF) --pos;

    if (keyword_table.find(word) != keyword_table.end()) return token_t(keyword_table.at(word), word, module->loc().line, _start_col);
    return token_t(TokenType::Identifier, word, module->loc().line, _start_col);
}

/* */
token_t Lexer::read_string() noexcept {
    std::string str;
    size_t _start_col = module->loc().column;
    int quote = get(), tmp = get();

    str += tmp;
    module->loc().column += 2;

    while ((tmp = get()) != EOF && tmp != quote) {
        if (tmp == '\\') {
            tmp = get();

            if (is_octal(tmp)) {
                --pos;
                read_octal_escape(str);
                continue;
            }

            if (tmp == 'x') {
                read_hexadecimal_escape(str);
                continue;
            }

            if (tmp == 'u' || tmp == 'U') {
                // TODO
                continue;
            }

            else {
                str += read_escape(tmp);
                module->loc().column += 2;
                continue;
            }
        }

        str += tmp;
    }
   
    if (tmp != quote) {
        diagnostic_engine->report(
            module->loc(),
            "Error: String not closed",
            "Solution: Close the string, append '\"' to the end of the string. Example: \"Hello World\" <- here\n",
            "More information: <...>",
            DiagnosticsLevel::Error
        );
    }

    return token_t(TokenType::String, str, module->loc().line, _start_col);
}

/* */
token_t Lexer::read_number() noexcept { // Refactorize: Verifiy if output is a valid number
    std::string number;
    size_t _start_col = module->loc().column;
    int c = get();
    bool flag = false;

    number += c;
    while ((c = get()) != EOF && is_numeric(c)) {
        if (c == '.' || c == 'e' || c == 'E') flag = true;
        number += c;
    }
    if (c != EOF) --pos;

    return token_t((flag) ? TokenType::Float : TokenType::Int, number, module->loc().line, _start_col);
}

/* */
token_t Lexer::read_operator() noexcept {
    int c;
    size_t _start_col = module->loc().column;

    std::string current;
    std::string longest_match;

    while ((c = get()) != EOF) {
        current += c;

        if (!TrieNode::is_prefix(current, operator_table))
            break;

        if (TrieNode::search(current))
            longest_match = current;
    }

    if (c != EOF) --pos;

    return token_t(operator_table.at(longest_match), longest_match, module->loc().line, _start_col); // Make trim to delete right spaces
}

/* */
void Lexer::ignore_comment() noexcept {
    int c = get();

    if (c == '*') {
        while ((c = get()) != EOF) {
            if (c == '*') {
                if (peek() == '/') {
                    get();
                    return;
                }
            }
        }

        diagnostic_engine->report(
            module->loc(),
            "Error: Block comment not closed, '*/' expected",
            "Solution: Append '*/' to the end of the block comment. Example: /* This is a block comment */ <- here\n",
            "More information: <...>",
            DiagnosticsLevel::Error
        );
    }

    else {
        while ((c = get()) != EOF && c != '\n');
    }
}

/* */
void Lexer::tokenize(std::vector<token_t> &tokens) noexcept {
    TrieNode::init(operator_table);
    int c;

    while ((c = get()) != EOF) {
        if (c == ' ' || c == '\t' || c == '\n') {
            continue;
        }

        if (is_symbol(c)) {
            tokens.emplace_back(
                symbol_table.at(c),
                std::string(1, static_cast<char>(c)),
                module->loc().line,
                module->loc().column
            );
            continue;
        }

        if (c == '\'' || c == '"') {
            --pos;
            tokens.emplace_back(read_string());
            continue;
        }

        if (c == '/') {
            if (peek() == '/' || peek() == '*') {
                ignore_comment();
                continue;
            } else {
                tokens.emplace_back(TokenType::Slash, "/", module->loc().line, module->loc().column);
                continue;
            }
        }
   
        if (is_operator(c)) {
            if (c == '+' || c == '-') {
                if (isdigit(peek())) {
                    --pos;
                    tokens.emplace_back(read_number());
                    continue;
                }
            }
           
            --pos;
            tokens.emplace_back(read_operator());
            continue;
        }

        if (isdigit(c)) {
            tokens.emplace_back(read_number());
            continue;
        }

        if (isalpha(c) || c == '_') {
            if ((c == 'f' || c == 'r') && peek() == '"') {
                tokens.emplace_back(
                    (c == 'f' ? TokenType::FString : TokenType::RawString),
                    (c == 'f' ? "f" : "r"),
                    module->loc().line, module->loc().column
                ); 
                continue;
            }

            --pos;
            tokens.emplace_back(read_identifier());
            continue;
        }
    }

    tokens.emplace_back(TokenType::EndOfFile, "EOF", module->loc().line, module->loc().column);
    TrieNode::destroy();
}

#ifdef OPTIC_DEBUG

#include <iostream>

/* */
const char *to_string_token(const TokenType type) {
    switch (type) {
#define X(name) case TokenType::name: return #name;
#include "tokens_types.def"
#undef X
        default: return "Unknown";
    }
}

/* */
void Lexer::print_tokens(Module &module) noexcept {
    for (auto &token : module.get_tokens()) {
        std::cout << "Token type: " << to_string_token(token.type) << "\n";
        std::cout << "Token value: '" << token.value << "'\n";
        std::cout << "At line: " << token.line << ", in column: " << token.column << "\n";
        std::cout << "In file: " << module.get_name() << "\n\n";
    }
}

#endif