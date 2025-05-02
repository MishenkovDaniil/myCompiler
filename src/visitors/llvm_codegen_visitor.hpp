#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <stack>
#include <memory>

#include "../parsing/ast.hpp"  
#include "visitor.hpp"


class LLVMCodeGenVisitor : public Visitor {
public:
    LLVMCodeGenVisitor();
    void generateIR(const std::string& outputFilename);

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
    llvm::LLVMContext context;
    std::unique_ptr<llvm::Module> module;
    llvm::IRBuilder<> builder;
    
    std::stack<llvm::Value*> valueStack;  
    std::map<std::string, llvm::AllocaInst*> symbolTable; 
    
    llvm::AllocaInst* createEntryBlockAlloca(llvm::Function* func, const std::string& name);
    llvm::Type* getLLVMType(Type* type);
    void declarePrintf();
};