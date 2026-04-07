#include "../include/ast_nodes.h"
#include "../include/utils.h"
#include <stdexcept>
#include <cmath>
#include <string>

using namespace std;

// NumNode
NumNode::NumNode(double v) : numVal(v) {}
double NumNode::eval(const map<string, double>&) const { return numVal; }
unique_ptr<Node> NumNode::clone() const { return make_unique<NumNode>(numVal); }
unique_ptr<Node> NumNode::derive(const string&) const { return make_unique<NumNode>(0); }
string NumNode::toString() const { return formatDouble(numVal); }
bool NumNode::isConstant() const { return true; }

// VarNode
VarNode::VarNode(const string& n) : varName(n) {}
double VarNode::eval(const map<string, double>& vars) const {
    auto it = vars.find(varName);
    if (it == vars.end()) throw runtime_error("ERROR Unknown variable: " + varName);
    return it->second;
}
unique_ptr<Node> VarNode::clone() const { return make_unique<VarNode>(varName); }
unique_ptr<Node> VarNode::derive(const string& var) const { 
    return make_unique<NumNode>(varName == var ? 1 : 0); 
}
string VarNode::toString() const { return varName; }
bool VarNode::isConstant() const { return false; }

// UnaryNode
UnaryNode::UnaryNode(char o, unique_ptr<Node> c) : op(o), subNode(move(c)) {}
double UnaryNode::eval(const map<string, double>& vars) const {
    double v = subNode->eval(vars);
    return op == '-' ? -v : v;
}
unique_ptr<Node> UnaryNode::clone() const { 
    return make_unique<UnaryNode>(op, subNode->clone()); 
}
unique_ptr<Node> UnaryNode::derive(const string& var) const {
    return make_unique<UnaryNode>(op, subNode->derive(var));
}
string UnaryNode::toString() const { 
    return "(" + string(1, op) + subNode->toString() + ")"; 
}
bool UnaryNode::isConstant() const { return subNode->isConstant(); }

// BinaryNode
BinaryNode::BinaryNode(char o, unique_ptr<Node> l, unique_ptr<Node> r) 
    : op(o), leftNode(move(l)), rightNode(move(r)) {}

double BinaryNode::eval(const map<string, double>& vars) const {
    double l = leftNode->eval(vars);
    double r = rightNode->eval(vars);
    
    if (op == '+') return l + r;
    if (op == '-') return l - r;
    if (op == '*') return l * r;
    if (op == '/') {
        if (r == 0.0) throw runtime_error("ERROR Domain error: anything/0.0 is undefined");
        return l / r;
    }
    if (op == '^') {
        if (l < 0 && floor(r) != r) 
            throw runtime_error("ERROR Domain error: fractional power of negative number");
        return pow(l, r);
    }
    return 0;
}

unique_ptr<Node> BinaryNode::clone() const { 
    return make_unique<BinaryNode>(op, leftNode->clone(), rightNode->clone()); 
}

string BinaryNode::toString() const { 
    return "(" + leftNode->toString() + " " + op + " " + rightNode->toString() + ")"; 
}

bool BinaryNode::isConstant() const { 
    return leftNode->isConstant() && rightNode->isConstant(); 
}

unique_ptr<Node> BinaryNode::derive(const string& var) const {
    auto derivLeft = leftNode->derive(var);
    auto derivRight = rightNode->derive(var);

    if (op == '+') return make_unique<BinaryNode>('+', move(derivLeft), move(derivRight));
    if (op == '-') return make_unique<BinaryNode>('-', move(derivLeft), move(derivRight));
    if (op == '*') {
        return make_unique<BinaryNode>('+',
            make_unique<BinaryNode>('*', move(derivLeft), rightNode->clone()),
            make_unique<BinaryNode>('*', leftNode->clone(), move(derivRight))
        );
    }
    if (op == '/') {
        auto num = make_unique<BinaryNode>('-',
            make_unique<BinaryNode>('*', move(derivLeft), rightNode->clone()),
            make_unique<BinaryNode>('*', leftNode->clone(), move(derivRight))
        );
        auto den = make_unique<BinaryNode>('^', rightNode->clone(), make_unique<NumNode>(2));
        return make_unique<BinaryNode>('/', move(num), move(den));
    }
    if (op == '^') {
        if (rightNode->isConstant()) {
            double c = rightNode->eval({});
            auto c_node = make_unique<NumNode>(c);
            auto c_minus_1 = make_unique<NumNode>(c - 1.0);
            auto u_pow = make_unique<BinaryNode>('^', leftNode->clone(), move(c_minus_1));
            auto mul1 = make_unique<BinaryNode>('*', move(c_node), move(u_pow));
            return make_unique<BinaryNode>('*', move(mul1), move(derivLeft));
        } else if (leftNode->isConstant()) {
            auto c_pow_v = make_unique<BinaryNode>('^', leftNode->clone(), rightNode->clone());
            auto ln_c = make_unique<FuncNode>("log", leftNode->clone());
            auto mul1 = make_unique<BinaryNode>('*', move(c_pow_v), move(ln_c));
            return make_unique<BinaryNode>('*', move(mul1), move(derivRight));
        } else {
            auto f_pow_g = make_unique<BinaryNode>('^', leftNode->clone(), rightNode->clone());
            auto ln_f = make_unique<FuncNode>("log", leftNode->clone());
            auto term1 = make_unique<BinaryNode>('*', move(derivRight), move(ln_f));
            auto f_prime_div_f = make_unique<BinaryNode>('/', move(derivLeft), leftNode->clone());
            auto term2 = make_unique<BinaryNode>('*', rightNode->clone(), move(f_prime_div_f));
            auto sum = make_unique<BinaryNode>('+', move(term1), move(term2));
            return make_unique<BinaryNode>('*', move(f_pow_g), move(sum));
        }
    }
    return make_unique<NumNode>(0);
}

