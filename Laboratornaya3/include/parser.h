#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast_nodes.h"

using namespace std;

class Parser {
    Lexer tokenizer;
    Token tok;

    void next();
    bool match(char op);

public:
    static bool isReservedFunction(const string& name);
    Parser(const string& expr);
    unique_ptr<Node> parse();

private:
    unique_ptr<Node> parseExpression();
    unique_ptr<Node> parseTerm();
    unique_ptr<Node> parseUnary();
    unique_ptr<Node> parseFactor();
    unique_ptr<Node> parsePrimary();
};

#endif