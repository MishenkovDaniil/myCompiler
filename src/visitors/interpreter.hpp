#pragma once

#include <map>
#include <iostream>

#include "visitor.hpp"

class Interpreter : public Visitor {
public:
    Interpreter() {};
    ~Interpreter() {};
    void Visit(ASTNode* node) override;
    
    void Visit(std::string& program) override;

    void Visit(ProgramBlocks* programBlocks) override;
    void Visit(ProgramBlock* programBlock) override;


    void Visit(FunctionDeclaration* statement) override;
    void Visit(Parameter* parameter) override;
    void Visit(Type* type) override;

    void Visit(StatementList* statement) override;
    
    void Visit(Statement* statement) override;
    void Visit(Assignment* assignment) override;
    void Visit(Declaration* declaration) override;
    void Visit(PrintStatement* print_statement) override;
    void Visit(ReturnStatement* returnStatement) override;
    void Visit(IfStatement* statement) override;

    void Visit(Expression* expression) override;
    void Visit(Number* expression) override;
    void Visit(Variable* expression) override;
    void Visit(BinaryExpression* expression) override;
    void Visit(Comparison* expression) override;
    void Visit(FunctionCall* statement) override;

private:
    std::map<std::string, int> variables_;
    std::map<std::string, FunctionDeclaration*> functions_;
    bool is_tos_expression_;
    int tos_value_;
    int calced_value_;
    bool is_calced_expression_;
    
    std::string CheckArgs(FunctionCall* functionCall, FunctionDeclaration* functionDeclaration);
    void SetTosValue(int value);
    void SetCalcedValue(int value);
    void UnsetCalcedValue();
    void UnsetTosValue();
};