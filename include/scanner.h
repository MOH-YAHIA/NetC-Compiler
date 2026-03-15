#pragma once

#include <string>
#include <vector>
#include <map>
#include "token.h"

using namespace std;

// Scanner class - performs lexical analysis on NetC source code
class Scanner {
private:
    string source;              // The source code to scan
    vector<Token> tokens;       // List of tokens found
    int start;                  // Start position of current lexeme
    int current;                // Current position in source
    int line;                   // Current line number
    int column;                 // Current column number
    
    // Map of keywords to their token types
    map<string, TokenType> keywords;
    
    // Helper methods for scanning
    bool isAtEnd();                    // Check if reached end of source
    char advance();                     // Get next character and advance
    char peek();                        // Look at current character without advancing
    char peekNext();                    // Look at next character without advancing
    bool match(char expected);          // Check if current char matches expected
    void addToken(TokenType type);      // Add a token to the list
    
    // Scanning methods for different token types
    void scanToken();                   // Scan a single token
    void scanString();                  // Scan string literal
    void scanNumber();                  // Scan numeric literal
    void scanIdentifier();              // Scan identifier or keyword
    
public:
    // Constructor
    Scanner(const string& source);
    
    // Main scanning method
    vector<Token> scanTokens();
    
    // Utility methods
    void printTokens();                 // Print all tokens in formatted table
    vector<Token> getTokens() const;    // Get the token list
};
