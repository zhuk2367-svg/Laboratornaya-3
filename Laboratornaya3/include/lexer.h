#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <stdexcept>
#include <cctype>

using namespace std;

enum class lexem_t { OP, NUM, ID, EOEX };

struct Token {
    lexem_t type;
    string value;
    double numValue = 0.0;
};

class Lexer {
    string source;
    size_t idx;

    char peek() const;
    char get();

public:
    Lexer(const string& expression);
    Token next();
};

#endif