#include "../include/semantic.h"
#include <iostream>

using namespace std;

// Constructor
SemanticAnalyzer::SemanticAnalyzer() : hadError(false), errorCount(0) {
    // Initialize with global scope
    scopeStack.push_back(map<string, Symbol>());
}

// ==================== Scope Management ====================

void SemanticAnalyzer::enterScope() {
    scopeStack.push_back(map<string, Symbol>());
    cout << "  [Entering new scope, depth: " << scopeStack.size() << "]" << endl;
}

void SemanticAnalyzer::exitScope() {
    if (scopeStack.size() > 1) {
        scopeStack.pop_back();
        cout << "  [Exiting scope, depth: " << scopeStack.size() << "]" << endl;
    }
}

bool SemanticAnalyzer::isDeclaredInCurrentScope(const string& name) {
    if (scopeStack.empty()) return false;
    return scopeStack.back().find(name) != scopeStack.back().end();
}

bool SemanticAnalyzer::isDeclaredInAnyScope(const string& name) {
    for (auto it = scopeStack.rbegin(); it != scopeStack.rend(); ++it) {
        if (it->find(name) != it->end()) {
            return true;
        }
    }
    return false;
}

Symbol* SemanticAnalyzer::lookupSymbol(const string& name) {
    // Search from innermost to outermost scope
    for (auto it = scopeStack.rbegin(); it != scopeStack.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) {
            return &(found->second);
        }
    }
    return nullptr;
}

void SemanticAnalyzer::addSymbol(const string& name, const Symbol& symbol) {
    if (!scopeStack.empty()) {
        //scopeStack.back()[name] = symbol;
        scopeStack.back().insert_or_assign(name, symbol);
    }
}

// ==================== Function Table Management ====================

bool SemanticAnalyzer::isFunctionDeclared(const string& name) {
    return functionTable.find(name) != functionTable.end();
}

void SemanticAnalyzer::addFunction(const string& name, const FunctionInfo& funcInfo) {
    //functionTable[name] = funcInfo;
    functionTable.insert_or_assign(name, funcInfo);
}

FunctionInfo* SemanticAnalyzer::lookupFunction(const string& name) {
    auto it = functionTable.find(name);
    if (it != functionTable.end()) {
        return &(it->second);
    }
    return nullptr;
}

// ==================== Type Checking ====================

TokenType SemanticAnalyzer::getExpressionType(ASTNode* expr) {
    if (!expr) return UNKNOWN;
    
    switch (expr->type) {
        case NodeType::LITERAL: {
            auto lit = dynamic_cast<LiteralNode*>(expr);
            if (lit->literalType == INTEGER_LITERAL) return DNUM;
            if (lit->literalType == FLOAT_LITERAL) return CNUM;
            if (lit->literalType == STRING_LITERAL) return TEXT;
            if (lit->literalType == BOOLEAN_LITERAL) return FLAG;
            return UNKNOWN;
        }
        
        case NodeType::IDENTIFIER: {
            auto id = dynamic_cast<IdentifierNode*>(expr);
            Symbol* sym = lookupSymbol(id->name);
            if (sym) {
                return sym->type;
            }
            return UNKNOWN;
        }
        
        case NodeType::BINARY_EXPR: {
            auto binExpr = dynamic_cast<BinaryExprNode*>(expr);
            TokenType leftType = getExpressionType(binExpr->left.get());
            TokenType rightType = getExpressionType(binExpr->right.get());
            
            // Arithmetic operations: if either is cnum, result is cnum
            if (binExpr->op == PLUS || binExpr->op == MINUS || 
                binExpr->op == MULTIPLY || binExpr->op == DIVIDE) {
                if (leftType == CNUM || rightType == CNUM) return CNUM;
                return DNUM;
            }
            
            // Comparison operations: result is flag (boolean)
            if (binExpr->op == EQ || binExpr->op == NEQ || 
                binExpr->op == LT || binExpr->op == GT ||
                binExpr->op == LTE || binExpr->op == GTE) {
                return FLAG;
            }
            
            // Logical operations: result is flag
            if (binExpr->op == AND || binExpr->op == OR) {
                return FLAG;
            }
            
            return leftType;
        }
        
        case NodeType::UNARY_EXPR: {
            auto unExpr = dynamic_cast<UnaryExprNode*>(expr);
            
            // Logical NOT: result is flag
            if (unExpr->op == NOT) return FLAG;
            
            // Arithmetic negation: preserves type
            return getExpressionType(unExpr->operand.get());
        }
        
        case NodeType::FUNCTION_CALL: {
            // For now, assume functions return dnum
            // In a real compiler, you'd look up the function's return type
            return DNUM;
        }
        
        default:
            return UNKNOWN;
    }
}

