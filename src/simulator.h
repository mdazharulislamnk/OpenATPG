#pragma once

#include "netlist.h"
#include <vector>
#include <memory>
#include <map>
#include <string>

class Simulator {
public:
    explicit Simulator(Netlist& netlist);

    // Set PI values, returns false if a node is not found
    bool setInput(const std::string& name, LogicVal val);
    
    // Evaluate the whole circuit
    void simulate();

    // Get value of a node
    LogicVal getValue(const std::string& name) const;

private:
    Netlist& netlist_;
    std::vector<std::shared_ptr<Node>> topoOrder_;

    void buildTopologicalOrder();
    LogicVal evaluateGate(TokenType gateType, const std::vector<LogicVal>& inVals);
};
