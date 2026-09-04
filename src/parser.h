#pragma once

#include "lexer.h"
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

enum class AstNodeType {
    Module,
    PortDecl,
    WireDecl,
    GateInst
};

struct AstNode {
    virtual ~AstNode() = default;
    AstNodeType type;
    AstNode(AstNodeType t) : type(t) {}
};

struct AstPortDecl : public AstNode {
    bool isOutput;
    std::vector<std::string> names;
    AstPortDecl() : AstNode(AstNodeType::PortDecl), isOutput(false) {}
};

struct AstWireDecl : public AstNode {
    std::vector<std::string> names;
    AstWireDecl() : AstNode(AstNodeType::WireDecl) {}
};

struct AstGateInst : public AstNode {
    TokenType gateType;
    std::string instName;
    std::vector<std::string> connections; // output is usually first in structural Verilog
    AstGateInst() : AstNode(AstNodeType::GateInst), gateType(TokenType::Unknown) {}
};

struct AstModule : public AstNode {
    std::string name;
    std::vector<std::string> ports;
    std::vector<std::shared_ptr<AstNode>> items;
    AstModule() : AstNode(AstNodeType::Module) {}
};

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    std::shared_ptr<AstModule> parse();

private:
    std::vector<Token> tokens_;
    size_t pos_;

    const Token& peek() const;
    const Token& advance();
    const Token& expect(TokenType type, const std::string& errMsg);
    bool match(TokenType type);

    std::shared_ptr<AstModule> parseModule();
    void parsePortList(std::shared_ptr<AstModule>& mod);
    void parseModuleItem(std::shared_ptr<AstModule>& mod);
    void parsePortDecl(std::shared_ptr<AstModule>& mod, bool isOutput);
    void parseWireDecl(std::shared_ptr<AstModule>& mod);
    void parseGateInst(std::shared_ptr<AstModule>& mod, TokenType gateType);
};
