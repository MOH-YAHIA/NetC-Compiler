#pragma once

#include <string>
#include <vector>
#include <memory>
#include "token.h"

using namespace std;

// Forward declarations
class ASTNode;
class ExprNode;
class StmtNode;

// Node type enumeration
enum class NodeType {
    // Program
    PROGRAM,
    
    // Statements
    DECLARATION,
    ASSIGNMENT,
    IF_STMT,
    UNTIL_STMT,
    ITERATE_STMT,
    NETWORK_STMT,
    INIT_STMT,
    RETURN_STMT,
    FEED_STMT,
    FORWARD_STMT,
    LINK_STMT,
    
    // Expressions
    BINARY_EXPR,
    UNARY_EXPR,
    LITERAL,
    IDENTIFIER,
    FUNCTION_CALL
};

// Base AST Node
class ASTNode {
public:
    NodeType type;
    int line;
    int column;
    
    ASTNode(NodeType t, int l = 0, int c = 0) 
        : type(t), line(l), column(c) {}
    virtual ~ASTNode() {} // ensure that when a unique_ptr<ASTNode> is deleted, the derived class (like BinaryExprNode) is cleaned up properly
    virtual void print(int indent = 0) = 0;
};

// ==================== Expression Nodes ====================

// Literal Node (numbers, strings, booleans)
class LiteralNode : public ASTNode {
public:
    TokenType literalType;  // INTEGER_LITERAL, FLOAT_LITERAL, etc.
    string value;
    
    LiteralNode(TokenType type, string val, int l = 0, int c = 0)
        : ASTNode(NodeType::LITERAL, l, c), literalType(type), value(val) {}
    
    void print(int indent = 0) override;
};

// Identifier Node
class IdentifierNode : public ASTNode {
public:
    string name;
    
    IdentifierNode(string n, int l = 0, int c = 0)
        : ASTNode(NodeType::IDENTIFIER, l, c), name(n) {}
    
    void print(int indent = 0) override;
};

// Binary Expression Node (a + b, x * y, etc.)
class BinaryExprNode : public ASTNode {
public:
    unique_ptr<ASTNode> left;
    TokenType op;
    unique_ptr<ASTNode> right;
    
    BinaryExprNode(unique_ptr<ASTNode> l, TokenType operation, unique_ptr<ASTNode> r, int line = 0, int col = 0)
        : ASTNode(NodeType::BINARY_EXPR, line, col), left(move(l)), op(operation), right(move(r)) {}
    
    void print(int indent = 0) override;
};

// Unary Expression Node (!, -, ~, ++, --)
class UnaryExprNode : public ASTNode {
public:
    TokenType op;
    unique_ptr<ASTNode> operand;
    
    UnaryExprNode(TokenType operation, unique_ptr<ASTNode> expr, int l = 0, int c = 0)
        : ASTNode(NodeType::UNARY_EXPR, l, c), op(operation), operand(move(expr)) {}
    
    void print(int indent = 0) override;
};

// Function Call Node
class FunctionCallNode : public ASTNode {
public:
    string functionName;
    vector<unique_ptr<ASTNode>> arguments;
    
    FunctionCallNode(string name, int l = 0, int c = 0)
        : ASTNode(NodeType::FUNCTION_CALL, l, c), functionName(name) {}
    
    void print(int indent = 0) override;
};

// ==================== Statement Nodes ====================

// Link Statement Node
class LinkStmtNode : public ASTNode {
public:
    string filename;
    
    LinkStmtNode(string file, int l = 0, int c = 0)
        : ASTNode(NodeType::LINK_STMT, l, c), filename(file) {}
    
    void print(int indent = 0) override;
};

// Declaration Node
class DeclarationNode : public ASTNode {
public:
    TokenType dataType;  // dnum, cnum, text, flag
    string varName;
    unique_ptr<ASTNode> initValue;  // nullptr if no initialization
    
    DeclarationNode(TokenType type, string name, int l = 0, int c = 0)
        : ASTNode(NodeType::DECLARATION, l, c), dataType(type), varName(name), initValue(nullptr) {}
    
