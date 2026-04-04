#ifndef AST_H
#define AST_H

#include <map>
#include <memory>
#include <string>

using namespace std;

class Node {
public:
    virtual ~Node() = default;
    virtual double eval(const map<string, double>& vars) const = 0;
    virtual unique_ptr<Node> clone() const = 0;
    virtual unique_ptr<Node> derive(const string& var) const = 0;
    virtual string toString() const = 0;
    virtual bool isConstant() const = 0;
};

#endif