#include "interpreter.hpp"
#include "../parsing/ast.hpp"


void Interpreter::Visit(ASTNode* node) {
    // cannot go here
}
    
void Interpreter::Visit(std::string& program) {
    // TODO
}

void Interpreter::Visit(ProgramBlocks* programBlocks) {
    for (auto&& block: programBlocks->blocks) {
        block->Accept(this);
    }
    // UnsetCalcedValue();
    // UnsetTosValue();
}
void Interpreter::Visit(ProgramBlock* programBlock) {
    // cannot go here
}


void Interpreter::Visit(FunctionDeclaration* func) {
    if (variables_.count(func->name) == 0) {
        variables_.emplace(func->name, 0);
        functions_.emplace(func->name, func);
    }
    if (func->name == "main") {
        func->body->Accept(this);
    }
}
void Interpreter::Visit(Parameter* parameter) {
    // shouldn't go here
}
void Interpreter::Visit(Type* type) {
    // shouldn't go here
}

void Interpreter::Visit(StatementList* statement_list) {
    for (auto&& stmnt: statement_list->statements) {
        stmnt->Accept(this);
    }
    // UnsetTosValue();
}

void Interpreter::Visit(Statement* statement) {
    // cannot go here
}
void Interpreter::Visit(Assignment* assignment) {
    assignment->expression->Accept(this);
    variables_[assignment->variable] = calced_value_;

    // UnsetCalcedValue();
}
void Interpreter::Visit(Declaration* declaration) {
    if (variables_.count(declaration->varName) == 0) {
        variables_.emplace(declaration->varName, 0);
    } else {
        std::cerr << "Ошибка: переменная" << declaration->varName << "уже существует\n";
    }
}
void Interpreter::Visit(PrintStatement* print_statement) {
    print_statement->expression->Accept(this);
    SetTosValue(calced_value_);
    std::cout << tos_value_ << std::endl;
}
void Interpreter::Visit(ReturnStatement* returnStatement) {
    returnStatement->expression->Accept(this);
    std::cout << "expression = " << calced_value_ << std::endl;
    // SetCalcedValue(calced_value_);
}
void Interpreter::Visit(IfStatement* statement) {
    statement->condition->Accept(this);
    if (is_calced_expression_) {
        if (calced_value_) {
            statement->thenBranch->Accept(this);
        } else if (statement->elseBranch) {
            statement->elseBranch->Accept(this);
        }
    } else {
        std::cerr << "Ошибка: Некорректное условие в if-выражении\n";
    }
}

void Interpreter::Visit(Expression* expression) {
    // cannot go here
}
void Interpreter::Visit(Number* expression) {
    SetCalcedValue(expression->value);
}
void Interpreter::Visit(Variable* expression) {
    if (variables_.count(expression->name) != 0) {
        SetCalcedValue(variables_[expression->name]);
    } else {
        std::cerr << "Ошибка: переменной " << expression->name << " не существует\n";
    }
}
void Interpreter::Visit(BinaryExpression* expression) {
    expression->left->Accept(this);
    int value = calced_value_;
    expression->right->Accept(this);

    switch (expression->op[0]) {
            case '+':
                value += calced_value_;
                break;
            case '-':
                value -= calced_value_;
                break;
            case '*':
                value *= calced_value_;
                break;
            case '/':
                value /= calced_value_;
                break;
            default:
                std::cout << "Неизвестная бинарная операция " << expression->op << "\n";
                return;
    }

    SetCalcedValue(value);
}
void Interpreter::Visit(Comparison* expression) {
    expression->left->Accept(this);
    int value = calced_value_;
    expression->right->Accept(this);

    switch (expression->op[0]) {
            case '<':
                value = (value <= calced_value_);
                break;
            case '>':
                value = (value >= calced_value_);
                break;
            case '=':
                value = (value == calced_value_);
                break;
            case '!':
                value = (value != calced_value_);
                break;
            default:
                std::cout << "Неизвестная бинарная операция " << expression->op << "\n";
                return;
    }

    SetCalcedValue(value);
}
void Interpreter::Visit(FunctionCall* functionCall) {
    if (variables_.count(functionCall->name) == 0) {
        std::cerr << "Функции " << functionCall->name << " не существует." << std::endl;
        return;
    }
    FunctionDeclaration* func = functions_[functionCall->name];
    std::string err_str = CheckArgs (functionCall, func);
    if (err_str != "") {
        std::cerr << err_str << std::endl;
        return;
    }
    func->body->Accept(this);
}


std::string Interpreter::CheckArgs(FunctionCall* functionCall, FunctionDeclaration* functionDeclaration) {
    // пока локальные переменные перекрывают глобальные так, что более к глобальным достпуа нет
    size_t arg_len = functionCall->args.size();
    size_t param_len = functionDeclaration->params.size();
    if (arg_len != param_len) {
        return "некорректное количество аргументов";
    }
    for (int i = 0; i < param_len; i++) {
        functionDeclaration->params[i]->Accept(this);
        functionCall->args[i]->Accept(this);
        variables_[functionDeclaration->params[i]->name] = calced_value_; 
        // UnsetCalcedValue();
    }
    return "";
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

void Interpreter::SetCalcedValue(int value) {
    calced_value_ = value;
    is_calced_expression_ = true;
}
void Interpreter::UnsetCalcedValue() {
    calced_value_ = 0;
    is_calced_expression_ = false;
}