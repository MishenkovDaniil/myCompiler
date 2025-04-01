#pragma once

#include "forward_decl.hpp"
#include <iostream>

class Visitor {
public:
    virtual void Visit(ASTNode* node) = 0;
    virtual void Visit(Expression* expression) = 0;
    virtual void Visit(Number* expression) = 0;
    virtual void Visit(Variable* expression) = 0;
    virtual void Visit(BinaryExpression* expression) = 0;
    virtual void Visit(Comparison* expression) = 0;
    virtual void Visit(Statement* statement) = 0;
    virtual void Visit(Assignment* assignment) = 0;
    virtual void Visit(Declaration* declaration) = 0;
    virtual void Visit(PrintStatement* print_statement) = 0;
    virtual void Visit(IfStatement* statement) = 0;
    virtual void Visit(StatementList* statement) = 0;
    virtual void Visit(std::string& program) = 0;
};