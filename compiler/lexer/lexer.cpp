// /* lexer.cpp */

// #include "lexer.hpp"

// #include <iostream>
// #include <format>

// std::string fname;
// std::vector<token_t> tokens;

// int Lexer::get() noexcept {
//    if (pos >= size) {
//        reached_eof = true;
//        return EOF;
//    }
   
//    return buffer[pos++];
// }

// int Lexer::next() const noexcept {
//    return (pos + 1 < size) ? buffer[pos + 1] : EOF;
// }

// int Lexer::peek() const noexcept {
//    return (pos < size) ? buffer[pos] : EOF;
// }

// bool Lexer::iseof() const noexcept {
//    return reached_eof;
// }

// bool is_octal(int c) {
//    return (c >= '0' && c <= '7');
// }

// bool is_numeric(int c) {
//    return (isdigit(c) || (c == '.') || 
//            (c == 'e') || (c == 'E') || 
//            (c == '+') || (c == '-'));
// }

// bool is_prefix_of_operator(const std::string &s) {
//    for (const auto &[op, _] : operator_table) {
//        if (op.starts_with(s)) return true;
//    }
//    return false;
// }

// char Lexer::read_escape(int c) {
//    switch (c) {
//        case 'a': return '\a';
//        case 'b': return '\b';
//        case 't': return '\t';
//        case 'n': return '\n';
//        case 'f': return '\f';
//        case 'r': return '\r';
//        case '"': return '\"';
//        case '\'': return '\'';
//        default: return c;
//    }
// }

// /* */
// void Lexer::read_octal_escape(std::string &str) {
//    std::string tmp_buf;
//    int n, c = get();

//    do {
//        tmp_buf += c;
//        col++;
//    } while (!iseof() && is_octal(c = get()));
//    --pos;

//    if ((n = std::stoi(tmp_buf, nullptr, 8)) > 255) {
//        std::cerr << std::format(
//            "On file: {}; at line: {}, in colunm: {}.\n" \
//            "Warning: Octal sequence out of range, n > 255, n is {} (max ASCII value is 255).\n\n",
//            fname, line, col, n
//        );
//    }

//    str += n;
// }

// /* */
// void Lexer::read_hexadecimal_escape(std::string &str) {
//    std::string tmp_buf;
//    short int n, c = get();

//    do {
//        tmp_buf += c;
//        col++;
//    } while (!iseof() && isxdigit(c = get()));
//    --pos;

//    if ((n = std::stoi(tmp_buf, nullptr, 16)) > 255) {
//        std::cerr << std::format(
//            "On file: {}; at line: {}, in colunm: {}.\n" \
//            "Warning: Hex sequence out of range, n > 255, n is {} (max ASCII value is 255).\n\n",
//            fname, line, col, n
//        );
//    }

//    str += n;
// }

// /* */
// void Lexer::read_unicode_escape(std::string &str, int bytes) {
//    return;
// }

// /* */
// token_t Lexer::read_identifier() {
//    int start_col = col, c = get();
//    std::string word;

//    do {
//        word += c;
//        col++;
//    } while (!iseof() && (isalnum(c = get()) || c == '_'));
//    --pos;

//    if (keyword_table.find(word) != keyword_table.end()) return token_t(keyword_table.at(word), word, line, start_col);
//    return token_t(TokenType::Identifier, word, line, start_col);
// }

// /* */
// token_t Lexer::read_string() {
//    int start_col = col;
//    int quote = get(), tmp = get();
//    std::string str;

//    str += tmp;
//    col += 2;

//    while (!iseof() && (tmp = get()) != quote) {
//        if (tmp == '\\') {
//            tmp = get();
//            col++;

//            if (is_octal(tmp)) {
//                --pos;
//                read_octal_escape(str);
//                continue;
//            }

//            if (tmp == 'x') {
//                col++;
//                read_hexadecimal_escape(str);
//                continue;
//            }

//            if (tmp == 'u' || tmp == 'U') {
//                // TODO
//                continue;
//            }

//            else {
//                str += read_escape(tmp);
//                col += 2;
//                continue;
//            }
//        }

//        str += tmp;
//        col++;
//    }
   
//    if (tmp != quote) {
//        // Throw
//    }

//    return token_t(TokenType::String, str, line, start_col);
// }

