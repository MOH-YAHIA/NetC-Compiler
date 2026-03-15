#include <iostream>
#include <fstream>
#include <sstream>
#include "../include/scanner.h"
#include "../include/parser.h"
#include "../include/semantic.h"
#include "../include/token.h"
#include "../include/ast.h"

using namespace std;

// Function to read entire file into a string
string readFile(const string& filename) {
    ifstream file(filename);
    
    if (!file.is_open()) {
        cerr << "Error: Could not open file '" << filename << "'" << endl;
        exit(1);
    }
    
    stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    return buffer.str();
}

// Display usage information
void printUsage(const string& programName) {
    cout << "NetC Compiler - Scanner, Parser, and Semantic Analyzer\n";
    cout << "Usage: " << programName << " <input_file.netc> [options]\n";
    cout << "Options:\n";
    cout << "  -s, --scan-only       Run scanner only (skip parsing)\n";
    cout << "  -p, --parse-only      Run parser only (skip semantic analysis)\n";
    cout << "  --show-ast            Display the Abstract Syntax Tree\n";
    cout << "  --show-symbols        Display symbol table after semantic analysis\n";
    cout << "Example: " << programName << " test.netc --show-ast\n";
}

int main(int argc, char* argv[]) {
    // Initialize token type names for printing
    initializeTokenTypeNames();
    
    // Parse command line arguments
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    string filename = argv[1];
    bool scanOnly = false;
    bool parseOnly = false;
    bool showAST = false;
    bool showSymbols = false;
    
    // Check for options
    for (int i = 2; i < argc; i++) {
        string arg = argv[i];
        if (arg == "-s" || arg == "--scan-only") {
            scanOnly = true;
        } else if (arg == "-p" || arg == "--parse-only") {
            parseOnly = true;
        } else if (arg == "--show-ast") {
            showAST = true;
        } else if (arg == "--show-symbols") {
            showSymbols = true;
        }
    }
    
    cout << "============================================\n";
    cout << "NetC Compiler - Full Analysis Pipeline\n";
    cout << "============================================\n";
    cout << "Input file: " << filename << "\n";
    cout << "============================================\n\n";
    
    // ==================== SCANNER PHASE ====================
    cout << "PHASE 1: LEXICAL ANALYSIS (SCANNER)\n";
    cout << "--------------------------------------------\n";
    
    // Read source file
    string sourceCode = readFile(filename);
    
    // Create scanner and tokenize
    Scanner scanner(sourceCode);
    vector<Token> tokens = scanner.scanTokens();
    
    cout << "Scanning completed!\n";
    cout << "Total tokens found: " << tokens.size() << "\n";
    
    // Display tokens if not parse-only mode
    if (!parseOnly) {
        scanner.printTokens();
        
        // Save tokens to file
        string outputFilename = filename.substr(0, filename.find_last_of('.')) + "_tokens.txt";
        ofstream outFile(outputFilename);
        
        if (outFile.is_open()) {
            outFile << "Token Analysis for: " << filename << "\n\n";
            outFile << "Line\tCol\tType\t\t\tLexeme\n";
            outFile << "----\t---\t----\t\t\t------\n";
            
            for (const auto& token : tokens) {
                if (token.type != COMMENT) {
                    outFile << token.line << "\t" << token.column << "\t" 
                           << tokenTypeToString(token.type) << "\t\t";
                    if (tokenTypeToString(token.type).length() < 16) outFile << "\t";
                    if (tokenTypeToString(token.type).length() < 8) outFile << "\t";
                    outFile << token.lexeme << "\n";
                }
            }
            
            outFile.close();
            cout << "\nToken list saved to: " << outputFilename << "\n";
        }
    }
    
    // If scan-only mode, exit here
    if (scanOnly) {
        cout << "\n============================================\n";
        cout << "Scan-only mode: Parsing skipped\n";
        cout << "============================================\n";
        return 0;
    }
    
    // ==================== PARSER PHASE ====================
    cout << "\n\n";
    cout << "PHASE 2: SYNTAX ANALYSIS (PARSER)\n";
    cout << "--------------------------------------------\n";
    
    // Create parser and parse (builds AST)
    Parser parser(tokens);
    auto ast = parser.parse();
    
    // Check for errors
    if (parser.hasError()) {
        cout << "\n============================================\n";
        cout << "Parsing failed with errors!\n";
        cout << "============================================\n";
        return 1;
    }
    
    // Print the AST if requested
    if (showAST && ast) {
        cout << "\n";
        ast->print(0);
    }
    
    // If parse-only mode, exit here
    if (parseOnly) {
        cout << "\n============================================\n";
        cout << "Parse-only mode: Semantic analysis skipped\n";
        cout << "============================================\n";
        return 0;
    }
    
    // ==================== SEMANTIC ANALYSIS PHASE ====================
    cout << "\n\n";
    cout << "PHASE 3: SEMANTIC ANALYSIS\n";
    cout << "--------------------------------------------\n";
    
    // Create semantic analyzer and analyze
    SemanticAnalyzer semanticAnalyzer;
    semanticAnalyzer.analyze(ast.get());
    
    // Check for semantic errors
    if (semanticAnalyzer.hasError()) {
        cout << "\n============================================\n";
        cout << "Semantic analysis failed with " << semanticAnalyzer.getErrorCount() << " error(s)!\n";
        cout << "============================================\n";
        
        // Still show symbol table if requested
        if (showSymbols) {
            semanticAnalyzer.printSymbolTable();
            semanticAnalyzer.printFunctionTable();
        }
        
        return 1;
    }
    
    // Show symbol table if requested
    if (showSymbols) {
        semanticAnalyzer.printSymbolTable();
        semanticAnalyzer.printFunctionTable();
    }
    
    cout << "\n============================================\n";
    cout << "✓ All phases completed successfully!\n";
    cout << "============================================\n";
    cout << "✓ Lexical Analysis   - No errors\n";
    cout << "✓ Syntax Analysis    - No errors\n";
    cout << "✓ Semantic Analysis  - No errors\n";
    cout << "============================================\n";
    
    return 0;
}