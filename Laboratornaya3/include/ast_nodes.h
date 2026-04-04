#ifndef AST_NODES_H
#define AST_NODES_H

#include "ast.h"

class NumNode : public Node {
    double numVal;
public:
    NumNode(double v);
    double eval(const map<string, double>& vars) const override;
    unique_ptr<Node> clone() const override;
    unique_ptr<Node> derive(const string& var) const override;
    string toString() const override;
    bool isConstant() const override;
};

class VarNode : public Node {
    string varName;
public:
    VarNode(const string& n);
    double eval(const map<string, double>& vars) const override;
    unique_ptr<Node> clone() const override;
    unique_ptr<Node> derive(const string& var) const override;
    string toString() const override;
    bool isConstant() const override;
};

class UnaryNode : public Node {
    char op;
    unique_ptr<Node> subNode;
public:
    UnaryNode(char o, unique_ptr<Node> c);
    double eval(const map<string, double>& vars) const override;
    unique_ptr<Node> clone() const override;
    unique_ptr<Node> derive(const string& var) const override;
    string toString() const override;
    bool isConstant() const override;
};

class BinaryNode : public Node {
    char op;
    unique_ptr<Node> leftNode, rightNode;
public:
    BinaryNode(char o, unique_ptr<Node> l, unique_ptr<Node> r);
    double eval(const map<string, double>& vars) const override;
    unique_ptr<Node> clone() const override;
    string toString() const override;
    bool isConstant() const override;
    unique_ptr<Node> derive(const string& var) const override;
};

class FuncNode : public Node {
    string name;
    unique_ptr<Node> funcArg;
public:
    FuncNode(const string& n, unique_ptr<Node> a);
    double eval(const map<string, double>& vars) const override;
    unique_ptr<Node> clone() const override;
    string toString() const override;
    bool isConstant() const override;
    unique_ptr<Node> derive(const string& var) const override;
};

#endif