#pragma once

#include <map>
#include <iostream>

#include "visitor.hpp"

class Interpreter : public Visitor {
public:
    Interpreter() {};
    ~Interpreter() {};
    void Visit(ASTNode* node)override {};
    void Visit(Expression* expression)override {};
    void Visit(Number* expression)override;
    void Visit(Variable* expression)override;
    void Visit(BinaryExpression* expression)override;
    void Visit(Comparison* expression)override;
    void Visit(Statement* statement)override {};
    void Visit(Assignment* assignment)override;
    void Visit(Declaration* declaration)override;
    void Visit(PrintStatement* print_statement)override;
    void Visit(IfStatement* statement)override;
    void Visit(StatementList* statement)override;
    void Visit(std::string& program)override;

private:
    std::map<std::string, int> variables_;
    bool is_tos_expression_;
    int tos_value_;

    void SetTosValue(int value);
    void UnsetTosValue();
};