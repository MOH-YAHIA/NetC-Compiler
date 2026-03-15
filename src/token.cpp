#include "token.h"

// Token constructor implementation
Token::Token(TokenType t, string lex, int l, int c) 
    : type(t), lexeme(lex), line(l), column(c) {}

// Global map definition - maps token types to their string names
map<TokenType, string> tokenTypeNames;

// Initialize the token type names map
void initializeTokenTypeNames() {
    tokenTypeNames[FEED] = "FEED";
    tokenTypeNames[FORWARD] = "FORWARD";
    tokenTypeNames[ITERATE] = "ITERATE";
    tokenTypeNames[UNTIL] = "UNTIL";
    tokenTypeNames[NETWORK] = "NETWORK";
    tokenTypeNames[INIT] = "INIT";
    tokenTypeNames[IF] = "IF";
    tokenTypeNames[ELSE] = "ELSE";
    tokenTypeNames[YIELD] = "YIELD";
    tokenTypeNames[LINK] = "LINK";
    
    tokenTypeNames[TEXT] = "TEXT";
    tokenTypeNames[DNUM] = "DNUM";
    tokenTypeNames[CNUM] = "CNUM";
    tokenTypeNames[FLAG] = "FLAG";
    
    tokenTypeNames[PLUS] = "PLUS";
    tokenTypeNames[MINUS] = "MINUS";
    tokenTypeNames[MULTIPLY] = "MULTIPLY";
    tokenTypeNames[DIVIDE] = "DIVIDE";
    tokenTypeNames[MODULO] = "MODULO";
    
    tokenTypeNames[ASSIGN] = "ASSIGN";
    tokenTypeNames[PLUS_ASSIGN] = "PLUS_ASSIGN";
    tokenTypeNames[MINUS_ASSIGN] = "MINUS_ASSIGN";
    tokenTypeNames[MULT_ASSIGN] = "MULT_ASSIGN";
    tokenTypeNames[DIV_ASSIGN] = "DIV_ASSIGN";
    
    tokenTypeNames[EQ] = "EQ";
    tokenTypeNames[NEQ] = "NEQ";
    tokenTypeNames[LT] = "LT";
    tokenTypeNames[GT] = "GT";
    tokenTypeNames[LTE] = "LTE";
    tokenTypeNames[GTE] = "GTE";
    
    tokenTypeNames[AND] = "AND";
    tokenTypeNames[OR] = "OR";
    tokenTypeNames[NOT] = "NOT";
    
    tokenTypeNames[BITWISE_AND] = "BITWISE_AND";
    tokenTypeNames[BITWISE_OR] = "BITWISE_OR";
    tokenTypeNames[BITWISE_XOR] = "BITWISE_XOR";
    tokenTypeNames[BITWISE_NOT] = "BITWISE_NOT";
    tokenTypeNames[LEFT_SHIFT] = "LEFT_SHIFT";
    tokenTypeNames[RIGHT_SHIFT] = "RIGHT_SHIFT";
    
    tokenTypeNames[INCREMENT] = "INCREMENT";
    tokenTypeNames[DECREMENT] = "DECREMENT";
    
    tokenTypeNames[SEMICOLON] = "SEMICOLON";
    tokenTypeNames[COMMA] = "COMMA";
    tokenTypeNames[LPAREN] = "LPAREN";
    tokenTypeNames[RPAREN] = "RPAREN";
    tokenTypeNames[LBRACE] = "LBRACE";
    tokenTypeNames[RBRACE] = "RBRACE";
    tokenTypeNames[LBRACKET] = "LBRACKET";
    tokenTypeNames[RBRACKET] = "RBRACKET";
    
    tokenTypeNames[INTEGER_LITERAL] = "INTEGER_LITERAL";
    tokenTypeNames[FLOAT_LITERAL] = "FLOAT_LITERAL";
    tokenTypeNames[STRING_LITERAL] = "STRING_LITERAL";
    tokenTypeNames[BOOLEAN_LITERAL] = "BOOLEAN_LITERAL";
    
    tokenTypeNames[IDENTIFIER] = "IDENTIFIER";
    tokenTypeNames[COMMENT] = "COMMENT";
    tokenTypeNames[END_OF_FILE] = "END_OF_FILE";
    tokenTypeNames[UNKNOWN] = "UNKNOWN";
}

// Utility function to convert TokenType to string
string tokenTypeToString(TokenType type) {
    if (tokenTypeNames.find(type) != tokenTypeNames.end()) {
        return tokenTypeNames[type];
    }
    return "UNKNOWN";
}