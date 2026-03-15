#include "ast.h"
#include <iostream>

using namespace std;

// Helper function to print indentation
void printIndent(int indent) {
    for (int i = 0; i < indent; i++) {
        cout << "  ";
    }
}

// ==================== Expression Nodes ====================

void LiteralNode::print(int indent) {
    printIndent(indent);
    cout << "Literal: " << value << " (";
    cout << tokenTypeToString(literalType) << ")" << endl;
}

void IdentifierNode::print(int indent) {
    printIndent(indent);
    cout << "Identifier: " << name << endl;
}

void BinaryExprNode::print(int indent) {
    printIndent(indent);
    cout << "BinaryExpr (" << tokenTypeToString(op) << ")" << endl;
    
    printIndent(indent);
    cout << "├─ Left:" << endl;
    left->print(indent + 1);
    
    printIndent(indent);
    cout << "└─ Right:" << endl;
    right->print(indent + 1);
}

void UnaryExprNode::print(int indent) {
    printIndent(indent);
    cout << "UnaryExpr (" << tokenTypeToString(op) << ")" << endl;
    operand->print(indent + 1);
}

void FunctionCallNode::print(int indent) {
    printIndent(indent);
    cout << "FunctionCall: " << functionName << "()" << endl;
    
    if (!arguments.empty()) {
        printIndent(indent);
        cout << "Arguments:" << endl;
        for (auto& arg : arguments) {
            arg->print(indent + 1);
        }
    }
}

// ==================== Statement Nodes ====================

void LinkStmtNode::print(int indent) {
    printIndent(indent);
    cout << "Link: " << filename << endl;
}

void DeclarationNode::print(int indent) {
    printIndent(indent);
    cout << "Declaration" << endl;
    
    printIndent(indent + 1);
    cout << "Type: " << tokenTypeToString(dataType) << endl;
    
    printIndent(indent + 1);
    cout << "Name: " << varName << endl;
    
    if (initValue) {
        printIndent(indent + 1);
        cout << "InitValue:" << endl;
        initValue->print(indent + 2);
    }
}

void AssignmentNode::print(int indent) {
    printIndent(indent);
    cout << "Assignment" << endl;
    
    printIndent(indent + 1);
    cout << "Variable: " << varName << endl;
    
    printIndent(indent + 1);
    cout << "Value:" << endl;
    value->print(indent + 2);
}

void IfStmtNode::print(int indent) {
    printIndent(indent);
    cout << "IfStmt" << endl;
    
    printIndent(indent + 1);
    cout << "Condition:" << endl;
    condition->print(indent + 2);
    
    printIndent(indent + 1);
    cout << "Then:" << endl;
    for (auto& stmt : thenBody) {
        stmt->print(indent + 2);
    }
    
    if (!elseBody.empty()) {
        printIndent(indent + 1);
        cout << "Else:" << endl;
        for (auto& stmt : elseBody) {
            stmt->print(indent + 2);
        }
    }
}

void UntilStmtNode::print(int indent) {
    printIndent(indent);
    cout << "UntilStmt" << endl;
    
    printIndent(indent + 1);
    cout << "Condition:" << endl;
    condition->print(indent + 2);
    
    printIndent(indent + 1);
    cout << "Body:" << endl;
    for (auto& stmt : body) {
        stmt->print(indent + 2);
    }
}

void IterateStmtNode::print(int indent) {
    printIndent(indent);
    cout << "IterateStmt" << endl;
    
    if (init) {
        printIndent(indent + 1);
        cout << "Init:" << endl;
        init->print(indent + 2);
    }
    
    if (condition) {
        printIndent(indent + 1);
        cout << "Condition:" << endl;
        condition->print(indent + 2);
    }
    
    if (update) {
        printIndent(indent + 1);
        cout << "Update:" << endl;
        update->print(indent + 2);
    }
    
    printIndent(indent + 1);
    cout << "Body:" << endl;
    for (auto& stmt : body) {
        stmt->print(indent + 2);
    }
}

void NetworkStmtNode::print(int indent) {
    printIndent(indent);
    cout << "NetworkStmt: " << functionName << endl;
    
    if (!parameters.empty()) {
        printIndent(indent + 1);
        cout << "Parameters:" << endl;
        for (const auto& param : parameters) {
            printIndent(indent + 2);
            cout << tokenTypeToString(param.type) << " " << param.name << endl;
        }
    }
    
    printIndent(indent + 1);
    cout << "Body:" << endl;
    for (auto& stmt : body) {
        stmt->print(indent + 2);
    }
}

void InitStmtNode::print(int indent) {
    printIndent(indent);
    cout << "InitStmt (main)" << endl;
    
    printIndent(indent + 1);
    cout << "Body:" << endl;
    for (auto& stmt : body) {
        stmt->print(indent + 2);
    }
}

void ReturnStmtNode::print(int indent) {
    printIndent(indent);
    cout << "ReturnStmt (yield)" << endl;
    
    if (returnValue) {
        returnValue->print(indent + 1);
    }
}

void FeedStmtNode::print(int indent) {
    printIndent(indent);
    cout << "FeedStmt: " << varName << endl;
}

void ForwardStmtNode::print(int indent) {
    printIndent(indent);
    cout << "ForwardStmt" << endl;
    
    if (expression) {
        expression->print(indent + 1);
    }
}

// ==================== Program Node ====================

void ProgramNode::print(int indent) {
    printIndent(indent);
    cout << "Program" << endl;
    
    for (auto& stmt : statements) {
        stmt->print(indent + 1);
    }
}