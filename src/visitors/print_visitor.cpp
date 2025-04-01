#include "print_visitor.hpp"
#include "../parsing/ast.hpp"

SymbolTreeVisitor::SymbolTreeVisitor(const std::string& filename) : stream(filename) {
}

SymbolTreeVisitor::~SymbolTreeVisitor() {
    stream.close();
}

void SymbolTreeVisitor::Visit(Number* expression) {
    PrintTabs();
    stream << "Variable: " << expression->value << std::endl;
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
}

void SymbolTreeVisitor::Visit(PrintStatement* print_statement) {
    PrintTabs();
    stream << "PrintStatement: " << std::endl;
    ++num_tabs;
    print_statement->expression->Accept(this);
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

void SymbolTreeVisitor::Visit(StatementList* statement) {
    PrintTabs();
    stream << "StatementList: " << std::endl;

    ++num_tabs;
    for (auto&& stmnt : statement->statements) {
        stmnt->Accept(this);
    }
    --num_tabs;
}

void SymbolTreeVisitor::Visit(std::string& program) {
    // Todo
}

void SymbolTreeVisitor::PrintTabs() {
    for (int i = 0; i < num_tabs; ++i) {
        stream << '\t';
    }
}