#include "../include/scanner.h"
#include <iostream>
#include <cctype>

using namespace std;

// Constructor - initializes scanner with source code
Scanner::Scanner(const string& src) : source(src), start(0), current(0), line(1), column(1) {
    // Initialize keywords map
    keywords["feed"] = FEED;
    keywords["forward"] = FORWARD;
    keywords["iterate"] = ITERATE;
    keywords["until"] = UNTIL;
    keywords["network"] = NETWORK;
    keywords["init"] = INIT;
    keywords["if"] = IF;
    keywords["else"] = ELSE;
    keywords["yield"] = YIELD;
    keywords["link"] = LINK;
    keywords["text"] = TEXT;
    keywords["dnum"] = DNUM;
    keywords["cnum"] = CNUM;
    keywords["flag"] = FLAG;
    keywords["true"] = BOOLEAN_LITERAL;
    keywords["false"] = BOOLEAN_LITERAL;
}

// Check if we've reached the end of source code
bool Scanner::isAtEnd() {
    return current >= source.length();
}

// Get current character and move forward
char Scanner::advance() {
    column++;
    return source[current++];
}

// Look at current character without consuming it
char Scanner::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

// Look at next character without consuming it
char Scanner::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

// Check if current character matches expected, consume if true
bool Scanner::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;
    current++;
    column++;
    return true;
}

// Add a token to the token list
void Scanner::addToken(TokenType type) {
    string text = source.substr(start, current - start);
    tokens.push_back(Token(type, text, line, column - text.length()));
}

// Main token scanning method
void Scanner::scanToken() {
    char c = advance();
    
    switch (c) {
        // Single character delimiters
        case '(': addToken(LPAREN); break;
        case ')': addToken(RPAREN); break;
        case '{': addToken(LBRACE); break;
        case '}': addToken(RBRACE); break;
        case '[': addToken(LBRACKET); break;
        case ']': addToken(RBRACKET); break;
        case ';': addToken(SEMICOLON); break;
        case ',': addToken(COMMA); break;
        case '~': addToken(BITWISE_NOT); break;
        case '^': addToken(BITWISE_XOR); break;
        case '%': addToken(MODULO); break;
        
        // Operators that can be combined
        case '+':
            if (match('+')) addToken(INCREMENT); //++
            else if (match('=')) addToken(PLUS_ASSIGN); //+=
            else addToken(PLUS);
            break;
        case '-':
            if (match('-')) addToken(DECREMENT);
            else if (match('=')) addToken(MINUS_ASSIGN);
            else addToken(MINUS);
            break;
        case '*':
            if (match('=')) addToken(MULT_ASSIGN);
            else addToken(MULTIPLY);
            break;
        case '/':
            if (match('=')) addToken(DIV_ASSIGN);
            else addToken(DIVIDE);
            break;
        case '!':
            if (match('=')) addToken(NEQ);
            else addToken(NOT);
            break;
        case '=':
            if (match('=')) addToken(EQ);
            else addToken(ASSIGN);
            break;
        case '<':
            if (match('<')) addToken(LEFT_SHIFT);
            else if (match('=')) addToken(LTE);
            else addToken(LT);
            break;
        case '>':
            if (match('>')) addToken(RIGHT_SHIFT);
            else if (match('=')) addToken(GTE);
            else addToken(GT);
            break;
        case '&':
            if (match('&')) addToken(AND);
            else addToken(BITWISE_AND);
            break;
        case '|':
            if (match('|')) addToken(OR);
            else addToken(BITWISE_OR);
            break;
        
        // Comments - scan to end of line
        case '#':
            while (peek() != '\n' && !isAtEnd()) advance();
            addToken(COMMENT);
            break;
        
        // String literals
        case '"':
            scanString();
            break;
        
        // Whitespace - ignore but track newlines for line numbers
        case ' ':
        case '\r':
        case '\t':
            break;
        case '\n':
            line++;
            column = 1;
            break;
        
        // Numbers, identifiers, or unknown
        default:
            if (isdigit(c)) {
                scanNumber();
            } else if (isalpha(c) || c == '_') {
                scanIdentifier();
            } else {
                cout << "Error: Unknown character '" << c << "' at line " << line 
                     << ", column " << column << endl;
                addToken(UNKNOWN);
            }
            break;
    }
}

// Scan a string literal (between double quotes)
void Scanner::scanString() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
            column = 1;
        }
        advance();
    }
    
    // Unterminated string error
    if (isAtEnd()) {
        cout << "Error: Unterminated string at line " << line << endl;
        return;
    }
    
    advance(); // Consume closing "
    addToken(STRING_LITERAL);
}

// Scan a numeric literal (integer or floating point)
void Scanner::scanNumber() {
    // Consume all digits
    while (isdigit(peek())) advance();
    
    // Check for decimal point followed by digits (floating point)
    if (peek() == '.' && isdigit(peekNext())) {
        advance(); // Consume '.'
        while (isdigit(peek())) advance();
        addToken(FLOAT_LITERAL);
    } else {
        addToken(INTEGER_LITERAL);
    }
}

// Scan an identifier or keyword
void Scanner::scanIdentifier() {
    // Consume alphanumeric characters and underscores
    while (isalnum(peek()) || peek() == '_') advance();
    
    // Check if it's a keyword or just an identifier
    string text = source.substr(start, current - start);
    TokenType type = keywords.count(text) ? keywords[text] : IDENTIFIER;
    addToken(type);
}

// Main method - scan all tokens from source
vector<Token> Scanner::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }
    
    // Add end-of-file token
    tokens.push_back(Token(END_OF_FILE, "", line, column));
    return tokens;
}

// Print all tokens in a formatted table
void Scanner::printTokens() {
    cout << "\n====================== TOKEN LIST ======================\n";
    cout << "Line\tCol\tType\t\t\tLexeme\n";
    cout << "----\t---\t----\t\t\t------\n";
    
    for (const auto& token : tokens) {
        // Skip comments in output
        if (token.type == COMMENT) continue;
        
        cout << token.line << "\t" << token.column << "\t" 
             << tokenTypeToString(token.type);
        
        // Add extra tab for alignment if type name is short
        if (tokenTypeToString(token.type).length() < 16) cout << "\t";
        if (tokenTypeToString(token.type).length() < 8) cout << "\t";
        
        cout << token.lexeme << "\n";
    }
    cout << "========================================================\n";
}

// Getter for tokens
vector<Token> Scanner::getTokens() const {
    return tokens;
}