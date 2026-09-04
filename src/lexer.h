#pragma once

#include <string>
#include <vector>
#include <cstdint>

enum class TokenType {
    Module,
    EndModule,
    Input,
    Output,
    Wire,
    And,
    Or,
    Not,
    Nand,
    Nor,
    Xor,
    Identifier,
    LParen,
    RParen,
    Comma,
    Semicolon,
    Eof,
    Unknown
};

struct Token {
    TokenType type;
    std::string value;
    size_t line;
};

class Lexer {
public:
    explicit Lexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    std::string source_;
    size_t pos_;
    size_t line_;

    char peek() const;
    char advance();
    void skipWhitespace();
    Token nextToken();
};
