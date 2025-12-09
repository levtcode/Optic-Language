/* lexer.hpp */

#include <string>

enum class TokenType {
    // Types
    Int,
    Float,
    Double,
    Bool,
    String,
    Identifier,

    // Symbols
    LeftParen,
    RightParen,
    LeftBrace,
    RightBrace,
    LeftBracket,
    RightBracket,
    Colon,
    Semicolon,

    // Operators
    Equal,
    Plus,
    Minus,
    Star,
    Slash,
    Mod,
    BitAnd,
    BitOr,
    BitNot,
    BitXor,
    And_C,
    Or_C,
    Not_C,
    And,
    Or,
    Not,
    EqualEqual,
    BangEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,

    // Keywords
    Func,
    When,
    But,
};

typedef struct token_t {
    TokenType type;
    std::string value;
    int line;
    int column;
};

class Lexer {

};