// FuncNode
FuncNode::FuncNode(const string& n, unique_ptr<Node> a) : name(n), funcArg(move(a)) {}

double FuncNode::eval(const map<string, double>& vars) const {
    double v = funcArg->eval(vars);
    
    if (name == "sin") return sin(v);
    if (name == "cos") return cos(v);
    if (name == "tan") return tan(v);
    if (name == "exp") return exp(v);
    if (name == "log") {
        if (v <= 0) throw runtime_error("ERROR Domain error: log(x) requires x >= 0");
        return log(v);
    }
    if (name == "sqrt") {
        if (v < 0) throw runtime_error("ERROR Domain error: sqrt(-1)");
        return sqrt(v);
    }
    if (name == "asin") {
        if (v < -1 || v > 1) throw runtime_error("ERROR Domain error: asin(x) requires |x| <= 1");
        return asin(v);
    }
    if (name == "acos") {
        if (v < -1 || v > 1) throw runtime_error("ERROR Domain error: acos(x) requires |x| <= 1");
        return acos(v);
    }
    if (name == "atan") return atan(v);
    
    throw runtime_error("ERROR Unknown function: " + name);
}

unique_ptr<Node> FuncNode::clone() const { 
    return make_unique<FuncNode>(name, funcArg->clone()); 
}

string FuncNode::toString() const { 
    return name + "(" + funcArg->toString() + ")"; 
}

bool FuncNode::isConstant() const { 
    return funcArg->isConstant(); 
}

unique_ptr<Node> FuncNode::derive(const string& var) const {
    auto derivArg = funcArg->derive(var);
    unique_ptr<Node> funcDeriv;
    if (name == "sin") funcDeriv = make_unique<FuncNode>("cos", funcArg->clone());
    else if (name == "cos") funcDeriv = make_unique<UnaryNode>('-', make_unique<FuncNode>("sin", funcArg->clone()));
    else if (name == "tan") funcDeriv = make_unique<BinaryNode>('/', make_unique<NumNode>(1), 
        make_unique<BinaryNode>('^', make_unique<FuncNode>("cos", funcArg->clone()), make_unique<NumNode>(2)));
    else if (name == "log") funcDeriv = make_unique<BinaryNode>('/', make_unique<NumNode>(1), funcArg->clone());
    else if (name == "exp") funcDeriv = make_unique<FuncNode>("exp", funcArg->clone());
    else if (name == "sqrt") {
        funcDeriv = make_unique<BinaryNode>('/', make_unique<NumNode>(1), 
             make_unique<BinaryNode>('*', make_unique<NumNode>(2), make_unique<FuncNode>("sqrt", funcArg->clone())));
    }
    else if (name == "asin") funcDeriv = make_unique<BinaryNode>('/', make_unique<NumNode>(1), 
        make_unique<FuncNode>("sqrt", make_unique<BinaryNode>('-', make_unique<NumNode>(1), 
        make_unique<BinaryNode>('^', funcArg->clone(), make_unique<NumNode>(2)))));
    else if (name == "acos") funcDeriv = make_unique<UnaryNode>('-', 
        make_unique<BinaryNode>('/', make_unique<NumNode>(1), 
        make_unique<FuncNode>("sqrt", make_unique<BinaryNode>('-', make_unique<NumNode>(1), 
        make_unique<BinaryNode>('^', funcArg->clone(), make_unique<NumNode>(2))))));
    else if (name == "atan") funcDeriv = make_unique<BinaryNode>('/', make_unique<NumNode>(1), 
        make_unique<BinaryNode>('+', make_unique<NumNode>(1), 
        make_unique<BinaryNode>('^', funcArg->clone(), make_unique<NumNode>(2))));
    else funcDeriv = make_unique<NumNode>(0);
    
    return make_unique<BinaryNode>('*', move(funcDeriv), move(derivArg));
}
