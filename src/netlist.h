#pragma once

#include "parser.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

enum class LogicVal {
    Zero,
    One,
    X,
    D,
    DBar
};

enum class FaultType {
    None,
    SA0,
    SA1
};

enum class NodeType {
    PI,
    PO,
    Gate
};

class Node {
public:
    std::string name;
    NodeType type;
    TokenType gateType; // Only valid if type == Gate
    
    std::vector<std::shared_ptr<Node>> inputs;
    std::vector<std::shared_ptr<Node>> outputs;
    
    LogicVal value;
    FaultType fault;

    Node(const std::string& n, NodeType t, TokenType gt = TokenType::Unknown)
        : name(n), type(t), gateType(gt), value(LogicVal::X), fault(FaultType::None) {}

    void addInput(std::shared_ptr<Node> node) { inputs.push_back(node); }
    void addOutput(std::shared_ptr<Node> node) { outputs.push_back(node); }
};

class Netlist {
public:
    Netlist() = default;
    
    void buildFromAst(const std::shared_ptr<AstModule>& ast);
    
    std::shared_ptr<Node> getNode(const std::string& name) const;
    const std::vector<std::shared_ptr<Node>>& getPrimaryInputs() const { return primaryInputs_; }
    const std::vector<std::shared_ptr<Node>>& getPrimaryOutputs() const { return primaryOutputs_; }
    const std::unordered_map<std::string, std::shared_ptr<Node>>& getAllNodes() const { return nodes_; }

private:
    std::unordered_map<std::string, std::shared_ptr<Node>> nodes_;
    std::vector<std::shared_ptr<Node>> primaryInputs_;
    std::vector<std::shared_ptr<Node>> primaryOutputs_;

    std::shared_ptr<Node> getOrCreateNode(const std::string& name);
};
