#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "lexer.h"
#include "parser.h"
#include "netlist.h"
#include "simulator.h"
#include "atpg.h"
#include "visualizer.h"

void printUsage() {
    std::cout << "Usage: atpg_engine --input <verilog_file> [--output <result_file>] [--visualize]\n";
}

int main(int argc, char* argv[]) {
    std::string inputFile;
    std::string outputFile;
    bool visualize = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--input" && i + 1 < argc) {
            inputFile = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg == "--visualize") {
            visualize = true;
        } else {
            printUsage();
            return 1;
        }
    }

    if (inputFile.empty()) {
        std::cerr << "Error: Input file required.\n";
        printUsage();
        return 1;
    }

    std::ifstream t(inputFile);
    if (!t.is_open()) {
        std::cerr << "Error: Cannot open input file " << inputFile << "\n";
        return 1;
    }
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string source = buffer.str();

    try {
        Lexer lexer(source);
        auto tokens = lexer.tokenize();

        Parser parser(tokens);
        auto ast = parser.parse();

        Netlist netlist;
        netlist.buildFromAst(ast);

        Simulator sim(netlist);
        ATPG atpg(netlist, sim);

        int totalFaults = 0;
        int detectedFaults = 0;

        std::ostream* out = &std::cout;
        std::ofstream outFile;
        if (!outputFile.empty()) {
            outFile.open(outputFile);
            if (outFile.is_open()) {
                out = &outFile;
            }
        }

        *out << "ATPG Fault Coverage Report\n";
        *out << "----------------------------------------\n";

        for (const auto& pair : netlist.getAllNodes()) {
            std::string nodeName = pair.first;
            
            for (FaultType fType : {FaultType::SA0, FaultType::SA1}) {
                totalFaults++;
                std::map<std::string, LogicVal> testVector;
                
                if (atpg.generatePattern(nodeName, fType, testVector)) {
                    detectedFaults++;
                    *out << "Detected " << ((fType == FaultType::SA0) ? "SA0" : "SA1") << " at node " << nodeName << " with vector: ";
                    for (const auto& tv : testVector) {
                        *out << tv.first << "=" << (tv.second == LogicVal::One ? "1" : "0") << " ";
                    }
                    *out << "\n";
                    
                    if (visualize) {
                        Visualizer vis(netlist);
                        std::string fStr = (fType == FaultType::SA0) ? "SA0" : "SA1";
                        vis.generateDot("circuit_" + nodeName + "_" + fStr + ".dot", nodeName, fType);
                    }
                } else {
                    *out << "Undetected " << ((fType == FaultType::SA0) ? "SA0" : "SA1") << " at node " << nodeName << "\n";
                }
            }
        }

        double coverage = (totalFaults > 0) ? (static_cast<double>(detectedFaults) / totalFaults * 100.0) : 0.0;
        *out << "----------------------------------------\n";
        *out << "Total Faults: " << totalFaults << "\n";
        *out << "Detected Faults: " << detectedFaults << "\n";
        *out << "Fault Coverage: " << coverage << "%\n";
        
        if (visualize && detectedFaults == 0) {
            // Generate at least one clean dot if none was generated due to no faults
            Visualizer vis(netlist);
            vis.generateDot("circuit_clean.dot");
        }

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
