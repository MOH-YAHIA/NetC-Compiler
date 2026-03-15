#pragma once

#include <vector>
#include <string>
#include <memory>
#include "token.h"
#include "ast.h"

using namespace std;

// Parser class - performs syntax analysis on NetC token stream
class Parser {
private:
    vector<Token> tokens;       // List of tokens from scanner
    int current;                // Current token position
    bool hadError;              // Track if any errors occurred

    unique_ptr<ProgramNode> ast;       // The Abstract Syntax Tree
    
    // Utility methods
    Token peek();               // Look at current token
    Token previous();           // Look at previous token
    Token advance();            // Consume and return current token
    bool isAtEnd();            // Check if at end of tokens
    bool check(TokenType type); // Check if current token is of type
    bool match(TokenType type); // Check and consume if matches
    bool match(vector<TokenType> types); // Check multiple types
    
    // Error handling
    void error(string message);
    void synchronize();         // Recover from errors
    
    // Grammar rules - now return AST nodes
    unique_ptr<ProgramNode> program();
    void statementList(vector<unique_ptr<ASTNode>>& statements);
    unique_ptr<ASTNode> statement();
    unique_ptr<LinkStmtNode> linkStmt();
    unique_ptr<DeclarationNode> declaration();
    unique_ptr<AssignmentNode> assignment();
    unique_ptr<IfStmtNode> ifStmt();
    unique_ptr<UntilStmtNode> untilStmt();
    unique_ptr<IterateStmtNode> iterateStmt();
    unique_ptr<NetworkStmtNode> networkStmt();
    unique_ptr<InitStmtNode> initStmt();
    unique_ptr<ReturnStmtNode> returnStmt();
    unique_ptr<FeedStmtNode> feedStmt();
    unique_ptr<ForwardStmtNode> forwardStmt();
    void parameterList(vector<Parameter>& params);
    void argumentList(vector<unique_ptr<ASTNode>>& args);
    unique_ptr<ASTNode> condition();
    unique_ptr<ASTNode> expr();
    unique_ptr<ASTNode> term();
    unique_ptr<ASTNode> factor();
    
    // Helper methods
    bool isDataType(TokenType type);
    bool isAddOp(TokenType type);
    bool isMulOp(TokenType type);
    bool isRelOp(TokenType type);
    bool isLogicalOp(TokenType type);
    bool isUnaryOp(TokenType type);
    
public:
    Parser(vector<Token> tokens);
    unique_ptr<ProgramNode> parse();  // Returns AST root
    bool hasError();                   // Check if parsing had errors
    void printAST();                   // Print the AST
    
    
};