bool SemanticAnalyzer::areTypesCompatible(TokenType type1, TokenType type2) {
    if (type1 == type2) return true;
    
    // Allow dnum to cnum conversion (int to float)
    if (type1 == CNUM && type2 == DNUM) return true;
    
    // All other combinations are incompatible
    return false;
}

string SemanticAnalyzer::typeToString(TokenType type) {
    return tokenTypeToString(type);
}

// ==================== Error Reporting ====================

void SemanticAnalyzer::error(const string& message, int line, int column) {
    cerr << "Semantic Error at line " << line << ", column " << column 
         << ": " << message << endl;
    hadError = true;
    errorCount++;
}

void SemanticAnalyzer::warning(const string& message, int line, int column) {
    cout << "Warning at line " << line << ", column " << column 
         << ": " << message << endl;
}

// ==================== Analysis Methods ====================

void SemanticAnalyzer::analyzeProgram(ProgramNode* node) {
    cout << "Starting semantic analysis..." << endl;
    
    for (auto& stmt : node->statements) {
        analyzeStatement(stmt.get());
    }
    
    if (!hadError) {
        cout << "\n=== Semantic analysis completed successfully! ===" << endl;
        cout << "No semantic errors found." << endl;
    } else {
        cout << "\n=== Semantic analysis completed with " << errorCount << " error(s) ===" << endl;
    }
}

void SemanticAnalyzer::analyzeStatement(ASTNode* node) {
    if (!node) return;
    
    switch (node->type) {
        case NodeType::LINK_STMT:
            analyzeLinkStmt(dynamic_cast<LinkStmtNode*>(node));
            break;
        case NodeType::DECLARATION:
            analyzeDeclaration(dynamic_cast<DeclarationNode*>(node));
            break;
        case NodeType::ASSIGNMENT:
            analyzeAssignment(dynamic_cast<AssignmentNode*>(node));
            break;
        case NodeType::IF_STMT:
            analyzeIfStmt(dynamic_cast<IfStmtNode*>(node));
            break;
        case NodeType::UNTIL_STMT:
            analyzeUntilStmt(dynamic_cast<UntilStmtNode*>(node));
            break;
        case NodeType::ITERATE_STMT:
            analyzeIterateStmt(dynamic_cast<IterateStmtNode*>(node));
            break;
        case NodeType::NETWORK_STMT:
            analyzeNetworkStmt(dynamic_cast<NetworkStmtNode*>(node));
            break;
        case NodeType::INIT_STMT:
            analyzeInitStmt(dynamic_cast<InitStmtNode*>(node));
            break;
        case NodeType::RETURN_STMT:
            analyzeReturnStmt(dynamic_cast<ReturnStmtNode*>(node));
            break;
        case NodeType::FEED_STMT:
            analyzeFeedStmt(dynamic_cast<FeedStmtNode*>(node));
            break;
        case NodeType::FORWARD_STMT:
            analyzeForwardStmt(dynamic_cast<ForwardStmtNode*>(node));
            break;
        default:
            break;
    }
}

void SemanticAnalyzer::analyzeLinkStmt(LinkStmtNode* node) {
    cout << "Analyzing link statement: " << node->filename << endl;
    // Link statements don't need semantic checking beyond syntax
}

