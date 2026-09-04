# OpenATPG: C++ Automatic Test Pattern Generation Engine
OpenATPG is a custom C++ Electronic Design Automation (EDA) tool designed to parse structural Verilog netlists, simulate logic propagation, and generate binary test vectors for stuck-at faults (SAF).

## Features
- FSM Lexer & Parser: Converts structural Verilog into an Abstract Syntax Tree (AST).
- Netlist DAG: Models logic gates and wire connections using smart pointers.
- Fault Injection: Simulates Stuck-At-0 (SA0) and Stuck-At-1 (SA1) faults.
- ATPG Algorithm: Uses basic path sensitization (D-Algorithm concepts) to generate test vectors.
- Visualization: Exports the circuit graph to Graphviz .dot files.

## Build Instructions
```bash
mkdir build && cd build
cmake ..
make
```

## Usage
Run the CLI against a structural Verilog file:
```bash
./atpg_engine --input ../examples/simple_and_or.v --output results.txt --visualize
```
