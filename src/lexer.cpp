#include "lexer.h"
#include <cctype>
#include <unordered_map>

Lexer::Lexer(const std::string& source) : source_(source), pos_(0), line_(1) {}

char Lexer::peek() const {
    if (pos_ >= source_.length()) return '\0';
    return source_[pos_];
}

char Lexer::advance() {
    if (pos_ >= source_.length()) return '\0';
    char c = source_[pos_++];
    if (c == '\n') {
        line_++;
    }
    return c;
}

void Lexer::skipWhitespace() {
    while (true) {
        char c = peek();
        if (std::isspace(c)) {
            advance();
        } else if (c == '/' && pos_ + 1 < source_.length() && source_[pos_ + 1] == '/') {
            // Skip single line comment
            while (peek() != '\n' && peek() != '\0') {
                advance();
            }
        } else {
            break;
        }
    }
}

Token Lexer::nextToken() {
    skipWhitespace();
    
    char c = peek();
    if (c == '\0') {
        return {TokenType::Eof, "", line_};
    }
    
    if (std::isalpha(c) || c == '_') {
        std::string ident;
        while (std::isalnum(peek()) || peek() == '_') {
            ident += advance();
        }
        
        static const std::unordered_map<std::string, TokenType> keywords = {
            {"module", TokenType::Module},
            {"endmodule", TokenType::EndModule},
            {"input", TokenType::Input},
            {"output", TokenType::Output},
            {"wire", TokenType::Wire},
            {"and", TokenType::And},
            {"or", TokenType::Or},
            {"not", TokenType::Not},
            {"nand", TokenType::Nand},
            {"nor", TokenType::Nor},
            {"xor", TokenType::Xor}
        };
        
        auto it = keywords.find(ident);
        if (it != keywords.end()) {
            return {it->second, ident, line_};
        }
        return {TokenType::Identifier, ident, line_};
    }
    
    char next = advance();
    switch (next) {
        case '(': return {TokenType::LParen, "(", line_};
        case ')': return {TokenType::RParen, ")", line_};
        case ',': return {TokenType::Comma, ",", line_};
        case ';': return {TokenType::Semicolon, ";", line_};
        default: return {TokenType::Unknown, std::string(1, next), line_};
    }
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    Token tok;
    do {
        tok = nextToken();
        tokens.push_back(tok);
    } while (tok.type != TokenType::Eof);
    return tokens;
}
