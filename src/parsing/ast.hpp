#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <fstream>
#include <map>
#include "../visitors/visitor.hpp"
#include "../visitors/interpreter.hpp"
#include "../visitors/print_visitor.hpp"

class ASTNode { 
public: 
    virtual ~ASTNode() = default; 
    void PrintShift (std::ofstream& ofstream, int shift) {
        ofstream << "\n";
        for (int i = 0; i < shift; ++i){
            ofstream << "\t";
        }
        ofstream.flush();
    }
    virtual void Accept (Visitor* visitor) = 0;
};


class Expression : public ASTNode {
public:
    void Accept (Visitor* visitor) override = 0;
};

class Number : public Expression {
public:
    int value;
    explicit Number(int v) : value(v) {}
    void Accept (Visitor* visitor) override {
        visitor->Visit(this);
    }
};
class Variable : public Expression {
public:
    std::string name;
    explicit Variable(std::string n) : name(std::move(n)) {}
    void Accept (Visitor* visitor) override {
        visitor->Visit(this);
    }
};
class BinaryExpression : public Expression {
public:
    std::string op;
    std::unique_ptr<Expression> left, right;
    BinaryExpression(std::string o, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
        : op(std::move(o)), left(std::move(l)), right(std::move(r)) {}
    void Accept (Visitor* visitor) override {
        visitor->Visit(this);
    }
};
class Comparison : public Expression {
public:
    std::unique_ptr<Expression> left, right;
    std::string op;
    Comparison(std::string o, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
        : left(std::move(l)), op(std::move(o)), right(std::move(r)) {}
    void Accept (Visitor* visitor) override {
        visitor->Visit(this);
    }
};


class Statement : public ASTNode {};

class Assignment : public Statement {
public:
    std::string variable;
    std::unique_ptr<Expression> expression;
    Assignment(std::string var, std::unique_ptr<Expression> expr)
        : variable(std::move(var)), expression(std::move(expr)) {}
    void Accept (Visitor* visitor) override {
        visitor->Visit(this);
    }
};
class Declaration : public Statement {
public:
    std::string varName;
    Declaration(std::string name) : varName(name) {}
    void Accept (Visitor* visitor) override {
        visitor->Visit(this);
    }
};
class PrintStatement : public Statement {
public:
    std::unique_ptr<Expression> expression;
    explicit PrintStatement(std::unique_ptr<Expression> expr) :
         expression(std::move(expr)) {}
    void Accept (Visitor* visitor) override {
        visitor->Visit(this);
    }
};

class IfStatement : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> thenBranch;
    std::unique_ptr<Statement> elseBranch;

    IfStatement(std::unique_ptr<Expression> cond, std::unique_ptr<Statement> thenB, std::unique_ptr<Statement> elseB = nullptr)
        : condition(std::move(cond)), thenBranch(std::move(thenB)), elseBranch(std::move(elseB)) {}
    void Accept (Visitor* visitor) override {
        visitor->Visit(this);
    }
};
class StatementList : public Statement {
public:
    std::vector<std::unique_ptr<Statement>> statements;
    void Accept (Visitor* visitor) override {
        visitor->Visit(this);
    }
};