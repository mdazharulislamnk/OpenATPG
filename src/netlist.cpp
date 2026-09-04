#include "netlist.h"

std::shared_ptr<Node> Netlist::getOrCreateNode(const std::string& name) {
    auto it = nodes_.find(name);
    if (it != nodes_.end()) {
        return it->second;
    }
    auto node = std::make_shared<Node>(name, NodeType::Gate); // Default to gate/wire
    nodes_[name] = node;
    return node;
}

std::shared_ptr<Node> Netlist::getNode(const std::string& name) const {
    auto it = nodes_.find(name);
    if (it != nodes_.end()) {
        return it->second;
    }
    return nullptr;
}

void Netlist::buildFromAst(const std::shared_ptr<AstModule>& ast) {
    if (!ast) return;

    for (const auto& item : ast->items) {
        if (item->type == AstNodeType::PortDecl) {
            auto portDecl = std::static_pointer_cast<AstPortDecl>(item);
            for (const auto& name : portDecl->names) {
                auto node = getOrCreateNode(name);
                if (portDecl->isOutput) {
                    node->type = NodeType::PO;
                    primaryOutputs_.push_back(node);
                } else {
                    node->type = NodeType::PI;
                    primaryInputs_.push_back(node);
                }
            }
        }
    }

    // Now process gates
    for (const auto& item : ast->items) {
        if (item->type == AstNodeType::GateInst) {
            auto gateInst = std::static_pointer_cast<AstGateInst>(item);
            if (gateInst->connections.empty()) continue;

            // Output is typically the first connection
            auto outNode = getOrCreateNode(gateInst->connections[0]);
            
            // If the outNode is just a wire and not PI/PO, ensure it's marked as Gate
            if (outNode->type != NodeType::PI && outNode->type != NodeType::PO) {
                outNode->type = NodeType::Gate;
            }
            outNode->gateType = gateInst->gateType;

            for (size_t i = 1; i < gateInst->connections.size(); ++i) {
                auto inNode = getOrCreateNode(gateInst->connections[i]);
                outNode->addInput(inNode);
                inNode->addOutput(outNode);
            }
        }
    }
}
