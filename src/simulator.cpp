#include "simulator.h"
#include <queue>
#include <unordered_map>
#include <stdexcept>

Simulator::Simulator(Netlist& netlist) : netlist_(netlist) {
    buildTopologicalOrder();
}

void Simulator::buildTopologicalOrder() {
    std::unordered_map<std::shared_ptr<Node>, int> inDegree;
    const auto& allNodes = netlist_.getAllNodes();
    
    for (const auto& pair : allNodes) {
        inDegree[pair.second] = pair.second->inputs.size();
    }

    std::queue<std::shared_ptr<Node>> q;
    for (const auto& pair : inDegree) {
        if (pair.second == 0) {
            q.push(pair.first);
        }
    }

    while (!q.empty()) {
        auto curr = q.front();
        q.pop();
        topoOrder_.push_back(curr);

        for (auto out : curr->outputs) {
            if (--inDegree[out] == 0) {
                q.push(out);
            }
        }
    }
}

bool Simulator::setInput(const std::string& name, LogicVal val) {
    auto node = netlist_.getNode(name);
    if (node && node->type == NodeType::PI) {
        node->value = val;
        return true;
    }
    return false;
}

LogicVal Simulator::evaluateGate(TokenType gateType, const std::vector<LogicVal>& inVals) {
    bool hasX = false;
    
    if (gateType == TokenType::And || gateType == TokenType::Nand) {
        bool hasZero = false;
        for (auto v : inVals) {
            if (v == LogicVal::Zero) hasZero = true;
            else if (v == LogicVal::X) hasX = true;
        }
        LogicVal res = hasZero ? LogicVal::Zero : (hasX ? LogicVal::X : LogicVal::One);
        if (gateType == TokenType::Nand) {
            if (res == LogicVal::Zero) res = LogicVal::One;
            else if (res == LogicVal::One) res = LogicVal::Zero;
        }
        return res;
    } else if (gateType == TokenType::Or || gateType == TokenType::Nor) {
        bool hasOne = false;
        for (auto v : inVals) {
            if (v == LogicVal::One) hasOne = true;
            else if (v == LogicVal::X) hasX = true;
        }
        LogicVal res = hasOne ? LogicVal::One : (hasX ? LogicVal::X : LogicVal::Zero);
        if (gateType == TokenType::Nor) {
            if (res == LogicVal::Zero) res = LogicVal::One;
            else if (res == LogicVal::One) res = LogicVal::Zero;
        }
        return res;
    } else if (gateType == TokenType::Not) {
        if (inVals.empty()) return LogicVal::X;
        if (inVals[0] == LogicVal::Zero) return LogicVal::One;
        if (inVals[0] == LogicVal::One) return LogicVal::Zero;
        return LogicVal::X;
    } else if (gateType == TokenType::Xor) {
        int ones = 0;
        for (auto v : inVals) {
            if (v == LogicVal::X) return LogicVal::X;
            if (v == LogicVal::One) ones++;
        }
        return (ones % 2 == 1) ? LogicVal::One : LogicVal::Zero;
    }
    
    return LogicVal::X;
}

void Simulator::simulate() {
    for (auto node : topoOrder_) {
        // Evaluate logic for non-PIs
        if (node->type != NodeType::PI) {
            std::vector<LogicVal> inVals;
            for (auto in : node->inputs) {
                inVals.push_back(in->value);
            }

            if (node->gateType != TokenType::Unknown) {
                node->value = evaluateGate(node->gateType, inVals);
            } else if (!inVals.empty()) {
                node->value = inVals[0]; // Simple wire passing
            }
        }

        // Apply fault injection
        if (node->fault == FaultType::SA0) {
            node->value = LogicVal::Zero;
        } else if (node->fault == FaultType::SA1) {
            node->value = LogicVal::One;
        }
    }
}

LogicVal Simulator::getValue(const std::string& name) const {
    auto node = netlist_.getNode(name);
    if (node) return node->value;
    return LogicVal::X;
}
