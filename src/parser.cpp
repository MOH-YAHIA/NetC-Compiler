#include "../include/parser.h"
#include <iostream>

using namespace std;

// Constructor
Parser::Parser(vector<Token> tokens) : tokens(tokens), current(0), hadError(false), ast(nullptr) {}

// ==================== Utility Methods ====================

Token Parser::peek() {
    return tokens[current];
}

Token Parser::previous() {
    return tokens[current - 1];
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::isAtEnd() {
    return peek().type == END_OF_FILE;
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::match(vector<TokenType> types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

// ==================== Error Handling ====================

void Parser::error(string message) {
    Token token = peek();
    cerr << "Parse Error at line " << token.line << ", column " << token.column 
         << ": " << message << endl;
    cerr << "  Found: " << tokenTypeToString(token.type) 
         << " ('" << token.lexeme << "')" << endl;
    hadError = true;
}

void Parser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (previous().type == SEMICOLON) return;
        
        switch (peek().type) {
            case LINK: case TEXT: case DNUM: case CNUM: case FLAG:
            case IF: case UNTIL: case ITERATE: case NETWORK: case INIT:
            case FEED: case FORWARD: case YIELD:
                return;
            default:
                break;
        }
        advance();
    }
}

// ==================== Helper Methods ====================

bool Parser::isDataType(TokenType type) {
    return type == TEXT || type == DNUM || type == CNUM || type == FLAG;
}

bool Parser::isAddOp(TokenType type) {
    return type == PLUS || type == MINUS || type == BITWISE_OR || 
           type == BITWISE_AND || type == BITWISE_XOR;
}

bool Parser::isMulOp(TokenType type) {
    return type == MULTIPLY || type == DIVIDE || type == MODULO || 
           type == LEFT_SHIFT || type == RIGHT_SHIFT;
}

bool Parser::isRelOp(TokenType type) {
    return type == EQ || type == NEQ || type == LT || 
           type == GT || type == LTE || type == GTE;
}

bool Parser::isLogicalOp(TokenType type) {
    return type == AND || type == OR;
}

bool Parser::isUnaryOp(TokenType type) {
    return type == MINUS || type == NOT || type == BITWISE_NOT || 
           type == INCREMENT || type == DECREMENT;
}

// ==================== Grammar Rules (Building AST) ====================

// Program → StatementList EOF
unique_ptr<ProgramNode> Parser::program() {
    cout << "Parsing program and building AST..." << endl;
    
    auto prog = make_unique<ProgramNode>();
    statementList(prog->statements);
    
    if (!isAtEnd()) {
        error("Expected end of file");
    }
    
    if (!hadError) {
        cout << "\n=== Parsing completed successfully! ===" << endl;
        cout << "=== AST built successfully! ===" << endl;
    } else {
        cout << "\n=== Parsing completed with errors ===" << endl;
    }
    
    return prog;
}

// StatementList → Statement*
void Parser::statementList(vector<unique_ptr<ASTNode>>& statements) {
    while (!isAtEnd() && !check(RBRACE)) {
        auto stmt = statement();
        if (stmt) {
            statements.push_back(move(stmt));
        }
    }
}

// Statement → Declaration | Assignment | ...
unique_ptr<ASTNode> Parser::statement() {
    if (match(COMMENT)) {
        return nullptr;  // Skip comments
    }
    
    if (check(LINK)) {
        return linkStmt();
    }
    
    if (isDataType(peek().type)) {
        return declaration();
    }
    
    if (check(IF)) {
        return ifStmt();
    }
    
    if (check(UNTIL)) {
        return untilStmt();
    }
    
    if (check(ITERATE)) {
        return iterateStmt();
    }
    
    if (check(NETWORK)) {
        return networkStmt();
    }
    
    if (check(INIT)) {
        return initStmt();
    }
    
    if (check(FEED)) {
        return feedStmt();
    }
    
    if (check(FORWARD)) {
        return forwardStmt();
    }
    
    if (check(YIELD)) {
        return returnStmt();
    }
    
    if (check(IDENTIFIER)) {
        return assignment();
    }
    
    if (match(SEMICOLON)) {
        return nullptr;  // Empty statement
    }
    
    if (!isAtEnd() && !check(RBRACE)) {
        error("Unexpected token in statement");
        synchronize();
    }
    
    return nullptr;
}

// LinkStmt → link StringLiteral ;
unique_ptr<LinkStmtNode> Parser::linkStmt() {
    cout << "Parsing link statement..." << endl;
    
    Token linkToken = advance();  // Consume 'link'
    
    if (!check(STRING_LITERAL)) {
        error("Expected string literal after 'link'");
        return nullptr;
    }
    
    Token filename = advance();
    auto node = make_unique<LinkStmtNode>(filename.lexeme, filename.line, filename.column);
    
    if (!match(SEMICOLON)) {
        error("Expected ';' after link statement");
        synchronize();
    }
    
    return node;
}

// Declaration → DataType Identifier [= Expr] ;
unique_ptr<DeclarationNode> Parser::declaration() {
    cout << "Parsing declaration..." << endl;
    
    Token typeToken = advance();  // Consume data type
    
    if (!check(IDENTIFIER)) {
        error("Expected identifier in declaration");
        return nullptr;
    }
    
    Token idToken = advance();
    auto node = make_unique<DeclarationNode>(typeToken.type, idToken.lexeme, 
                                              idToken.line, idToken.column);
    
    if (match(ASSIGN)) {
        node->initValue = expr();
    }
    
    if (!match(SEMICOLON)) {
        error("Expected ';' after declaration");
        synchronize();
    }
    
    return node;
}

// Assignment → Identifier = Expr ;
unique_ptr<AssignmentNode> Parser::assignment() {
    cout << "Parsing assignment..." << endl;
    
    Token idToken = advance();  // Consume identifier
    
    if (!match(ASSIGN)) {
        error("Expected '=' in assignment");
        return nullptr;
    }
    
    auto value = expr();
    auto node = make_unique<AssignmentNode>(idToken.lexeme, move(value), 
                                             idToken.line, idToken.column);
    
    if (!match(SEMICOLON)) {
        error("Expected ';' after assignment");
        synchronize();
    }
    
    return node;
}

// IfStmt → if ( Condition ) { StatementList } [else { StatementList }]
unique_ptr<IfStmtNode> Parser::ifStmt() {
    cout << "Parsing if statement..." << endl;
    
    Token ifToken = advance();  // Consume 'if'
    
    if (!match(LPAREN)) {
        error("Expected '(' after 'if'");
        return nullptr;
    }
    
    auto cond = condition();
    
    if (!match(RPAREN)) {
        error("Expected ')' after condition");
        return nullptr;
    }
    
    if (!match(LBRACE)) {
        error("Expected '{' after if condition");
        return nullptr;
    }
    
    auto node = make_unique<IfStmtNode>(move(cond), ifToken.line, ifToken.column);
    statementList(node->thenBody);
    
    if (!match(RBRACE)) {
        error("Expected '}' after if body");
        return node;
    }
    
    if (match(ELSE)) {
        if (!match(LBRACE)) {
            error("Expected '{' after 'else'");
            return node;
        }
        
        statementList(node->elseBody);
        
        if (!match(RBRACE)) {
            error("Expected '}' after else body");
        }
    }
    
    return node;
}

// UntilStmt → until ( Condition ) { StatementList }
unique_ptr<UntilStmtNode> Parser::untilStmt() {
    cout << "Parsing until loop..." << endl;
    
    Token untilToken = advance();  // Consume 'until'
    
    if (!match(LPAREN)) {
        error("Expected '(' after 'until'");
        return nullptr;
    }
    
    auto cond = condition();
    
    if (!match(RPAREN)) {
        error("Expected ')' after condition");
        return nullptr;
    }
    
    if (!match(LBRACE)) {
        error("Expected '{' after until condition");
        return nullptr;
    }
    
    auto node = make_unique<UntilStmtNode>(move(cond), untilToken.line, untilToken.column);
    statementList(node->body);
    
    if (!match(RBRACE)) {
        error("Expected '}' after until body");
    }
    
    return node;
}

// IterateStmt → iterate ( Init ; Condition ; Update ) { StatementList }
unique_ptr<IterateStmtNode> Parser::iterateStmt() {
    cout << "Parsing iterate loop..." << endl;
    
    Token iterToken = advance();  // Consume 'iterate'
    
    if (!match(LPAREN)) {
        error("Expected '(' after 'iterate'");
        return nullptr;
    }
    
    auto node = make_unique<IterateStmtNode>(iterToken.line, iterToken.column);
    
    // Init (declaration)
    if (isDataType(peek().type)) {
        Token typeToken = advance();
        if (check(IDENTIFIER)) {
            Token idToken = advance();
            auto decl = make_unique<DeclarationNode>(typeToken.type, idToken.lexeme, 
                                                      idToken.line, idToken.column);
            if (match(ASSIGN)) {
                decl->initValue = expr();
            }
            node->init = move(decl);
        }
    }
    
    if (!match(SEMICOLON)) {
        error("Expected ';' after iterate initialization");
        return nullptr;
    }
    
    // Condition
    node->condition = condition();
    
    if (!match(SEMICOLON)) {
        error("Expected ';' after iterate condition");
        return nullptr;
    }
    
    // Update (assignment)
    if (check(IDENTIFIER)) {
        Token idToken = advance();
        if (match(ASSIGN)) {
            auto value = expr();
            node->update = make_unique<AssignmentNode>(idToken.lexeme, move(value), 
                                                        idToken.line, idToken.column);
        }
    }
    
    if (!match(RPAREN)) {
        error("Expected ')' after iterate clauses");
        return nullptr;
    }
    
    if (!match(LBRACE)) {
        error("Expected '{' after iterate header");
        return nullptr;
    }
    
    statementList(node->body);
    
    if (!match(RBRACE)) {
        error("Expected '}' after iterate body");
    }
    
    return node;
}

// NetworkStmt → network Identifier ( ParameterList ) { StatementList }
unique_ptr<NetworkStmtNode> Parser::networkStmt() {
    cout << "Parsing network function..." << endl;
    
    Token netToken = advance();  // Consume 'network'
    
    if (!check(IDENTIFIER)) {
        error("Expected function name after 'network'");
        return nullptr;
    }
    
    Token nameToken = advance();
    auto node = make_unique<NetworkStmtNode>(nameToken.lexeme, nameToken.line, nameToken.column);
    
    if (!match(LPAREN)) {
        error("Expected '(' after function name");
        return nullptr;
    }
    
    if (!check(RPAREN)) {
        parameterList(node->parameters);
    }
    
    if (!match(RPAREN)) {
        error("Expected ')' after parameters");
        return nullptr;
    }
    
    if (!match(LBRACE)) {
        error("Expected '{' after function header");
        return nullptr;
    }
    
    statementList(node->body);
    
    if (!match(RBRACE)) {
        error("Expected '}' after function body");
    }
    
    return node;
}

// InitStmt → init ( ) { StatementList }
unique_ptr<InitStmtNode> Parser::initStmt() {
    cout << "Parsing init function..." << endl;
    
    Token initToken = advance();  // Consume 'init'
    
    if (!match(LPAREN)) {
        error("Expected '(' after 'init'");
        return nullptr;
    }
    
    if (!match(RPAREN)) {
        error("Expected ')' after 'init'");
        return nullptr;
    }
    
    if (!match(LBRACE)) {
        error("Expected '{' after init header");
        return nullptr;
    }
    
    auto node = make_unique<InitStmtNode>(initToken.line, initToken.column);
    statementList(node->body);
    
    if (!match(RBRACE)) {
        error("Expected '}' after init body");
    }
    
    return node;
}

// ParameterList → DataType Identifier [, DataType Identifier]*
void Parser::parameterList(vector<Parameter>& params) {
    do {
        if (!isDataType(peek().type)) {
            error("Expected data type in parameter list");
            return;
        }
        
        Token typeToken = advance();
        
        if (!check(IDENTIFIER)) {
            error("Expected parameter name");
            return;
        }
        
        Token nameToken = advance();
        params.push_back({typeToken.type, nameToken.lexeme});
        
    } while (match(COMMA));
}

// ReturnStmt → yield Expr ;
unique_ptr<ReturnStmtNode> Parser::returnStmt() {
    cout << "Parsing return statement..." << endl;
    
    Token yieldToken = advance();  // Consume 'yield'
    auto value = expr();
    auto node = make_unique<ReturnStmtNode>(move(value), yieldToken.line, yieldToken.column);
    
    if (!match(SEMICOLON)) {
        error("Expected ';' after return statement");
        synchronize();
    }
    
    return node;
}

// FeedStmt → feed Identifier ;
unique_ptr<FeedStmtNode> Parser::feedStmt() {
    cout << "Parsing feed statement..." << endl;
    
    Token feedToken = advance();  // Consume 'feed'
    
    if (!check(IDENTIFIER)) {
        error("Expected identifier after 'feed'");
        return nullptr;
    }
    
    Token idToken = advance();
    auto node = make_unique<FeedStmtNode>(idToken.lexeme, feedToken.line, feedToken.column);
    
    if (!match(SEMICOLON)) {
        error("Expected ';' after feed statement");
        synchronize();
    }
    
    return node;
}

// ForwardStmt → forward ( Expr ) ;
unique_ptr<ForwardStmtNode> Parser::forwardStmt() {
    cout << "Parsing forward statement..." << endl;
    
    Token fwdToken = advance();  // Consume 'forward'
    
    if (!match(LPAREN)) {
        error("Expected '(' after 'forward'");
        return nullptr;
    }
    
    auto expr_node = expr();
    
    if (!match(RPAREN)) {
        error("Expected ')' after expression");
        return nullptr;
    }
    
    auto node = make_unique<ForwardStmtNode>(move(expr_node), fwdToken.line, fwdToken.column);
    
    if (!match(SEMICOLON)) {
        error("Expected ';' after forward statement");
        synchronize();
    }
    
    return node;
}

// Condition → Expr [RelOp|LogicalOp Expr] | ! Condition | ( Condition )
unique_ptr<ASTNode> Parser::condition() {
    if (match(NOT)) {
        Token opToken = previous();
        auto operand = condition();
        return make_unique<UnaryExprNode>(opToken.type, move(operand), opToken.line, opToken.column);
    }
    
    if (match(LPAREN)) {
        auto cond = condition();
        if (!match(RPAREN)) {
            error("Expected ')' after condition");
        }
        return cond;
    }
    
    auto left = expr();
    
    if (isRelOp(peek().type) || isLogicalOp(peek().type)) {
        Token opToken = advance();
        auto right = expr();
        return make_unique<BinaryExprNode>(move(left), opToken.type, move(right), 
                                            opToken.line, opToken.column);
    }
    
    return left;
}

// Expr → Term [AddOp Term]*
unique_ptr<ASTNode> Parser::expr() {
    auto left = term();
    
    while (isAddOp(peek().type)) {
        Token opToken = advance();
        auto right = term();
        left = make_unique<BinaryExprNode>(move(left), opToken.type, move(right), 
                                            opToken.line, opToken.column);
    }
    
    return left;
}

// Term → Factor [MulOp Factor]*
unique_ptr<ASTNode> Parser::term() {
    auto left = factor();
    
    while (isMulOp(peek().type)) {
        Token opToken = advance();
        auto right = factor();
        left = make_unique<BinaryExprNode>(move(left), opToken.type, move(right), 
                                            opToken.line, opToken.column);
    }
    
    return left;
}

// Factor → Literal | Identifier | ( Expr ) | UnaryOp Factor | FunctionCall
unique_ptr<ASTNode> Parser::factor() {
    // Unary operators
    if (isUnaryOp(peek().type)) {
        Token opToken = advance();
        auto operand = factor();
        return make_unique<UnaryExprNode>(opToken.type, move(operand), 
                                           opToken.line, opToken.column);
    }
    
    // Literals
    if (check(INTEGER_LITERAL) || check(FLOAT_LITERAL) || 
        check(STRING_LITERAL) || check(BOOLEAN_LITERAL)) {
        Token litToken = advance();
        return make_unique<LiteralNode>(litToken.type, litToken.lexeme, 
                                         litToken.line, litToken.column);
    }
    
    // Identifier or function call
    if (check(IDENTIFIER)) {
        Token idToken = advance();
        
        // Function call
        if (match(LPAREN)) {
            auto funcCall = make_unique<FunctionCallNode>(idToken.lexeme, 
                                                           idToken.line, idToken.column);
            if (!check(RPAREN)) {
                argumentList(funcCall->arguments);
            }
            if (!match(RPAREN)) {
                error("Expected ')' after arguments");
            }
            return funcCall;
        }
        
        // Just identifier
        return make_unique<IdentifierNode>(idToken.lexeme, idToken.line, idToken.column);
    }
    
    // Parenthesized expression
    if (match(LPAREN)) {
        auto expr_node = expr();
        if (!match(RPAREN)) {
            error("Expected ')' after expression");
        }
        return expr_node;
    }
    
    error("Expected expression");
    return nullptr;
}

// ArgumentList → Expr [, Expr]*
void Parser::argumentList(vector<unique_ptr<ASTNode>>& args) {
    do {
        args.push_back(expr());
    } while (match(COMMA));
}

// ==================== Public Methods ====================

unique_ptr<ProgramNode> Parser::parse() {
    ast = program();
    return move(ast);
}

bool Parser::hasError() {
    return hadError;
}

void Parser::printAST() {
    if (ast) {
        cout << "\n========== ABSTRACT SYNTAX TREE ==========\n";
        ast->print(0);
        cout << "==========================================\n";
    }
}