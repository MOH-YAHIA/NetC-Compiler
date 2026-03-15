#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include "ast.h"
#include "token.h"

using namespace std;

// Symbol information stored in symbol table
struct Symbol {
    string name;
    TokenType type;      // dnum, cnum, text, flag
    int line;            // Where it was declared
    int column;
    bool initialized;    // Has it been assigned a value?
    
    Symbol(string n, TokenType t, int l, int c, bool init = false)
        : name(n), type(t), line(l), column(c), initialized(init) {}
};

// Function information
struct FunctionInfo {
    string name;
    TokenType returnType;  // For now, we'll track if it returns something
    vector<Parameter> parameters;
    int line;
    int column;
    
    FunctionInfo(string n, int l, int c)
        : name(n), returnType(DNUM), line(l), column(c) {}
};

// Semantic Analyzer class
class SemanticAnalyzer {
private:
    // Symbol table - maps variable name to symbol info
    map<string, Symbol> symbolTable;
    
    // Function table - maps function name to function info
    map<string, FunctionInfo> functionTable;
    
    // Error tracking
    bool hadError;
    int errorCount;
    
    // Current scope tracking (for nested blocks)
    vector<map<string, Symbol>> scopeStack;
    
    // Helper methods for symbol table
    void enterScope();
    void exitScope();
    bool isDeclaredInCurrentScope(const string& name);
    bool isDeclaredInAnyScope(const string& name);
    Symbol* lookupSymbol(const string& name);
    void addSymbol(const string& name, const Symbol& symbol);
    
    // Helper methods for function table
    bool isFunctionDeclared(const string& name);
    void addFunction(const string& name, const FunctionInfo& funcInfo);
    FunctionInfo* lookupFunction(const string& name);
    
    // Type checking helpers
    TokenType getExpressionType(ASTNode* expr);
    bool areTypesCompatible(TokenType type1, TokenType type2);
    string typeToString(TokenType type);
    
    // Error reporting
    void error(const string& message, int line, int column);
    void warning(const string& message, int line, int column);
    
    // Analysis methods for each node type
    void analyzeProgram(ProgramNode* node);
    void analyzeStatement(ASTNode* node);
    void analyzeLinkStmt(LinkStmtNode* node);
    void analyzeDeclaration(DeclarationNode* node);
    void analyzeAssignment(AssignmentNode* node);
    void analyzeIfStmt(IfStmtNode* node);
    void analyzeUntilStmt(UntilStmtNode* node);
    void analyzeIterateStmt(IterateStmtNode* node);
    void analyzeNetworkStmt(NetworkStmtNode* node);
    void analyzeInitStmt(InitStmtNode* node);
    void analyzeReturnStmt(ReturnStmtNode* node);
    void analyzeFeedStmt(FeedStmtNode* node);
    void analyzeForwardStmt(ForwardStmtNode* node);
    void analyzeExpression(ASTNode* expr);
    void analyzeFunctionCall(FunctionCallNode* node);
    
public:
    SemanticAnalyzer();
    
    // Main analysis method
    void analyze(ProgramNode* ast);
    
    // Error checking
    bool hasError() const;
    int getErrorCount() const;
    
    // Symbol table access (for debugging/printing)
    void printSymbolTable();
    void printFunctionTable();
};
