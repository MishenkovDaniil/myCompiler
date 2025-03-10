#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <fstream>
#include <map>

class ASTNode { 
public: 
    virtual ~ASTNode() = default; 
    virtual void print(std::ofstream& ofstream, int shift) = 0;
    void PrintShift (std::ofstream& ofstream, int shift) {
        ofstream << "\n";
        for (int i = 0; i < shift; ++i){
            ofstream << "\t";
        }
        ofstream.flush();
    }
    virtual void fastExecute(std::map<std::string, int>& variables) = 0;
};


class Expression : public ASTNode {
public:
    virtual ASTNode *get(std::map<std::string, int>& variables) = 0;
};

class Number : public Expression {
public:
    int value;
    explicit Number(int v) : value(v) {}
    virtual void print(std::ofstream& ofstream, int shift) override {
        PrintShift(ofstream, shift);
        ofstream << "[NUM] " << value; 
    }
    virtual void fastExecute(std::map<std::string, int>& variables) override {
        return;
    }
    virtual ASTNode *get(std::map<std::string, int>& variables) override {
        return this;
    }
};
class Variable : public Expression {
public:
    std::string name;
    explicit Variable(std::string n) : name(std::move(n)) {}
    virtual void print(std::ofstream& ofstream, int shift) override {
        PrintShift(ofstream, shift);
        ofstream << "[VAR] " << name; 
    }
    virtual void fastExecute(std::map<std::string, int>& variables) override {
        return;
    }
    virtual ASTNode *get(std::map<std::string, int>& variables) override {
        if (variables.count(name) == 0) {
            std::cout << "переменной " << name << "не существует\n";
            return nullptr;
        } 
        return new Number(variables[name]);
    }
};
class BinaryExpression : public Expression {
public:
    std::string op;
    std::unique_ptr<Expression> left, right;
    BinaryExpression(std::string o, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
        : op(std::move(o)), left(std::move(l)), right(std::move(r)) {}
    virtual void print(std::ofstream& ofstream, int shift) override {
        PrintShift(ofstream, shift);
        ofstream << "[BINOP] " << op;
        left->print(ofstream, shift + 1);
        right->print(ofstream, shift + 1);
    }
    virtual void fastExecute(std::map<std::string, int>& variables) override {
        return;
    }
    virtual ASTNode *get(std::map<std::string, int>& variables) override {
        auto* lval = dynamic_cast<Number*>(left->get(variables));
        auto* rval = dynamic_cast<Number*>(right->get(variables));

        switch (op[0]) {
            case '+':
                return new Number(lval->value + rval->value);
            case '-':
                return new Number(lval->value - rval->value);
            case '*':
                return new Number(lval->value * rval->value);
            case '/':
                return new Number(lval->value / rval->value);
            default:
                std::cout << "Неизвестная бинарная операция " << op << "\n";
                return nullptr;
        }
    }
};
class Comparison : public Expression {
public:
    std::unique_ptr<Expression> left, right;
    std::string op;
    Comparison(std::string o, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
        : left(std::move(l)), op(std::move(o)), right(std::move(r)) {}
    virtual void print(std::ofstream& ofstream, int shift) override {
        PrintShift(ofstream, shift);
        ofstream << "[COMP] " << op;
        left->print(ofstream, shift + 1);
        right->print(ofstream, shift + 1);
    }
    virtual ASTNode* get(std::map<std::string, int>& variables) override {
        auto* lval = dynamic_cast<Number*>(left->get(variables));
        auto* rval = dynamic_cast<Number*>(right->get(variables));

        switch (op[0]) {
            case '<':
                return new Number(lval->value <= rval->value);
            case '=':
                return new Number(lval->value == rval->value);
            case '!':
                return new Number(lval->value != rval->value);
            case '>':
                return new Number(lval->value >= rval->value);
            default:
                std::cout << "Неизвестная операция сравнения " << op << "\n";
                return nullptr;
        }
    }
    virtual void fastExecute(std::map<std::string, int>& variables) override {
        return;
    }
};


class Statement : public ASTNode {};

class Assignment : public Statement {
public:
    std::string variable;
    std::unique_ptr<Expression> expression;
    Assignment(std::string var, std::unique_ptr<Expression> expr)
        : variable(std::move(var)), expression(std::move(expr)) {}
    virtual void print(std::ofstream& ofstream, int shift) override {
        PrintShift(ofstream, shift);
        ofstream << "[ASSIGN] " << variable << " to ";
        expression->print(ofstream, shift + 1);
    }
    virtual void fastExecute(std::map<std::string, int>& variables) override {
        auto* numNode = dynamic_cast<Number*>(expression->get(variables));
        if (numNode) {
            variables[variable] = numNode->value;
        } else {
            std::cerr << "Ошибка: Присваивание нечислового значения\n";
        }
    }
};
class Declaration : public Statement {
public:
    std::string varName;
    Declaration(std::string name) : varName(name) {}
    virtual void print(std::ofstream& ofstream, int shift) override {
        PrintShift(ofstream, shift);
        ofstream << "[DECLARE] " << varName;
    }
    virtual void fastExecute(std::map<std::string, int>& variables) override {
        if (variables.count(varName) == 0) {
            variables.emplace(varName, 0);
        } else {
            std::cerr << "Ошибка: переменная" << varName << "уже существует\n";
        }
    }
};
class PrintStatement : public Statement {
public:
    std::unique_ptr<Expression> expression;
    explicit PrintStatement(std::unique_ptr<Expression> expr) :
         expression(std::move(expr)) {}
    virtual void print(std::ofstream& ofstream, int shift) override {
        PrintShift(ofstream, shift);
        ofstream << "[FUNC] print";
        expression->print(ofstream, shift + 1);
    }
    virtual void fastExecute(std::map<std::string, int>& variables) override {
        auto* val = dynamic_cast<Number*>(expression->get(variables));
        std::cout << val->value << std::endl;
    }
};

class IfStatement : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> thenBranch;
    std::unique_ptr<Statement> elseBranch;

    IfStatement(std::unique_ptr<Expression> cond, std::unique_ptr<Statement> thenB, std::unique_ptr<Statement> elseB = nullptr)
        : condition(std::move(cond)), thenBranch(std::move(thenB)), elseBranch(std::move(elseB)) {}
    virtual void print(std::ofstream& ofstream, int shift) override {
        PrintShift(ofstream, shift);
        ofstream << "[IFELSE] if";
        condition->print(ofstream, shift + 1);
        PrintShift(ofstream, shift);
        ofstream << "[IFELSE] then";
        thenBranch->print(ofstream, shift + 1);
        PrintShift(ofstream, shift);
        ofstream << "[IFELSE] else";
        elseBranch->print(ofstream, shift + 1);
    }
    virtual void fastExecute(std::map<std::string, int>& variables) override {
        auto* condNum = dynamic_cast<Number*>(condition->get(variables));
        if (condNum) {
            if (condNum->value != 0) {
                thenBranch->fastExecute(variables);
            } else if (elseBranch) {
                elseBranch->fastExecute(variables);
            }
        } else {
            std::cerr << "Ошибка: Некорректное условие в if-выражении\n";
        }
    }
};
class StatementList : public Statement {
public:
    std::vector<std::unique_ptr<Statement>> statements;
    virtual void print(std::ofstream& ofstream, int shift = 0) override {
        PrintShift(ofstream, shift);
        for (auto&& statement : statements) {
            statement->print(ofstream, shift);
        }
    }
    virtual void fastExecute(std::map<std::string, int>& variables) override {
        for (auto&& statement : statements) {
            statement->fastExecute(variables);
        }
    }
};