#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <fstream>
#include <map>
#include "../visitors/visitor.hpp"
// #include "../visitors/interpreter.hpp"
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

class FunctionCall : public Expression {
    public:
    std::string name;
    std::vector<std::unique_ptr<Expression>> args;
    FunctionCall(std::string name, std::vector<std::unique_ptr<Expression>> args) :
        name(name), args(std::move(args)) {}
    void Accept (Visitor* visitor) override {
        visitor->Visit(this);
    }
    FunctionCall(FunctionCall&&) = default;
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


enum class Types {
    INT, 
    FLOAT, // not implemented
    CHAR, // not implemented
};

class Type : public ASTNode {
public: 
    Types type;
    Type(Types tp) : type(tp) {}
    void Accept (Visitor* visitor) {
        visitor->Visit(this);
    }
    Type(Type&&) = default;

};

class Parameter : public ASTNode {
public: 
    std::string name;
    std::unique_ptr<Type> type;
    Parameter(std::string nm, 
        std::unique_ptr<Type> tp) :
        name(nm),
         type(std::move(tp)) {}
    void Accept (Visitor* visitor) override {
        visitor->Visit(this);
    }
    Parameter(Parameter&&) = default;

};

class FunctionDeclaration : public ASTNode {
    public:
    std::string name;
    std::vector<std::unique_ptr<Parameter>> params;
    std::unique_ptr<StatementList> body;
    std::unique_ptr<Type> returnType;
    FunctionDeclaration(std::string name, std::vector<std::unique_ptr<Parameter>> params, std::unique_ptr<StatementList> body, std::unique_ptr<Type> returnTp) :
        name(name), params(std::move(params)), body(std::move(body)), returnType(std::move(returnTp)) {}
    void Accept (Visitor* visitor) override {
        visitor->Visit(this);
    }
    FunctionDeclaration(FunctionDeclaration&&) = default;

};

class Statement : public ASTNode {};

class ReturnStatement : public Statement {
    public:
    std::unique_ptr<Expression> expression;
    ReturnStatement(std::unique_ptr<Expression> expr) :
        expression(std::move(expr)) {}
    void Accept (Visitor* visitor) override {
        visitor->Visit(this);
    }
    ReturnStatement(ReturnStatement&&) = default;

};

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
    std::unique_ptr<Type> varType;
    Declaration(std::string name, std::unique_ptr<Type> type) : varName(name), varType(std::move(type)) {}
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


class ProgramBlock : public ASTNode {
public:
    std::unique_ptr<Statement> statement = nullptr;
    std::unique_ptr<FunctionDeclaration> function = nullptr;
    ProgramBlock(std::unique_ptr<Statement> sttmnt) :
        statement(std::move(sttmnt)) {}
    ProgramBlock(std::unique_ptr<FunctionDeclaration> func) :
        function(std::move(func)) {}
    ProgramBlock(std::unique_ptr<Statement> sttmnt, std::unique_ptr<FunctionDeclaration> func) :
        statement(std::move(sttmnt)), function(std::move(func)) {}

    void Accept (Visitor* visitor) override {
        if (statement != nullptr) {
            visitor->Visit(statement.get());
        } else {
            visitor->Visit(function.get());
        }
    }   
    ProgramBlock(ProgramBlock&&) = default;

};

class ProgramBlocks : public ASTNode {
public:
    std::vector<std::unique_ptr<ProgramBlock>> blocks;
    void Accept (Visitor* visitor) override {
        visitor->Visit(this);
    }
};