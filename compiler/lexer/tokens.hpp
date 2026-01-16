/* tokens.hpp */

#ifdef OPTIC_DEBUG

enum class TokenType {
#define X(name) name,
#include "tokens_types.def"
#undef X
};

#else

enum class TokenType {
    // Types
    Int,            // int
    Float,          // float
    Bool,           // bool
    String,         // string
    Char,           // char
    Identifier,

    // Combined types
    Const,          // const
    Pointer,        // pointer
    Array,          // array
    Tuple,          // tuple
    Dict,           // dict

    // Symbols
    LeftParen,      // (
    RightParen,     // )
    LeftBrace,      // {
    RightBrace,     // }
    LeftBracket,    // [
    RightBracket,   // ]
    Colon,          // :
    Semicolon,      // ;
    Comma,          // ,
    Point,          // .
    TernaryOp,      // ?
    Arrow,          // ->

    // Simple operators
    Equal,          // =
    Plus,           // +
    Minus,          // -
    Star,           // *
    Slash,          // /
    Mod,            // %
    Pow,            // **

    // Bit & Bit operators
    BitAnd,         // &
    BitOr,          // |
    BitNot,         // ~
    BitXor,         // ^
    LeftShift,      // <<
    RightShift,     // >>

    // Assign operators
    Increment,      // ++
    Decrement,      // --
    PlusAssign,     // +=
    MinusAssign,    // -=
    MulAssign,      // *=
    DivAssign,      // /=
    ModAssign,      // %=
    AndAssign,      // &=
    OrAssign,       // |=
    LShiftAssign,   // <<=
    RShiftAssign,   // >>=

    // Logical operators
    And_C,          // &&
    Or_C,           // ||
    Not_C,          // !
    And,            // and
    Or,             // or
    Not,            // not

    // Comparison operators
    EqualEqual,     // ==
    NotEqual,       // !=
    Less,           // <
    LessEqual,      // <=
    Greater,        // >
    GreaterEqual,   // >=

    // Preprocessor keywords
    Import,         // import
    Directive,      // directive
    Macro,          // macro

    // Keywords
    Asm,            // asm
    New,            // new
    Delete,         // delete
    Generic,        // generic
    If,             // if
    Elif,           // elif
    Else,           // else
    Func,           // func
    Null,           // null
    Return,         // return
    ForLoop,        // for
    WhileLoop,      // while
    Break,          // break
    Continue,       // continue
    Switch,         // switch
    Case,           // case
    Try,            // try
    Catch,          // catch
    Throw,          // throw
    Enum,           // enum
    Struct,         // struct
    Class,          // Class
    Lambda,         // lambda
    Template,       // template

    // Especial values
    LiteralTrue,    // true
    LiteralFalse,   // false
    FString,        // f"{}"
    RawString,      // r""
    EndOfFile       // EOF
};

#endif