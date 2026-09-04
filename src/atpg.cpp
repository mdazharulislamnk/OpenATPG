#include "atpg.h"
#include <iostream>

ATPG::ATPG(Netlist& netlist, Simulator& sim) : netlist_(netlist), sim_(sim) {
    piNodes_ = netlist_.getPrimaryInputs();
}

bool ATPG::generatePattern(const std::string& faultNode, FaultType faultType, std::map<std::string, LogicVal>& testVector) {
    // Reset PIs to X
    for (auto& pi : piNodes_) {
        pi->value = LogicVal::X;
    }

    if (searchPattern(0, faultNode, faultType)) {
        for (auto& pi : piNodes_) {
            testVector[pi->name] = pi->value;
        }
        return true;
    }
    return false;
}

bool ATPG::searchPattern(size_t piIndex, const std::string& faultNode, FaultType faultType) {
    if (piIndex == piNodes_.size()) {
        return isFaultDetected(faultNode, faultType);
    }

    // Try assigning 0
    piNodes_[piIndex]->value = LogicVal::Zero;
    if (searchPattern(piIndex + 1, faultNode, faultType)) {
        return true;
    }

    // Try assigning 1
    piNodes_[piIndex]->value = LogicVal::One;
    if (searchPattern(piIndex + 1, faultNode, faultType)) {
        return true;
    }

    // Backtrack
    piNodes_[piIndex]->value = LogicVal::X;
    return false;
}

bool ATPG::isFaultDetected(const std::string& faultNode, FaultType faultType) {
    auto target = netlist_.getNode(faultNode);
    if (!target) return false;

    // 1. Simulate Good Circuit
    target->fault = FaultType::None;
    sim_.simulate();
    
    std::map<std::string, LogicVal> goodPO;
    for (const auto& po : netlist_.getPrimaryOutputs()) {
        goodPO[po->name] = po->value;
    }

    // 2. Simulate Faulty Circuit
    target->fault = faultType;
    sim_.simulate();

    bool detected = false;
    for (const auto& po : netlist_.getPrimaryOutputs()) {
        LogicVal goodVal = goodPO[po->name];
        LogicVal faultyVal = po->value;
        
        if ((goodVal == LogicVal::Zero && faultyVal == LogicVal::One) ||
            (goodVal == LogicVal::One && faultyVal == LogicVal::Zero)) {
            detected = true;
            break;
        }
    }

    // Reset fault
    target->fault = FaultType::None;
    return detected;
}
