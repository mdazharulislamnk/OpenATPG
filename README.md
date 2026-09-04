# OpenATPG
![C++](https://img.shields.io/badge/C++-17-blue.svg) ![CMake](https://img.shields.io/badge/CMake-3.14+-green.svg) ![GTest](https://img.shields.io/badge/Testing-Google%20Test-yellow.svg) ![License](https://img.shields.io/badge/License-MIT-lightgray.svg)

OpenATPG is a custom C++ Electronic Design Automation (EDA) tool engineered to parse structural Verilog netlists, simulate logic propagation, and algorithmically generate binary test vectors for **Stuck-At Faults (SAF)**. It leverages Object-Oriented Programming (OOP), smart pointers, and a topological logic simulator to achieve 100% test coverage on supported digital circuits.

---

## Table of Contents
- [Overview](#overview)
- [Tech Stack](#tech-stack)
- [Directory Structure](#directory-structure)
- [Prerequisites](#prerequisites)
- [Installation Guide](#installation-guide)
- [Usage & Running Commands](#usage--running-commands)
- [Testing](#testing)
- [Interactive Web Simulator](#interactive-web-simulator)
- [Author & License](#author--license)

---

## Overview
When silicon chips are manufactured, microscopic physical defects (like short circuits to ground or power) can occur. OpenATPG determines the exact sequence of `1`s and `0`s (test vectors) required to catch these defects using path sensitization. 

**Core Capabilities:**
1. **Frontend**: A custom FSM-based Lexer and Recursive Descent Parser convert structural Verilog into an Abstract Syntax Tree (AST).
2. **Intermediate Representation (IR)**: The AST is lowered into a smart-pointer-backed Directed Acyclic Graph (DAG) for traversal.
3. **Logic Simulation**: A topological sort-based engine evaluates binary inputs (`0`, `1`) and unknown states (`X`) across all logic gates.
4. **Fault Injection & ATPG**: Simulates both Good and Faulty circuits concurrently, sweeping Primary Inputs (PI) to find the exact combination that propagates the injected Stuck-At-0 (SA0) or Stuck-At-1 (SA1) error to a Primary Output (PO).
5. **Visualization**: Generates `.dot` files color-coding the circuit and highlighting the specific injected faults for Graphviz rendering.

---

## Tech Stack
- **Language**: C++17
- **Build System**: CMake (v3.14+)
- **Testing Framework**: Google Test (GTest) via CMake `FetchContent`
- **Visualization Tooling**: Graphviz (`dot` language)

---

## Directory Structure
```text
OpenATPG/
├── CMakeLists.txt        # CMake build configuration and GTest FetchContent
├── .gitignore            # Ignores CMake build artifacts and binaries
├── README.md             # Project documentation
├── web_simulator.html    # Interactive HTML/JS simulator for visual learning
├── examples/
│   └── simple_and_or.v   # Structural Verilog test circuit (W1 = A & B; Y = W1 | C)
├── src/
│   ├── main.cpp          # CLI entry point, argument parsing, and engine orchestration
│   ├── lexer.h / .cpp    # FSM tokenization of Verilog syntax
│   ├── parser.h / .cpp   # Recursive descent parsing into AST
│   ├── netlist.h / .cpp  # DAG construction and node management
│   ├── simulator.h / .cpp# Topological logic evaluation
│   ├── atpg.h / .cpp     # Fault injection and test pattern generation
│   └── visualizer.h/.cpp # Graphviz DOT file generation
└── tests/
    └── test_main.cpp     # Unit tests for Lexer and Parser using Google Test
```

---

## Prerequisites
To compile and run OpenATPG, ensure your system has the following installed:
1. **C++ Compiler**: A compiler supporting C++17 (e.g., MSVC on Windows, GCC/Clang on Linux/macOS).
2. **CMake**: Version 3.14 or higher.
3. **Git**: Required by CMake to fetch Google Test during the build process.
4. **Graphviz (Optional)**: Required only if you wish to convert the generated `.dot` visual files into PNG/PDFs.

---

## Installation Guide

Follow these exact terminal commands to configure and build the project from scratch. 

**1. Navigate to the project root:**
```bash
cd OpenATPG
```

**2. Create a build directory:**
```bash
mkdir build
cd build
```

**3. Configure the project with CMake:**
*(Note: `CMAKE_TLS_VERIFY=OFF` is strictly hardcoded in the `CMakeLists.txt` to bypass corporate firewall/SSL issues when fetching GTest).*
```bash
cmake ..
```

**4. Compile the executables:**
```bash
cmake --build .
```

---

## Usage & Running Commands

The executable (`atpg_engine.exe` on Windows) is generated in the `build/Debug` (or `build/Release`) directory depending on your generator.

**Run the ATPG Engine against the test circuit:**
```bash
cd Debug
./atpg_engine.exe --input ../../examples/simple_and_or.v --visualize
```

**Expected Console Output:**
```text
ATPG Fault Coverage Report
----------------------------------------
Detected SA0 at node Y with vector: A=0 B=0 C=1 
Detected SA1 at node Y with vector: A=0 B=0 C=0 
Detected SA0 at node A with vector: A=1 B=1 C=0 
Detected SA1 at node A with vector: A=0 B=1 C=0 
...
----------------------------------------
Total Faults: 10
Detected Faults: 10
Fault Coverage: 100%
```

**Viewing the Visualizer Output:**
The `--visualize` flag triggers the engine to output several `.dot` files in your current directory (e.g., `circuit_A_SA0.dot`). 
To convert these to PNG images using Graphviz, run:
```bash
dot -Tpng circuit_A_SA0.dot -o circuit_A_SA0.png
```

---

## Testing

OpenATPG ships with a Google Test suite to verify the integrity of the Lexer and Parser. CMake automatically fetches GTest and compiles the test binary (`atpg_tests.exe`).

To run the unit tests, execute `ctest` from your `build` directory:
```bash
cd build
ctest -C Debug --output-on-failure
```
**Expected Output:**
```text
1/2 Test #1: LexerTest.BasicTokens ............   Passed    0.01 sec
2/2 Test #2: ParserTest.BasicParse ............   Passed    0.01 sec
100% tests passed, 0 tests failed out of 2
```

---

## Interactive Web Simulator
To make this project completely easy to understand for beginners, a standalone HTML/JS visualization tool is included. 

Simply open **`web_simulator.html`** in any web browser. It provides a visual, interactive simulation of the `simple_and_or.v` circuit. You can manually inject Stuck-At faults using dropdown menus and instantly see how the ATPG engine calculates the test vectors and propagates the logic (using standard `D`/`D'` notation) to detect the defect.

---

## Author & License
**Author**: Md. Azharul Islam  
**License**: MIT License 

```text
MIT License

Copyright (c) 2026 Md. Azharul Islam

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction...
```