void SemanticAnalyzer::analyzeDeclaration(DeclarationNode* node) {
    cout << "Analyzing declaration: " << typeToString(node->dataType) 
         << " " << node->varName << endl;
    
    // Check if variable already declared in current scope
    if (isDeclaredInCurrentScope(node->varName)) {
        error("Variable '" + node->varName + "' is already declared in this scope",
              node->line, node->column);
        return;
    }
    
    // Check initialization type if present
    if (node->initValue) {
        TokenType exprType = getExpressionType(node->initValue.get());
        
        if (exprType == UNKNOWN) {
            error("Cannot determine type of initialization expression",
                  node->line, node->column);
        } else if (!areTypesCompatible(node->dataType, exprType)) {
            error("Type mismatch: cannot initialize " + typeToString(node->dataType) +
                  " with " + typeToString(exprType),
                  node->line, node->column);
        }
        
        // Analyze the initialization expression
        analyzeExpression(node->initValue.get());
        
        // Add to symbol table as initialized
        addSymbol(node->varName, Symbol(node->varName, node->dataType, 
                                        node->line, node->column, true));
    } else {
        // Add to symbol table as uninitialized
        addSymbol(node->varName, Symbol(node->varName, node->dataType, 
                                        node->line, node->column, false));
        
        warning("Variable '" + node->varName + "' declared but not initialized",
                node->line, node->column);
    }
}

void SemanticAnalyzer::analyzeAssignment(AssignmentNode* node) {
    cout << "Analyzing assignment to: " << node->varName << endl;
    
    // Check if variable is declared
    if (!isDeclaredInAnyScope(node->varName)) {
        error("Variable '" + node->varName + "' used before declaration",
              node->line, node->column);
        return;
    }
    
    // Get variable type
    Symbol* sym = lookupSymbol(node->varName);
    if (!sym) {
        error("Internal error: symbol not found",
              node->line, node->column);
        return;
    }
    
    // Check type compatibility
    TokenType exprType = getExpressionType(node->value.get());
    
    if (exprType == UNKNOWN) {
        error("Cannot determine type of expression",
              node->line, node->column);
    } else if (!areTypesCompatible(sym->type, exprType)) {
        error("Type mismatch: cannot assign " + typeToString(exprType) +
              " to variable of type " + typeToString(sym->type),
              node->line, node->column);
    }
    
    // Analyze the value expression
    analyzeExpression(node->value.get());
    
    // Mark variable as initialized
    sym->initialized = true;
}

void SemanticAnalyzer::analyzeIfStmt(IfStmtNode* node) {
    cout << "Analyzing if statement..." << endl;
    
    // Analyze condition
    analyzeExpression(node->condition.get());
    
    // Check condition type
    TokenType condType = getExpressionType(node->condition.get());
    if (condType != FLAG && condType != UNKNOWN) {
        warning("Condition should be of type 'flag' (boolean), got " + typeToString(condType),
                node->line, node->column);
    }
    
    // Analyze then body
    enterScope();
    for (auto& stmt : node->thenBody) {
        analyzeStatement(stmt.get());
    }
    exitScope();
    
    // Analyze else body if present
    if (!node->elseBody.empty()) {
        enterScope();
        for (auto& stmt : node->elseBody) {
            analyzeStatement(stmt.get());
        }
        exitScope();
    }
}

void SemanticAnalyzer::analyzeUntilStmt(UntilStmtNode* node) {
    cout << "Analyzing until loop..." << endl;
    
    // Analyze condition
    analyzeExpression(node->condition.get());
    
    // Check condition type
    TokenType condType = getExpressionType(node->condition.get());
    if (condType != FLAG && condType != UNKNOWN) {
        warning("Loop condition should be of type 'flag' (boolean), got " + typeToString(condType),
                node->line, node->column);
    }
    
    // Analyze body
    enterScope();
    for (auto& stmt : node->body) {
        analyzeStatement(stmt.get());
    }
    exitScope();
}

void SemanticAnalyzer::analyzeIterateStmt(IterateStmtNode* node) {
    cout << "Analyzing iterate loop..." << endl;
    
    enterScope();
    
    // Analyze initialization
    if (node->init) {
        analyzeStatement(node->init.get());
    }
    
    // Analyze condition
    if (node->condition) {
        analyzeExpression(node->condition.get());
        
        TokenType condType = getExpressionType(node->condition.get());
        if (condType != FLAG && condType != UNKNOWN) {
            warning("Loop condition should be of type 'flag' (boolean), got " + typeToString(condType),
                    node->line, node->column);
        }
    }
    
    // Analyze update
    if (node->update) {
        analyzeStatement(node->update.get());
    }
    
    // Analyze body
    for (auto& stmt : node->body) {
        analyzeStatement(stmt.get());
    }
    
    exitScope();
}