// /* */
// token_t Lexer::read_number() {
//    int start_col = col, c = get();
//    std::string number;
//    bool flag = false;

//    number += c;
//    col++;

//    while (!iseof() && is_numeric(c = get())) {
//        if (c == '.' || c == 'e' || c == 'E') flag = true;
//        number += c;
//        col++;
//    }
//    --pos;

//    return token_t((flag) ? TokenType::Float : TokenType::Int, number, line, start_col);
// }

// /* */
// token_t Lexer::read_operator() {
//    // TODO
// }

// /* */
// void Lexer::ignore_comment() {
//    int c = get();

//    if (c == '*') {
//        col++;
//        while (!iseof()) {
//            c = get();

//            if (c == '\n') {
//                line++;
//                col = 1;
//                continue;
//            }

//            if (c == '*') {
//                if (peek() == '/') {
//                    get();
//                    col += 2;
//                    return;
//                }
//            }

//            col++;
//        }

//        // Throw not closed block comment
//    }

//    else {
//        do {
//            col++;
//        } while (!iseof() && (c = get()) != '\n');
       
//        if (c == '\n') {
//            line++;
//            col = 1;
//        }
//    }
// }

// bool Lexer::is_symbol(const int c) {
//    return ((c == '(') || (c == ')') ||
//            (c == '{') || (c == '}') ||
//            (c == '[') || (c == ']') ||
//            (c == ';') || (c == ':') ||
//            (c == ',') || (c == '.'));
// }

// bool Lexer::is_operator(const int c) {
//    return ((c == '=') || (c == '+') || (c == '-') ||
//            (c == '*') || (c == '%') || (c == '&') ||
//            (c == '|') || (c == '~') || (c == '^') ||
//            (c == '<') || (c == '>') || (c == '!'));
// }

// /* */
// void Lexer::tokenize() {
//    std::string last_word; // for error inline printing
//    int c;

//    while (!iseof()) {
//        c = get();

//        if (c == ' ' || c == '\t') {
//            col++;
//            continue;
//        }

//        if (c == '\n') {
//            line++;
//            col = 1;
//            continue;
//        }

//        if (is_symbol(c)) {
//            tokens.emplace_back(
//                symbol_table.at(c),
//                std::string(1, static_cast<char>(c)),
//                line,
//                col
//            );
//            col++;
//            continue;
//        }

//        if (c == '\'' || c == '"') {
//            --pos;
//            tokens.emplace_back(read_string());
//            col++;
//            continue;
//        }

//        if (c == '/') {
//            if (peek() == '/' || peek() == '*') {
//                ignore_comment();
//                continue;
//            } else {
//                tokens.emplace_back(TokenType::Slash, "/", line, col);
//                col++;
//                continue;
//            }
//        }
   
//        if (is_operator(c)) {
//            if (c == '+' || c == '-') {
//                if (isdigit(peek())) {
//                    --pos;
//                    tokens.emplace_back(read_number());
//                    continue;
//                }
//            }
           
//            --pos;
//            tokens.emplace_back(read_operator());
//            continue;
//        }

//        if (isdigit(c)) {
//            --pos;
//            tokens.emplace_back(read_number());
//            continue;
//        }

//        if (isalpha(c) || c == '_') {
//            if (c == 'f' && (peek() == '"' || peek() == '\'')) {
//                tokens.emplace_back(TokenType::FString, "f", line, col);
//                col++;
//                continue;
//            }

//            --pos;
//            tokens.emplace_back(read_identifier());
//            continue;
//        }

//        else {
//            // Throw exception
//        }
//    }

//    tokens.emplace_back(TokenType::EndOfFile, "", line, col);
// }

// #ifdef OPTIC_DEBUG

// /* */
// const char *to_string_token(TokenType &type) {
//    switch (type) {
// #define X(name) case TokenType::name: return #name;
// #include "tokens_types.def"
// #undef X
//        default: return "Unknown";
//    }
// }

// /* */
// void Lexer::print() {
//    for (auto &token : tokens) {
//        std::cout << "\nToken type: " << to_string_token(token.type) << "\n";
//        std::cout << "Token value: '" << token.value << "'\n";
//        std::cout << "At line: " << token.line << ", in column: " << token.column << "\n";
//        std::cout << "In file: " << fname << "\n";
//    }
// }

// #else

// // Code here

// #endif