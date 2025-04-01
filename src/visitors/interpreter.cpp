#include "interpreter.hpp"
#include "../parsing/ast.hpp"

void Interpreter::Visit(Number* expression) {
    // std::cout << "Visit Number\n";
    SetTosValue(expression->value);
}

void Interpreter::Visit(Variable* expression) {
    // std::cout << "Visit Variable\n";
    if (variables_.count(expression->name) != 0) {
        SetTosValue(variables_[expression->name]);
    } else {
        std::cerr << "Ошибка: переменной" << expression->name << "не существует\n";
    }
}

void Interpreter::Visit(BinaryExpression* expression) {
    // std::cout << "Visit BinaryExpression\n";
    expression->left->Accept(this);
    int value = tos_value_;
    expression->right->Accept(this);

    switch (expression->op[0]) {
            case '+':
                value += tos_value_;
                break;
            case '-':
                value -= tos_value_;
                break;
            case '*':
                value *= tos_value_;
                break;
            case '/':
                value /= tos_value_;
                break;
            default:
                std::cout << "Неизвестная бинарная операция " << expression->op << "\n";
                return;
    }

    SetTosValue(value);
}

void Interpreter::Visit(Comparison* expression) {
    // std::cout << "Visit Comparison\n";
    expression->left->Accept(this);
    int value = tos_value_;
    expression->right->Accept(this);

    switch (expression->op[0]) {
            case '<':
                value = (value <= tos_value_);
                break;
            case '>':
                value = (value >= tos_value_);
                break;
            case '=':
                value = (value == tos_value_);
                break;
            case '!':
                value = (value != tos_value_);
                break;
            default:
                std::cout << "Неизвестная бинарная операция " << expression->op << "\n";
                return;
    }

    SetTosValue(value);
}

void Interpreter::Visit(Assignment* assignment) {
    // std::cout << "Visit Assignment\n";
    assignment->expression->Accept(this);
    variables_[assignment->variable] = tos_value_;

    UnsetTosValue();
}

void Interpreter::Visit(Declaration* declaration) {
    // std::cout << "Visit Declaration\n";
    if (variables_.count(declaration->varName) == 0) {
        variables_.emplace(declaration->varName, 0);
    } else {
        std::cerr << "Ошибка: переменная" << declaration->varName << "уже существует\n";
    }
}

void Interpreter::Visit(PrintStatement* print_statement) {
    // std::cout << "Visit PrintStatement\n";
    print_statement->expression->Accept(this);
    std::cout << tos_value_ << std::endl;
}

void Interpreter::Visit(IfStatement* statement) {
    // std::cout << "Visit IfStatement\n";
    statement->condition->Accept(this);
    if (is_tos_expression_) {
        if (tos_value_) {
            statement->thenBranch->Accept(this);
        } else if (statement->elseBranch) {
            statement->elseBranch->Accept(this);
        }
    } else {
        std::cerr << "Ошибка: Некорректное условие в if-выражении\n";
    }
}

void Interpreter::Visit(StatementList* statement_list) {
    // std::cout << "Visit StatementList\n";
    for (auto&& stmnt: statement_list->statements) {
        stmnt->Accept(this);
    }
    UnsetTosValue();
}

void Interpreter::Visit(std::string& program) {
    // std::cout << "Visit program\n";
}


void Interpreter::SetTosValue(int value) {
    // std::cout << "tos value - " << value << "\n";
    tos_value_ = value;
    is_tos_expression_ = true;
}

void Interpreter::UnsetTosValue() {
    tos_value_ = 0;
    is_tos_expression_ = false;
}
