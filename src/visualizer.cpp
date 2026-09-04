#include "visualizer.h"
#include <fstream>
#include <sstream>

Visualizer::Visualizer(const Netlist& netlist) : netlist_(netlist) {}

std::string Visualizer::getGateSymbol(TokenType type) const {
    switch (type) {
        case TokenType::And: return "AND";
        case TokenType::Or: return "OR";
        case TokenType::Not: return "NOT";
        case TokenType::Nand: return "NAND";
        case TokenType::Nor: return "NOR";
        case TokenType::Xor: return "XOR";
        default: return "BUF";
    }
}

void Visualizer::generateDot(const std::string& filename, const std::string& faultNode, FaultType faultType) const {
    std::ofstream out(filename);
    if (!out.is_open()) return;

    out << "digraph Circuit {\n";
    out << "  rankdir=LR;\n";
    out << "  node [style=filled, fontname=\"Arial\"];\n";

    const auto& nodes = netlist_.getAllNodes();
    for (const auto& pair : nodes) {
        const auto& node = pair.second;
        std::string color = "gray";
        std::string label = node->name;
        std::string shape = "ellipse";

        if (node->type == NodeType::PI) {
            color = "green";
            shape = "box";
        } else if (node->type == NodeType::PO) {
            color = "blue";
            shape = "box";
        } else {
            label += "\\n" + getGateSymbol(node->gateType);
        }

        if (node->name == faultNode && faultType != FaultType::None) {
            color = "red";
            std::string faultStr = (faultType == FaultType::SA0) ? "SA0" : "SA1";
            label += "\\n[" + faultStr + "]";
        }

        out << "  \"" << node->name << "\" [label=\"" << label << "\", color=\"" << color << "\", shape=\"" << shape << "\"];\n";
    }

    for (const auto& pair : nodes) {
        const auto& node = pair.second;
        for (const auto& outNode : node->outputs) {
            out << "  \"" << node->name << "\" -> \"" << outNode->name << "\";\n";
        }
    }

    out << "}\n";
    out.close();
}