void SemanticAnalyzer::analyzeNetworkStmt(NetworkStmtNode* node) {
    cout << "Analyzing network function: " << node->functionName << endl;
    
    // Check if function already declared
    if (isFunctionDeclared(node->functionName)) {
        error("Function '" + node->functionName + "' is already declared",
              node->line, node->column);
        return;
    }
    
    // Add function to function table
    FunctionInfo funcInfo(node->functionName, node->line, node->column);
    funcInfo.parameters = node->parameters;
    addFunction(node->functionName, funcInfo);
    
    // Enter new scope for function body
    enterScope();
    
    // Add parameters to symbol table
    for (const auto& param : node->parameters) {
        addSymbol(param.name, Symbol(param.name, param.type, node->line, node->column, true));
    }
    
    // Analyze function body
    for (auto& stmt : node->body) {
        analyzeStatement(stmt.get());
    }
    
    exitScope();
}

void SemanticAnalyzer::analyzeInitStmt(InitStmtNode* node) {
    cout << "Analyzing init (main) function..." << endl;
    
    // Check if init already declared
    if (isFunctionDeclared("init")) {
        error("Multiple definitions of 'init' function",
              node->line, node->column);
        return;
    }
    
    // Add init to function table
    FunctionInfo funcInfo("init", node->line, node->column);
    addFunction("init", funcInfo);
    
    // Analyze body
    enterScope();
    for (auto& stmt : node->body) {
        analyzeStatement(stmt.get());
    }
    exitScope();
}

void SemanticAnalyzer::analyzeReturnStmt(ReturnStmtNode* node) {
    cout << "Analyzing return (yield) statement..." << endl;
    
    // Analyze return value expression
    if (node->returnValue) {
        analyzeExpression(node->returnValue.get());
    }
}

void SemanticAnalyzer::analyzeFeedStmt(FeedStmtNode* node) {
    cout << "Analyzing feed statement: " << node->varName << endl;
    
    // Check if variable is declared
    if (!isDeclaredInAnyScope(node->varName)) {
        error("Variable '" + node->varName + "' used before declaration",
              node->line, node->column);
        return;
    }
    
    // Mark variable as initialized (since we're reading into it)
    Symbol* sym = lookupSymbol(node->varName);
    if (sym) {
        sym->initialized = true;
    }
}

void SemanticAnalyzer::analyzeForwardStmt(ForwardStmtNode* node) {
    cout << "Analyzing forward statement..." << endl;
    
    // Analyze the expression being output
    if (node->expression) {
        analyzeExpression(node->expression.get());
    }
}

void SemanticAnalyzer::analyzeExpression(ASTNode* expr) {
    if (!expr) return;
    
    switch (expr->type) {
        case NodeType::IDENTIFIER: {
            auto id = dynamic_cast<IdentifierNode*>(expr);
            
            // Check if variable is declared
            if (!isDeclaredInAnyScope(id->name)) {
                error("Variable '" + id->name + "' used before declaration",
                      id->line, id->column);
                return;
            }
            
            // Check if variable is initialized
            Symbol* sym = lookupSymbol(id->name);
            if (sym && !sym->initialized) {
                warning("Variable '" + id->name + "' used before initialization",
                        id->line, id->column);
            }
            break;
        }
        
        case NodeType::BINARY_EXPR: {
            auto binExpr = dynamic_cast<BinaryExprNode*>(expr);
            analyzeExpression(binExpr->left.get());
            analyzeExpression(binExpr->right.get());
            
            // Check type compatibility for operation
            TokenType leftType = getExpressionType(binExpr->left.get());
            TokenType rightType = getExpressionType(binExpr->right.get());
            
            // String operations
            if (leftType == TEXT || rightType == TEXT) {
                if (binExpr->op != PLUS && binExpr->op != EQ && binExpr->op != NEQ) {
                    error("Invalid operation on text type",
                          binExpr->line, binExpr->column);
                }
            }
            
            // Boolean operations
            if (binExpr->op == AND || binExpr->op == OR) {
                if (leftType != FLAG || rightType != FLAG) {
                    error("Logical operators require flag (boolean) operands",
                          binExpr->line, binExpr->column);
                }
            }
            break;
        }
        
        case NodeType::UNARY_EXPR: {
            auto unExpr = dynamic_cast<UnaryExprNode*>(expr);
            analyzeExpression(unExpr->operand.get());
            
            // Check type compatibility
            TokenType operandType = getExpressionType(unExpr->operand.get());
            
            if (unExpr->op == NOT && operandType != FLAG) {
                error("Logical NOT requires flag (boolean) operand",
                      unExpr->line, unExpr->column);
            }
            break;
        }
        
        case NodeType::FUNCTION_CALL: {
            analyzeFunctionCall(dynamic_cast<FunctionCallNode*>(expr));
            break;
        }
        
        case NodeType::LITERAL:
            // Literals are always valid
            break;
        
        default:
            break;
    }
}

