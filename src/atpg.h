#pragma once

#include "netlist.h"
#include "simulator.h"
#include <map>
#include <string>
#include <vector>

class ATPG {
public:
    ATPG(Netlist& netlist, Simulator& sim);

    // Generates a test vector for the given fault. 
    // Returns true if successful, populating testVector.
    bool generatePattern(const std::string& faultNode, FaultType faultType, std::map<std::string, LogicVal>& testVector);

private:
    Netlist& netlist_;
    Simulator& sim_;
    std::vector<std::shared_ptr<Node>> piNodes_;

    bool searchPattern(size_t piIndex, const std::string& faultNode, FaultType faultType);
    bool isFaultDetected(const std::string& faultNode, FaultType faultType);
};
