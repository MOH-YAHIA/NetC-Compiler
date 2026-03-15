#pragma once

#include <string>
#include <map>
using namespace std;

// Enumeration of all token types in NetC language
enum TokenType {
    // Keywords
    FEED, FORWARD, ITERATE, UNTIL, NETWORK, INIT, IF, ELSE, YIELD, LINK,
    
    // Data types
    TEXT, DNUM, CNUM, FLAG,
    
    // Arithmetic Operators
    PLUS, MINUS, MULTIPLY, DIVIDE, MODULO,
    
    // Assignment Operators
    ASSIGN, PLUS_ASSIGN, MINUS_ASSIGN, MULT_ASSIGN, DIV_ASSIGN,
    
    // Comparison Operators
    EQ, NEQ, LT, GT, LTE, GTE,
    
    // Logical Operators
    AND, OR, NOT,
    
    // Bitwise Operators
    BITWISE_AND, BITWISE_OR, BITWISE_XOR, BITWISE_NOT, LEFT_SHIFT, RIGHT_SHIFT,
    
    // Increment/Decrement
    INCREMENT, DECREMENT,
    
    // Delimiters
    SEMICOLON, COMMA, LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET,
    
    // Literals
    INTEGER_LITERAL, FLOAT_LITERAL, STRING_LITERAL, BOOLEAN_LITERAL,
    
    // Identifier
    IDENTIFIER,
    
    // Special
    COMMENT, END_OF_FILE, UNKNOWN
};

// Token structure to store information about each token
struct Token {
    TokenType type;      // Type of the token
    string lexeme;       // The actual text from source code
    int line;           // Line number where token appears
    int column;         // Column number where token starts
    
    // Constructor
    Token(TokenType t, string lex, int l, int c);
};

// Global map for converting token types to readable strings
extern map<TokenType, string> tokenTypeNames;

// Utility function to convert TokenType to string
string tokenTypeToString(TokenType type);

// Function to initialize the token type names map
void initializeTokenTypeNames();
