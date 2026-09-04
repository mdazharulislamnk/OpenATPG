#include "parser.h"

Parser::Parser(const std::vector<Token>& tokens) : tokens_(tokens), pos_(0) {}

const Token& Parser::peek() const {
    if (pos_ >= tokens_.size()) return tokens_.back(); // Return EOF if out of bounds
    return tokens_[pos_];
}

const Token& Parser::advance() {
    if (pos_ >= tokens_.size()) return tokens_.back();
    return tokens_[pos_++];
}

const Token& Parser::expect(TokenType type, const std::string& errMsg) {
    if (peek().type == type) {
        return advance();
    }
    throw std::runtime_error("Parse error at line " + std::to_string(peek().line) + ": " + errMsg);
}

bool Parser::match(TokenType type) {
    if (peek().type == type) {
        advance();
        return true;
    }
    return false;
}

std::shared_ptr<AstModule> Parser::parse() {
    return parseModule();
}

std::shared_ptr<AstModule> Parser::parseModule() {
    expect(TokenType::Module, "Expected 'module' keyword");
    
    auto mod = std::make_shared<AstModule>();
    mod->name = expect(TokenType::Identifier, "Expected module name").value;
    
    if (match(TokenType::LParen)) {
        parsePortList(mod);
        expect(TokenType::RParen, "Expected ')' after port list");
    }
    expect(TokenType::Semicolon, "Expected ';' after module declaration");

    while (peek().type != TokenType::EndModule && peek().type != TokenType::Eof) {
        parseModuleItem(mod);
    }

    expect(TokenType::EndModule, "Expected 'endmodule' keyword");
    return mod;
}

void Parser::parsePortList(std::shared_ptr<AstModule>& mod) {
    if (peek().type == TokenType::Identifier) {
        mod->ports.push_back(advance().value);
        while (match(TokenType::Comma)) {
            mod->ports.push_back(expect(TokenType::Identifier, "Expected port name").value);
        }
    }
}

void Parser::parseModuleItem(std::shared_ptr<AstModule>& mod) {
    TokenType type = peek().type;
    switch (type) {
        case TokenType::Input:
            parsePortDecl(mod, false);
            break;
        case TokenType::Output:
            parsePortDecl(mod, true);
            break;
        case TokenType::Wire:
            parseWireDecl(mod);
            break;
        case TokenType::And:
        case TokenType::Or:
        case TokenType::Not:
        case TokenType::Nand:
        case TokenType::Nor:
        case TokenType::Xor:
            parseGateInst(mod, advance().type);
            break;
        default:
            throw std::runtime_error("Unexpected token in module body at line " + std::to_string(peek().line));
    }
}

void Parser::parsePortDecl(std::shared_ptr<AstModule>& mod, bool isOutput) {
    advance(); // Consume input/output
    auto decl = std::make_shared<AstPortDecl>();
    decl->isOutput = isOutput;
    
    decl->names.push_back(expect(TokenType::Identifier, "Expected port name").value);
    while (match(TokenType::Comma)) {
        decl->names.push_back(expect(TokenType::Identifier, "Expected port name").value);
    }
    expect(TokenType::Semicolon, "Expected ';' after port declaration");
    mod->items.push_back(decl);
}

void Parser::parseWireDecl(std::shared_ptr<AstModule>& mod) {
    advance(); // Consume wire
    auto decl = std::make_shared<AstWireDecl>();
    
    decl->names.push_back(expect(TokenType::Identifier, "Expected wire name").value);
    while (match(TokenType::Comma)) {
        decl->names.push_back(expect(TokenType::Identifier, "Expected wire name").value);
    }
    expect(TokenType::Semicolon, "Expected ';' after wire declaration");
    mod->items.push_back(decl);
}

void Parser::parseGateInst(std::shared_ptr<AstModule>& mod, TokenType gateType) {
    auto inst = std::make_shared<AstGateInst>();
    inst->gateType = gateType;
    
    if (peek().type == TokenType::Identifier) {
        inst->instName = advance().value;
    } else {
        inst->instName = "unnamed_gate_" + std::to_string(mod->items.size());
    }
    
    expect(TokenType::LParen, "Expected '(' after gate name");
    
    inst->connections.push_back(expect(TokenType::Identifier, "Expected connection name").value);
    while (match(TokenType::Comma)) {
        inst->connections.push_back(expect(TokenType::Identifier, "Expected connection name").value);
    }
    
    expect(TokenType::RParen, "Expected ')' after connections");
    expect(TokenType::Semicolon, "Expected ';' after gate instance");
    
    mod->items.push_back(inst);
}
