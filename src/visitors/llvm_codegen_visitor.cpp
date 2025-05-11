#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/Verifier.h>

#include "llvm_codegen_visitor.hpp"

LLVMCodeGenVisitor::LLVMCodeGenVisitor() 
    : module(std::make_unique<llvm::Module>("main", context)), builder(context) {}

// Генерация аллокации переменной в entry block
llvm::AllocaInst* LLVMCodeGenVisitor::createEntryBlockAlloca(llvm::Function* func, const std::string& name) {
    llvm::IRBuilder<> tmpBuilder(&func->getEntryBlock(), func->getEntryBlock().begin());
    return tmpBuilder.CreateAlloca(llvm::Type::getInt32Ty(context), nullptr, name);
}

llvm::Type* LLVMCodeGenVisitor::getLLVMType(Type* type) {
    if (type->type == Types::INT) return llvm::Type::getInt32Ty(context);
    throw std::runtime_error("Unknown type: ");
}


void LLVMCodeGenVisitor::Visit(ASTNode* node) {
    // cannot be called
}
    
void LLVMCodeGenVisitor::Visit(std::string& program) {
    // cannot be called
}

void LLVMCodeGenVisitor::Visit(ProgramBlocks* programBlocks) {
    for (auto&& block : programBlocks->blocks) {
        block->Accept(this);
    }
}
void LLVMCodeGenVisitor::Visit(ProgramBlock* programBlock) {
    // cannot be called
}

void LLVMCodeGenVisitor::Visit(FunctionDeclaration* funcDecl) {
    std::vector<llvm::Type*> paramTypes;
    for (auto& param : funcDecl->params) {
        paramTypes.push_back(getLLVMType(param->type.get()));
    }

    llvm::FunctionType* funcType = llvm::FunctionType::get(
        getLLVMType(funcDecl->returnType.get()), paramTypes, false
    );

    llvm::Function* func = llvm::Function::Create(
        funcType, llvm::Function::ExternalLinkage, funcDecl->name, module.get()
    );

    // Entry block
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(context, "entry", func);
    builder.SetInsertPoint(entry);

    // Добавляем параметры в symbolTable
    unsigned idx = 0;
    for (auto& arg : func->args()) {
        llvm::AllocaInst* alloca = createEntryBlockAlloca(func, funcDecl->params[idx]->name);
        builder.CreateStore(&arg, alloca);
        symbolTable[funcDecl->params[idx]->name] = alloca;
        idx++;
    }

    funcDecl->body->Accept(this);

    // Очистка symbolTable от параметров 
    for (auto& param : funcDecl->params) {
        symbolTable.erase(param->name);
    }
}
void LLVMCodeGenVisitor::Visit(Parameter* parameter) {
    // not called
}
void LLVMCodeGenVisitor::Visit(Type* type) {
    // not called
}

void LLVMCodeGenVisitor::Visit(StatementList* statementList) {
    for (auto&& statement : statementList->statements) {
        statement->Accept(this);
    }
}

void LLVMCodeGenVisitor::Visit(Statement* statement) {
    // cannot be called
}
void LLVMCodeGenVisitor::Visit(Assignment* assignment) {
    assignment->expression->Accept(this);  
    
    llvm::Value* value = valueStack.top();
    valueStack.pop();  
    
    llvm::AllocaInst* var = symbolTable[assignment->variable];
    builder.CreateStore(value, var);
}
void LLVMCodeGenVisitor::Visit(Declaration* declaration) {
    llvm::Function* func = builder.GetInsertBlock()->getParent();
    llvm::AllocaInst* alloca = createEntryBlockAlloca(func, declaration->varName);
    symbolTable[declaration->varName] = alloca;
}
void LLVMCodeGenVisitor::Visit(PrintStatement* printStatement) {
    declarePrintf(); 
    
    printStatement->expression->Accept(this); 
    
    llvm::Value* value = valueStack.top();
    valueStack.pop();
    
    llvm::Value* formatStr = builder.CreateGlobalString("%d\n");
    llvm::Value* formatCast = builder.CreatePointerCast(
        formatStr, 
        llvm::PointerType::get(context, 0)
    );

    builder.CreateCall(
        module->getFunction("printf"), 
        {formatCast, value},
        "printfCall"
    );
}

