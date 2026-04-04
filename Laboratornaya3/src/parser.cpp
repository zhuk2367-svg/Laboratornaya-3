#include "../include/parser.h"
#include <vector>
#include <algorithm>

using namespace std;

void Parser::next() { tok = tokenizer.next(); }

bool Parser::match(char op) {
    if (tok.type == lexem_t::OP && tok.value[0] == op) { 
        next(); 
        return true; 
    }
    return false;
}

bool Parser::isReservedFunction(const string& name) {
    static const vector<string> funcs = {"sin", "cos", "tan", "asin", "acos", "atan", "exp", "log", "sqrt"};
    return find(funcs.begin(), funcs.end(), name) != funcs.end();
}

Parser::Parser(const string& expr) : tokenizer(expr) { next(); }

unique_ptr<Node> Parser::parse() {
    auto astNode = parseExpression();
    if (tok.type != lexem_t::EOEX) throw runtime_error("ERROR: unexpected token at end");
    return astNode;
}

unique_ptr<Node> Parser::parseExpression() {
    auto astNode = parseTerm();
    while (tok.type == lexem_t::OP && (tok.value == "+" || tok.value == "-")) {
        char op = tok.value[0]; 
        next();
        astNode = make_unique<BinaryNode>(op, move(astNode), parseTerm());
    }
    return astNode;
}

unique_ptr<Node> Parser::parseTerm() {
    auto astNode = parseUnary();
    while (tok.type == lexem_t::OP && (tok.value == "*" || tok.value == "/")) {
        char op = tok.value[0]; 
        next();
        astNode = make_unique<BinaryNode>(op, move(astNode), parseUnary());
    }
    return astNode;
}

unique_ptr<Node> Parser::parseUnary() {
    if (tok.type == lexem_t::OP && (tok.value == "+" || tok.value == "-")) {
        char op = tok.value[0]; 
        next();
        return make_unique<UnaryNode>(op, parseUnary());
    }
    return parseFactor();
}

unique_ptr<Node> Parser::parseFactor() {
    auto astNode = parsePrimary();
    if (tok.type == lexem_t::OP && tok.value == "^") {
        next();
        astNode = make_unique<BinaryNode>('^', move(astNode), parseUnary()); 
    }
    return astNode;
}

unique_ptr<Node> Parser::parsePrimary() {
    if (tok.type == lexem_t::NUM) {
        auto astNode = make_unique<NumNode>(tok.numValue); 
        next(); 
        return astNode;
    }
    if (tok.type == lexem_t::ID) {
        string ident = tok.value; 
        next();
        
        bool isFunction = isReservedFunction(ident);
        
        if (isFunction && (tok.type != lexem_t::OP || tok.value != "(")) {
            throw runtime_error("ERROR: Function " + ident + " must be followed by '('");
        }

        if (tok.type == lexem_t::OP && tok.value == "(") {
            next(); 
            
            if (tok.type == lexem_t::OP && tok.value == ")") {
                 throw runtime_error("ERROR: Empty parentheses");
            }
            
            auto argument = parseExpression();
            if (!match(')')) throw runtime_error("ERROR: Expected )");
            
            if (isFunction) {
                return make_unique<FuncNode>(ident, move(argument));
            } else {
                throw runtime_error("ERROR Unknown function: " + ident);
            }
        }
        return make_unique<VarNode>(ident);
    }
    if (tok.type == lexem_t::OP && tok.value == "(") {
        next();
        if (tok.type == lexem_t::OP && tok.value == ")") {
             throw runtime_error("ERROR: Empty parentheses");
        }
        auto astNode = parseExpression();
        if (!match(')')) throw runtime_error("ERROR: Expected )");
        return astNode;
    }
    throw runtime_error("ERROR: Syntax error in primary");
}