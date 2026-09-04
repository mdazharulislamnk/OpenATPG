#pragma once

#include "netlist.h"
#include <string>

class Visualizer {
public:
    explicit Visualizer(const Netlist& netlist);

    void generateDot(const std::string& filename, const std::string& faultNode = "", FaultType faultType = FaultType::None) const;

private:
    const Netlist& netlist_;
    
    std::string getGateSymbol(TokenType type) const;
};
