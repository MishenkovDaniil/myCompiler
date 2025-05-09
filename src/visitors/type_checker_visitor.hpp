#pragma once 

#include "visitor.hpp"

#include <fstream>
#include <string>
#include <stdexcept>
#include <stack>
#include <vector>
#include <unordered_map>
#include <memory>
#include "scope_tree_visitor.hpp"

#include <vector>
#include <string>
#include <unordered_map>

class TypeCheckerVisitor : public Visitor {
    ScopeTree& scopeTree;
    std::vector<std::string> errors;
    Scope* currentScope;
    Types currentFunctionReturnType;
    Types currentExprType;

    // Вспомогательные методы
    Symbol* findSymbol(const std::string& name);

    void error(const std::string& msg, const ASTNode& node);

public:
    explicit TypeCheckerVisitor(ScopeTree& tree); 
    const std::vector<std::string>& getErrors() const { return errors; }

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
};