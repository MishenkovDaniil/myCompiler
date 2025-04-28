#include "print_visitor.hpp"
#include "../parsing/ast.hpp"

SymbolTreeVisitor::SymbolTreeVisitor(const std::string& filename) : stream(filename) {
}

SymbolTreeVisitor::~SymbolTreeVisitor() {
    stream.close();
}

void SymbolTreeVisitor::Visit(ASTNode* node) {
    std::cout << "error: visited abstract class ASTNode\n";
}
    
void SymbolTreeVisitor::Visit(std::string& program) {
    // cannot be called
}

void SymbolTreeVisitor::Visit(ProgramBlocks* programBlocks) {
    PrintTabs();
    stream << "ProgramBlocks: " << std::endl;

    ++num_tabs;
    for (auto&& block : programBlocks->blocks) {
        block->Accept(this);
    }
    --num_tabs;
}
void SymbolTreeVisitor::Visit(ProgramBlock* programBlock) {
    // cannot be called
}

void SymbolTreeVisitor::Visit(FunctionDeclaration* funcDecl) {
    PrintTabs();
    stream << "FunctionDeclaration: " << funcDecl->name << std::endl;
    ++num_tabs;
    for (auto&& var : funcDecl->params) {
        var->Accept(this);
    }
    ++num_tabs;
    ++num_tabs;
    stream << "ReturnType: " << std::endl;
    funcDecl->returnType->Accept(this);
    --num_tabs;
    funcDecl->body->Accept(this);
    --num_tabs;
    --num_tabs;
}
void SymbolTreeVisitor::Visit(Parameter* parameter) {
    PrintTabs();
    stream << "Parameter: " << parameter->name << std::endl;
    ++num_tabs;
    parameter->type->Accept(this);
    --num_tabs;
}

void SymbolTreeVisitor::Visit(Type* type) {
    PrintTabs();
    switch (type->type) {
        case Types::INT : {
            stream << "Type: INT" << std::endl;
            break;
        }
        default: {
            stream << "Type: ERROR_TYPE" << std::endl;
        }
    }
}

void SymbolTreeVisitor::Visit(StatementList* statement) {
    PrintTabs();
    stream << "StatementList: " << std::endl;

    ++num_tabs;
    for (auto&& stmnt : statement->statements) {
        stmnt->Accept(this);
    }
    --num_tabs;
}

void SymbolTreeVisitor::Visit(Statement* statement) {
    std::cout << "error: visited abstract class Statement\n";
}
void SymbolTreeVisitor::Visit(Assignment* assignment) {
    PrintTabs();
    stream << "Assignment: " << assignment->variable << std::endl;
    ++num_tabs;
    assignment->expression->Accept(this);
    --num_tabs;
}
void SymbolTreeVisitor::Visit(Declaration* declaration) {
    PrintTabs();
    stream << "Declaration: " << declaration->varName << std::endl;
    ++num_tabs;
    declaration->varType->Accept(this);
    --num_tabs;
}
void SymbolTreeVisitor::Visit(PrintStatement* print_statement) {
    PrintTabs();
    stream << "PrintStatement: " << std::endl;
    ++num_tabs;
    print_statement->expression->Accept(this);
    --num_tabs;
}
void SymbolTreeVisitor::Visit(ReturnStatement* returnStatement) {
    PrintTabs();

    stream << "ReturnStatement: " << std::endl;

    ++num_tabs;
    returnStatement->expression->Accept(this);
    --num_tabs;
}
void SymbolTreeVisitor::Visit(IfStatement* statement) {
    PrintTabs();
    stream << "IfStatement: " << std::endl;
    ++num_tabs;
    statement->condition->Accept(this);
    ++num_tabs;
    statement->thenBranch->Accept(this);
    --num_tabs;
    statement->elseBranch->Accept(this);
    --num_tabs;
}

void SymbolTreeVisitor::Visit(Expression* expression) {
    std::cout << "error: visited abstract class Expression\n";
}
void SymbolTreeVisitor::Visit(Number* expression) {
    PrintTabs();
    stream << "Number: " << expression->value << std::endl;
}
void SymbolTreeVisitor::Visit(Variable* expression) {
    PrintTabs();
    stream << "Variable: " << expression->name << std::endl;
}
void SymbolTreeVisitor::Visit(BinaryExpression* expression) {
    PrintTabs();
    stream << "BinaryExpression is:" << expression->op << std::endl;

    ++num_tabs;
    expression->left->Accept(this);
    expression->right->Accept(this);
    --num_tabs;
}
void SymbolTreeVisitor::Visit(Comparison* expression) {
   PrintTabs();
    stream << "Comparison is:" << expression->op << std::endl;

    ++num_tabs;
    expression->left->Accept(this);
    expression->right->Accept(this);
    --num_tabs;
}
void SymbolTreeVisitor::Visit(FunctionCall* funcCall) {
    PrintTabs();
    stream << "FunctionCall: " << funcCall->name << std::endl;
    ++num_tabs;
    for (auto&& var : funcCall->args) {
        var->Accept(this);
    }
    --num_tabs;
}

void SymbolTreeVisitor::PrintTabs() {
    for (int i = 0; i < num_tabs; ++i) {
        stream << '\t';
    }
}