void SemanticAnalyzer::analyzeFunctionCall(FunctionCallNode* node) {
    cout << "Analyzing function call: " << node->functionName << "()" << endl;
    
    // Check if function is declared
    if (!isFunctionDeclared(node->functionName)) {
        error("Function '" + node->functionName + "' is not declared",
              node->line, node->column);
        return;
    }
    
    // Get function info
    FunctionInfo* funcInfo = lookupFunction(node->functionName);
    if (!funcInfo) return;
    
    // Check argument count
    if (node->arguments.size() != funcInfo->parameters.size()) {
        error("Function '" + node->functionName + "' expects " + 
              to_string(funcInfo->parameters.size()) + " arguments, got " +
              to_string(node->arguments.size()),
              node->line, node->column);
        return;
    }
    
    // Check argument types
    for (size_t i = 0; i < node->arguments.size(); i++) {
        analyzeExpression(node->arguments[i].get());
        
        TokenType argType = getExpressionType(node->arguments[i].get());
        TokenType paramType = funcInfo->parameters[i].type;
        
        if (!areTypesCompatible(paramType, argType)) {
            error("Type mismatch in argument " + to_string(i + 1) + 
                  " of function '" + node->functionName + "': expected " +
                  typeToString(paramType) + ", got " + typeToString(argType),
                  node->line, node->column);
        }
    }
}

// ==================== Public Methods ====================

void SemanticAnalyzer::analyze(ProgramNode* ast) {
    if (!ast) {
        cerr << "Error: AST is null" << endl;
        hadError = true;
        return;
    }
    
    analyzeProgram(ast);
}

bool SemanticAnalyzer::hasError() const {
    return hadError;
}

int SemanticAnalyzer::getErrorCount() const {
    return errorCount;
}

void SemanticAnalyzer::printSymbolTable() {
    cout << "\n========== SYMBOL TABLE ==========\n";
    
    if (scopeStack.empty() || scopeStack[0].empty()) {
        cout << "Symbol table is empty.\n";
    } else {
        cout << "Name\t\tType\t\tLine\tInitialized\n";
        cout << "----\t\t----\t\t----\t-----------\n";
        
        for (const auto& entry : scopeStack[0]) {
            const Symbol& sym = entry.second;
            cout << sym.name << "\t\t" 
                 << typeToString(sym.type) << "\t\t"
                 << sym.line << "\t"
                 << (sym.initialized ? "Yes" : "No") << "\n";
        }
    }
    
    cout << "==================================\n";
}

void SemanticAnalyzer::printFunctionTable() {
    cout << "\n========== FUNCTION TABLE ==========\n";
    
    if (functionTable.empty()) {
        cout << "Function table is empty.\n";
    } else {
        cout << "Name\t\tParameters\tLine\n";
        cout << "----\t\t----------\t----\n";
        
        for (const auto& entry : functionTable) {
            const FunctionInfo& func = entry.second;
            cout << func.name << "\t\t" << func.parameters.size() << "\t\t" << func.line << "\n";
        }
    }
    
    cout << "====================================\n";
}