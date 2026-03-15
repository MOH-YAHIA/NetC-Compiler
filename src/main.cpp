#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem> // Required for folder logic
#include "scanner.h"
#include "parser.h"
#include "semantic.h"
#include "token.h"
#include "ast.h"

using namespace std;
namespace fs = std::filesystem;

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
    cout << "  -s, --scan-only      Run scanner only (skip parsing)\n";
    cout << "  -p, --parse-only      Run parser only (skip semantic analysis)\n";
    cout << "  --show-ast            Display the Abstract Syntax Tree\n";
    cout << "  --show-symbols        Display symbol table after semantic analysis\n";
}

int main(int argc, char* argv[]) {
    initializeTokenTypeNames();
    
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    string filename = argv[1];
    bool scanOnly = false, parseOnly = false, showAST = false, showSymbols = false;
    
    for (int i = 2; i < argc; i++) {
        string arg = argv[i];
        if (arg == "-s" || arg == "--scan-only") scanOnly = true;
        else if (arg == "-p" || arg == "--parse-only") parseOnly = true;
        else if (arg == "--show-ast") showAST = true;
        else if (arg == "--show-symbols") showSymbols = true;
    }

    // ==================== SCANNER PHASE ====================
    cout << "PHASE 1: LEXICAL ANALYSIS (SCANNER)\n--------------------------------------------\n";
    string sourceCode = readFile(filename);
    Scanner scanner(sourceCode);
    vector<Token> tokens = scanner.scanTokens();
    
    cout << "Scanning completed! Total tokens: " << tokens.size() << "\n";
    
    if (!parseOnly) {
        scanner.printTokens();
        
        // --- LOGIC CHANGE: Save to output/ folder ---

        // Extract filename without path/extension (e.g., "test.netc" -> "test")
        string baseName = fs::path(filename).stem().string();
        string outputFilename = "output/" + baseName + "_tokens.txt";
        
        ofstream outFile(outputFilename);
        if (outFile.is_open()) {
            outFile << "Token Analysis for: " << filename << "\n\n";
            outFile << "Line\tCol\tType\t\t\tLexeme\n----\t---\t----\t\t\t------\n";
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
    
    if (scanOnly) return 0;
    
    // ==================== PARSER PHASE ====================
    cout << "\nPHASE 2: SYNTAX ANALYSIS (PARSER)\n--------------------------------------------\n";
    Parser parser(tokens);
    auto ast = parser.parse();
    
    if (parser.hasError()) return 1;
    if (showAST && ast) ast->print(0);
    if (parseOnly) return 0;
    
    // ==================== SEMANTIC ANALYSIS PHASE ====================
    cout << "\nPHASE 3: SEMANTIC ANALYSIS\n--------------------------------------------\n";
    SemanticAnalyzer semanticAnalyzer;
    semanticAnalyzer.analyze(ast.get());
    
    if (semanticAnalyzer.hasError()) {
        if (showSymbols) {
            semanticAnalyzer.printSymbolTable();
            semanticAnalyzer.printFunctionTable();
        }
        return 1;
    }
    
    if (showSymbols) {
        semanticAnalyzer.printSymbolTable();
        semanticAnalyzer.printFunctionTable();
    }
    
    cout << "\n============================================\n All phases completed successfully!\n============================================\n";
    return 0;
}