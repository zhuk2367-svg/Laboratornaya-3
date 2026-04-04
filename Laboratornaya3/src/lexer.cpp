#include "../include/lexer.h"

char Lexer::peek() const { 
    return idx < source.length() ? source[idx] : '\0'; 
}

char Lexer::get() { 
    return idx < source.length() ? source[idx++] : '\0'; 
}

Lexer::Lexer(const string& expression) : source(expression), idx(0) {}

Token Lexer::next() {
    while (isspace(peek())) get();
    char ch = peek();
    if (ch == '\0') return {lexem_t::EOEX, "", 0.0};

    if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^' || ch == '(' || ch == ')') {
        return {lexem_t::OP, string(1, get()), 0.0};
    }

    if (isdigit(ch)) {
        string numStr;
        bool has_dot = false;
        while (isdigit(peek()) || peek() == '.') {
            if (peek() == '.') {
                if (has_dot) throw runtime_error("ERROR: multiple dots in a number");
                has_dot = true;
            }
            numStr += get();
        }
        if (numStr.length() > 1 && numStr[0] == '0' && numStr[1] != '.') 
            throw runtime_error("ERROR: leading zeros are not allowed");
        if (!numStr.empty() && numStr.back() == '.') 
            throw runtime_error("ERROR: number cannot end with a dot");
        if (isalpha(peek()) || peek() == '_') 
            throw runtime_error("ERROR: number cannot be directly followed by a letter");
        
        return {lexem_t::NUM, numStr, stod(numStr)};
    }

    if (isalpha(ch) || ch == '_') {
        string ident;
        while (isalnum(peek()) || peek() == '_') ident += tolower(get());
        return {lexem_t::ID, ident, 0.0};
    }

    throw runtime_error(string("ERROR: unknown character '") + ch + "'");
}