void LLVMCodeGenVisitor::Visit(ReturnStatement* returnStatement) {
    returnStatement->expression->Accept(this); 
    
    llvm::Value* retVal = valueStack.top();
    valueStack.pop();  
    
    builder.CreateRet(retVal);
}
void LLVMCodeGenVisitor::Visit(IfStatement* statement) {
    llvm::Function* func = builder.GetInsertBlock()->getParent();
    
    llvm::BasicBlock* thenBB = llvm::BasicBlock::Create(context, "then", func);
    llvm::BasicBlock* elseBB = llvm::BasicBlock::Create(context, "else");
    llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(context, "merge");
    
    // If
    statement->condition->Accept(this);
    llvm::Value* cond = valueStack.top();
    valueStack.pop();
    builder.CreateCondBr(cond, thenBB, elseBB);
    
    // Then
    builder.SetInsertPoint(thenBB);
    statement->thenBranch->Accept(this);
    if (!builder.GetInsertBlock()->getTerminator()) {
        builder.CreateBr(mergeBB);
    }

    // Else
    elseBB->insertInto(func);
    builder.SetInsertPoint(elseBB);
    if (statement->elseBranch) {
        statement->elseBranch->Accept(this);
    }
    if (!builder.GetInsertBlock()->getTerminator()) {
        builder.CreateBr(mergeBB);
    }

    // Merge (исправлено: используем insertInto)
    mergeBB->insertInto(func);
    builder.SetInsertPoint(mergeBB);
}

void LLVMCodeGenVisitor::Visit(Expression* expression) {
    // cannot be called
}
void LLVMCodeGenVisitor::Visit(Number* expression) {
    valueStack.push(llvm::ConstantInt::get(context, llvm::APInt(32, expression->value)));
}
void LLVMCodeGenVisitor::Visit(Variable* expression) {
    llvm::AllocaInst* alloca = symbolTable[expression->name];
    valueStack.push(builder.CreateLoad(alloca->getAllocatedType(), alloca, expression->name));
}
void LLVMCodeGenVisitor::Visit(BinaryExpression* expression) {
    expression->left->Accept(this);

    llvm::Value* left = valueStack.top();
    valueStack.pop();

    expression->right->Accept(this);
    llvm::Value* right = valueStack.top();
    valueStack.pop();
    
    switch (expression->op[0]) {
        case '+': 
            valueStack.push(builder.CreateAdd(left, right, "addtmp"));
            break;
        case '-': 
            valueStack.push(builder.CreateSub(left, right, "subtmp"));
            break;
        case '*': 
            valueStack.push(builder.CreateMul(left, right, "multmp"));
            break;
        case '/': 
            valueStack.push(builder.CreateSDiv(left, right, "divtmp"));
            break;
        default: 
            throw std::runtime_error("Unknown operator");
    }
}
void LLVMCodeGenVisitor::Visit(Comparison* comparison) {
    comparison->left->Accept(this);
    llvm::Value* left = valueStack.top();
    valueStack.pop();
    
    comparison->right->Accept(this);
    llvm::Value* right = valueStack.top();
    valueStack.pop();
    
    llvm::CmpInst::Predicate pred;
    if (comparison->op == "==") pred = llvm::CmpInst::ICMP_EQ;
    else if (comparison->op == "!=") pred = llvm::CmpInst::ICMP_NE;
    else if (comparison->op == "<") pred = llvm::CmpInst::ICMP_SLT;
    else if (comparison->op == ">") pred = llvm::CmpInst::ICMP_SGT;
    else if (comparison->op == "<=") pred = llvm::CmpInst::ICMP_SLE;
    else if (comparison->op == ">=") pred = llvm::CmpInst::ICMP_SGE;
    else throw std::runtime_error("Unknown comparison operator");
    
    valueStack.push(builder.CreateICmp(pred, left, right, "cmptmp"));
}
void LLVMCodeGenVisitor::Visit(FunctionCall* funcCall) {
    llvm::Function* callee = module->getFunction(funcCall->name);
    std::vector<llvm::Value*> args;
    
    for (auto& argExpr : funcCall->args) {
        argExpr->Accept(this);
        args.push_back(valueStack.top());
        valueStack.pop();
    }
    
    llvm::Value* result = builder.CreateCall(callee, args);
    valueStack.push(result); 
}

void LLVMCodeGenVisitor::generateIR(const std::string& outputFilename) {
    std::error_code EC;
    llvm::raw_fd_ostream out(outputFilename, EC);
    module->print(out, nullptr);
}

void LLVMCodeGenVisitor::declarePrintf() {
    if (!module->getFunction("printf")) {
        std::vector<llvm::Type*> printfArgs;
        printfArgs.push_back(llvm::PointerType::get(context, 0));
        
        llvm::FunctionType* printfType = llvm::FunctionType::get(
            llvm::Type::getInt32Ty(context),
            printfArgs,
            true
        );
        
        llvm::Function::Create(
            printfType,
            llvm::Function::ExternalLinkage,
            "printf",
            module.get()
        );
    }
}