    void print(int indent = 0) override;
};

// Assignment Node
class AssignmentNode : public ASTNode {
public:
    string varName;
    unique_ptr<ASTNode> value;
    
    AssignmentNode(string name, unique_ptr<ASTNode> val, int l = 0, int c = 0)
        : ASTNode(NodeType::ASSIGNMENT, l, c), varName(name), value(move(val)) {}
    
    void print(int indent = 0) override;
};

// If Statement Node
class IfStmtNode : public ASTNode {
public:
    unique_ptr<ASTNode> condition;
    vector<unique_ptr<ASTNode>> thenBody;
    vector<unique_ptr<ASTNode>> elseBody;
    
    IfStmtNode(unique_ptr<ASTNode> cond, int l = 0, int c = 0)
        : ASTNode(NodeType::IF_STMT, l, c), condition(move(cond)) {}
    
    void print(int indent = 0) override;
};

// Until Statement Node (while loop)
class UntilStmtNode : public ASTNode {
public:
    unique_ptr<ASTNode> condition;
    vector<unique_ptr<ASTNode>> body;
    
    UntilStmtNode(unique_ptr<ASTNode> cond, int l = 0, int c = 0)
        : ASTNode(NodeType::UNTIL_STMT, l, c), condition(move(cond)) {}
    
    void print(int indent = 0) override;
};

// Iterate Statement Node (for loop)
class IterateStmtNode : public ASTNode {
public:
    unique_ptr<ASTNode> init;      // initialization
    unique_ptr<ASTNode> condition;
    unique_ptr<ASTNode> update;    // increment/update
    vector<unique_ptr<ASTNode>> body;
    
    IterateStmtNode(int l = 0, int c = 0)
        : ASTNode(NodeType::ITERATE_STMT, l, c) {}
    
    void print(int indent = 0) override;
};

// Parameter for function
struct Parameter {
    TokenType type;
    string name;
};

// Network Statement Node (function definition)
class NetworkStmtNode : public ASTNode {
public:
    string functionName;
    vector<Parameter> parameters;
    vector<unique_ptr<ASTNode>> body;
    
    NetworkStmtNode(string name, int l = 0, int c = 0)
        : ASTNode(NodeType::NETWORK_STMT, l, c), functionName(name) {}
    
    void print(int indent = 0) override;
};

// Init Statement Node (main function)
class InitStmtNode : public ASTNode {
public:
    vector<unique_ptr<ASTNode>> body;
    
    InitStmtNode(int l = 0, int c = 0)
        : ASTNode(NodeType::INIT_STMT, l, c) {}
    
    void print(int indent = 0) override;
};

// Return Statement Node (yield)
class ReturnStmtNode : public ASTNode {
public:
    unique_ptr<ASTNode> returnValue;
    
    ReturnStmtNode(unique_ptr<ASTNode> val, int l = 0, int c = 0)
        : ASTNode(NodeType::RETURN_STMT, l, c), returnValue(move(val)) {}
    
    void print(int indent = 0) override;
};

// Feed Statement Node (input)
class FeedStmtNode : public ASTNode {
public:
    string varName;
    
    FeedStmtNode(string name, int l = 0, int c = 0)
        : ASTNode(NodeType::FEED_STMT, l, c), varName(name) {}
    
    void print(int indent = 0) override;
};

// Forward Statement Node (output)
class ForwardStmtNode : public ASTNode {
public:
    unique_ptr<ASTNode> expression;
    
    ForwardStmtNode(unique_ptr<ASTNode> expr, int l = 0, int c = 0)
        : ASTNode(NodeType::FORWARD_STMT, l, c), expression(move(expr)) {}
    
    void print(int indent = 0) override;
};

// ==================== Program Node ====================

// Program Node (root of AST)
class ProgramNode : public ASTNode {
public:
    vector<unique_ptr<ASTNode>> statements;
    
    ProgramNode() : ASTNode(NodeType::PROGRAM, 0, 0) {}
    
    void print(int indent = 0) override;
};

// Helper function to print indentation
void printIndent(int indent);
