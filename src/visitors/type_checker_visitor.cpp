#include "type_checker_visitor.hpp"
#include "../parsing/ast.hpp"

/*TODO: 
    добавить все проверки из scope_tree_visitor,
    изменить проверки на функции 
    (сделать проверку пост, ращрешая определение функции после ее вызова), 
    возможно оставить некоторые проверки в scope_tree_visitor
    (по типу проверки типа в бинарных операциях, хотя бин операции могут взаимодействовать с функциями???)
*/

/*TODO NEXT: 
    добавить еще типы
    добавить блоки {{{}}}
    добавить классы???
*/


Symbol* TypeCheckerVisitor::findSymbol(const std::string& name) {
    Scope* scope = currentScope;
    while (scope) {
        auto it = scope->symbols.find(name);
        if (it != scope->symbols.end()) return &it->second;
        scope = scope->parent;
    }
    return nullptr;
}

void TypeCheckerVisitor::error(const std::string& msg, const ASTNode& node) {
    // errors.push_back("Line " + std::to_string(node.line) + ": " + msg);
    errors.push_back(msg);
}

explicit TypeCheckerVisitor::TypeCheckerVisitor(ScopeTree& tree) 
    : scopeTree(tree), currentScope(tree.getGlobalScope()) {}

void TypeCheckerVisitor::Visit(ASTNode* node) {
    // cannot be called
}

void TypeCheckerVisitor::Visit(std::string& program) {
    // shouldn't be called
}

void TypeCheckerVisitor::Visit(ProgramBlocks* programBlocks) {
    for (auto& block : programBlocks->blocks) {
        block->Accept(this);
    }
}
void TypeCheckerVisitor::Visit(ProgramBlock* programBlock) {
    // cannot be called
}


void TypeCheckerVisitor::Visit(FunctionDeclaration* funcDecl) {
    // Сохраняем контекст возвращаемого типа
    Types prevReturnType = currentFunctionReturnType;
    currentFunctionReturnType = funcDecl->returnType->type;

    // Входим в область видимости функции
    Scope* prevScope = currentScope;
    // scopeTree.enterScope(funcDecl->name); //TODO: replace with just entering scope, not adding a new one!!!
    currentScope = currentScope->enter(funcDecl->name);

    // Проверяем параметры
    for (auto& param : funcDecl->params) {
        if (!findSymbol(param->name)) {
            error("Undefined parameter: " + param->name, *funcDecl);
        }
    }

    // Проверяем тело функции
    funcDecl->body->Accept(this);

    // Восстанавливаем контекст
    currentScope = currentScope->exit();
    // scopeTree.exitScope(); // TODO: also replace for consistency
    // currentScope = prevScope;
    currentFunctionReturnType = prevReturnType;
}

void TypeCheckerVisitor::Visit(Parameter* parameter) {
    // TODO
}
void TypeCheckerVisitor::Visit(Type* type) {
    // TODO
}

void TypeCheckerVisitor::Visit(StatementList* statement) {
    // TODO
}

void TypeCheckerVisitor::Visit(Statement* statement) {
    // TODO
}
void TypeCheckerVisitor::Visit(Assignment* assignment) {
    // Проверка существования переменной
    Symbol* varSym = findSymbol(assignment->variable);
    if (!varSym) {
        error("Undefined variable: " + assignment->variable, *assignment);
        return;
    }

    // Проверка типа присваивания
    assignment->expression->Accept(this);
    Types exprType = currentExprType;

    if (varSym->dataType != exprType) {
        error("Type mismatch: " + assignment->variable + " (" + TypeToStr(varSym->dataType) + 
                ") = " + TypeToStr(exprType), *assignment);
    }
}
void TypeCheckerVisitor::Visit(Declaration* declaration) {
    // TODO
}
void TypeCheckerVisitor::Visit(PrintStatement* print_statement) {
    // TODO
}
void TypeCheckerVisitor::Visit(ReturnStatement* returnStatement) {
    // TODO
}
void TypeCheckerVisitor::Visit(IfStatement* statement) {
    // TODO
}

void TypeCheckerVisitor::Visit(Expression* expression) {
    // TODO
}
void TypeCheckerVisitor::Visit(Number* expression) {
    // TODO
}
void TypeCheckerVisitor::Visit(Variable* expression) {
    // TODO
}
void TypeCheckerVisitor::Visit(BinaryExpression* expression) {
    // TODO
}
void TypeCheckerVisitor::Visit(Comparison* expression) {
    // TODO
}
void TypeCheckerVisitor::Visit(FunctionCall* funcCall) {
    Symbol* funcSym = findSymbol(funcCall->name);
    if (!funcSym || funcSym->type != SymbolType::Function) {
        error("Undefined function: " + funcCall->name, *funcCall);
        return;
    }

    // Проверка количества аргументов
    if (funcCall->args.size() != funcSym->params.size()) {
        error("Argument count mismatch for " + funcCall->name, *funcCall);
        return;
    }

    // Проверка типов аргументов
    for (size_t i = 0; i < funcCall->args.size(); ++i) {
        funcCall->args[i]->Accept(this);
        if (currentExprType != funcSym->params[i].dataType) {
            error("Argument type mismatch in " + funcCall->name + 
                    ", expected " + TypeToStr(funcSym->params[i].dataType) + 
                    " got " + TypeToStr(currentExprType), *funcCall);
        }
    }
}