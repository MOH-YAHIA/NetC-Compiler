<h1 align="center"> NetC Compiler </h1>

**NetC** is a domain-specific language designed with a focus on **logic-flow** and **network-like structures**. This project implements a lightweight compiler pipeline—including a Scanner, Parser, and Semantic Analyzer—designed to transform NetC source into a validated Abstract Syntax Tree (AST).

---

##  Project Architecture
The compiler is built in C++17 and follows a classic three-phase analysis:
1. **Lexical Analysis (Scanner)**: Converts source code into a stream of tokens.
2. **Syntax Analysis (Parser)**: Generates an Abstract Syntax Tree (AST) using Recursive Descent.
3. **Semantic Analysis**: Enforces scope rules, type checking, and variable initialization invariants.

---

##  NetC Syntax at a Glance

###  Data Types (The Primitives)
NetC uses a strict type system to ensure memory safety.

| Keyword | Logical Type | C++ Equivalent | Memory Logic |
| :--- | :--- | :--- | :--- |
| **`dnum`** | **Discrete Number** | `int` | Integer values for indexing and counting. |
| **`cnum`** | **Continuous Number** | `float` | Real numbers for calculations and weights. |
| **`text`** | **String Literal** | `std::string` | Sequence of characters for messages. |
| **`flag`** | **Boolean Logic** | `bool` | Binary state: `true` or `false`. |

###  Key Instructions
* **`network`**: Defines a function or logic block.
* **`init`**: The main entry point of the program (similar to `main` in C++).
* **`yield`**: Returns a value from a `network` function.
* **`feed` / `forward`**: Input and Output operations.
* **`iterate` / `until`**: Loop structures for repetitive logic.

---

##  Usage
### Command Line Interface
Run the compiler against a `.netc` file using the following syntax:
```bash
./bin/netc tests/example.netc [options]
```
**Available Options:**
* **`--show-ast`**: Prints the Abstract Syntax Tree to the console.
* **`--show-symbols`**: Displays the Symbol Table and Function Table.
* **`-s`** / **`--scan-only`**: Executes only the Lexical Analysis phase.
* **`-p`** / **`--parse-only`**: Executes only the Syntax Analysis phase.

### Graphical User Interface
To use the interactive Streamlit-based GUI:
1. Install streamlit
```bash
pip install streamlit
```

2. Navigate to the project root then run the app:
```Bash
streamlit run src/netc_streamlit.py
```


## Project Structure
```text
NetC-Compiler/
├── bin/             # Compiled executable binaries
├── docs/            # Documentation for the language and compiler
├── include/         # Header files (.h)
├── output/          # Generated token lists and analysis results
├── src/             # Source code files (.cpp)
├── tests/           # Sample NetC source files (.netc)
├── .gitignore       # Files and folders to be ignored by Git
└── README.md        # Project overview and documentation
```