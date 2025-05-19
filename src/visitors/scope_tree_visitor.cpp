#include "scope_tree_visitor.hpp"
#include "../parsing/ast.hpp"

Symbol::Symbol(SymbolType t, const std::string& n, Types type, size_t s, bool priv)
        : type(t), name(n), dataType(type), size(s), isPrivate(priv) {}

void Scope::add(const Symbol& sym) {
    if (symbols.find(sym.name) != symbols.end()) {
        throw std::runtime_error("Переменная '" + sym.name + "' уже объявлена");
    }
    symbols.emplace(sym.name, sym);
}

// Найти символ в текущем или родительских scope
Symbol* Scope::find(const std::string& name) {
    auto it = symbols.find(name);
    if (it != symbols.end()) return &it->second;
    return parent ? parent->find(name) : nullptr;
}

ScopeTree::ScopeTree() {
    globalScope = std::make_unique<Scope>();
    activeScopes.push(globalScope.get());
}

void ScopeTree::enterScope() {
    activeScopes.push(new Scope(activeScopes.top()));
}

void ScopeTree::exitScope() {
    Scope* old = activeScopes.top();
    activeScopes.pop();
    delete old; 
}

Scope* ScopeTree::currentScope() const {
    return activeScopes.top();
}
void ScopeTree::dumpToFile(const std::string& filename) {
    std::ofstream out(filename);
    dumpScope(globalScope.get(), out, 0);
}

void ScopeTree::dumpScope(Scope* scope, std::ostream& out, int indent) {
    std::string indentStr(indent * 4, ' ');
    
    for (const auto& [name, sym] : scope->symbols) {
        out << indentStr << symbolToString(sym) << "\n";
    }

    if (scope->parent) {
        dumpScope(scope->parent, out, indent + 1);
    }
}

std::string ScopeTree::symbolToString(const Symbol& sym) {
    std::string typeStr;
    switch(sym.type) {
        case SymbolType::Variable: {
            typeStr = "Variable"; 
            break;
        }
        case SymbolType::Function: {
            typeStr = "Function"; 
            break;
        }
        case SymbolType::Class: {
            typeStr = "Class"; 
            break;
        }
        case SymbolType::Method: {
            typeStr = "Method"; 
            break;
        }
        case SymbolType::Constructor: {
            typeStr = "Constructor"; 
            break;
        }
        case SymbolType::Array: {
            typeStr = "Array"; 
            break;
        }
        default: {
            typeStr = "<Unknown SymbolType>"; 
        }
    }
    return typeStr + " " + sym.name + " : " + TypeToStr(sym.dataType);
}


ScopeTreeVisitor::ScopeTreeVisitor() {
    scopes.push(scopeTree.currentScope());
}

ScopeTreeVisitor::~ScopeTreeVisitor() {}

void ScopeTreeVisitor::Visit(ASTNode* node) {
    // cannot be called
}

void ScopeTreeVisitor::Visit(std::string& program) {
    // cannot be called
}

void ScopeTreeVisitor::Visit(ProgramBlocks* programBlocks) {
    for (auto&& block : programBlocks->blocks) {
        block->Accept(this);
    }
}

void ScopeTreeVisitor::Visit(ProgramBlock* programBlock) {
    // cannot be called
}


void ScopeTreeVisitor::Visit(FunctionDeclaration* funcDecl) {
    Symbol funcSym(SymbolType::Function, funcDecl->name, funcDecl->returnType->type);
    for (auto& param : funcDecl->params) {
        Symbol paramSym(SymbolType::Variable, param->name, param->type->type);
        funcSym.params.push_back(paramSym);
    }
    scopes.top()->add(funcSym);

    scopeTree.enterScope();
    scopes.push(scopeTree.currentScope());

    for (auto& param : funcDecl->params) {
        Symbol paramSym(SymbolType::Variable, param->name, param->type->type);
        scopes.top()->add(paramSym);
    }

    currentFunction = &funcSym;
    funcDecl->body->Accept(this);
    currentFunction = nullptr;

    scopes.pop();
    scopeTree.exitScope();
}
void ScopeTreeVisitor::Visit(Parameter* parameter) {
    // cannot be called
}
void ScopeTreeVisitor::Visit(Type* type) {
    // cannot be called
}

void ScopeTreeVisitor::Visit(StatementList* statementList) {
    for (auto&& statement : statementList->statements) {
        statement->Accept(this);
    }
}

void ScopeTreeVisitor::Visit(Statement* statement) {
    // cannot be called
}
void ScopeTreeVisitor::Visit(Assignment* assignment) {
    Symbol* varSym = scopes.top()->find(assignment->variable);
    if (!varSym) {
        throw std::runtime_error("Необъявленная переменная: " + assignment->variable);
    }

    assignment->expression->Accept(this);
    Types exprType = currentExpressionType;

    // Проверка совместимости типов
    if (varSym->dataType != exprType) {
        throw std::runtime_error("Несоответствие типов: " + assignment->variable + " (" + TypeToStr(varSym->dataType) + 
                                ") не может быть присвоен " + TypeToStr(exprType));
    }

}
void ScopeTreeVisitor::Visit(Declaration* declaration) {
    checkVariableDecl(declaration->varName);
    Symbol sym(SymbolType::Variable, declaration->varName, declaration->varType->type);
    scopes.top()->add(sym);
}
void ScopeTreeVisitor::Visit(PrintStatement* print_statement) {
    print_statement->expression->Accept(this);
}
void ScopeTreeVisitor::Visit(ReturnStatement* returnStatement) {
    Symbol* currentFunc = currentFunction; // currentFunction должен храниться в Visitor
    
    if (!returnStatement->expression && currentFunc->dataType != Types::INT) {
        throw std::runtime_error("Необъявленная 'void' функция должна возвращать значение");
    }
    
    if (returnStatement->expression) {
        returnStatement->expression->Accept(this);
        if (currentExpressionType != currentFunc->dataType) {
            throw std::runtime_error("Несоответствие типов в 'return'. Ожидалось: " + 
                                   TypeToStr(currentFunc->dataType));
        }
    }
}
void ScopeTreeVisitor::Visit(IfStatement* statement) {
    // Проверка типа условия (должно быть boolean или int)
    statement->condition->Accept(this);
    Types condType = currentExpressionType;
    
    if (condType != Types::INT) { // no boolean yet
        throw std::runtime_error("Условие должно быть типа integer(boolean)");
    }

    scopeTree.enterScope();
    statement->thenBranch->Accept(this);
    scopeTree.exitScope();

    if (statement->elseBranch) {
        scopeTree.enterScope();
        statement->elseBranch->Accept(this);
        scopeTree.exitScope();
    }
}

void ScopeTreeVisitor::Visit(Expression* expression) {
    // cannot be called
}
void ScopeTreeVisitor::Visit(Number* expression) {
    currentExpressionType = Types::INT;
}
void ScopeTreeVisitor::Visit(Variable* var) {
    Symbol* sym = scopes.top()->find(var->name);
    if (!sym) {
        throw std::runtime_error("Необъявленная переменная: " + var->name);
    }
    currentExpressionType = sym->dataType;
}
void ScopeTreeVisitor::Visit(BinaryExpression* expression) {
    expression->left->Accept(this);
    Types leftType = currentExpressionType;
    
    expression->right->Accept(this);
    Types rightType = currentExpressionType;
    
    // Проверка совместимости типов для операции
    if (leftType != rightType) {
        throw std::runtime_error("Несоответствие типов в бинарной операции: " + TypeToStr(leftType) + " и " + TypeToStr(rightType));
    }
        
    currentExpressionType = leftType; 
}
void ScopeTreeVisitor::Visit(Comparison* expression) {
    expression->left->Accept(this);
    Types leftType = currentExpressionType;
    
    expression->right->Accept(this);
    Types rightType = currentExpressionType;
    
    // Проверка совместимости типов для операции
    if (leftType != rightType) {
        throw std::runtime_error("Несоответствие типов в операции сравнения: " + TypeToStr(leftType) + " и " + TypeToStr(rightType));
    }
        
    currentExpressionType = Types::INT; // no boolean yet
}
void ScopeTreeVisitor::Visit(FunctionCall* funcCall) {
    // Поиск функции в scope
    Symbol* funcSym = scopes.top()->find(funcCall->name);
    if (!funcSym || funcSym->type != SymbolType::Function) {
        throw std::runtime_error("Необъявленная функция: " + funcCall->name);
    }

    // Проверка количества аргументов
    if (funcCall->args.size() != funcSym->params.size()) {
        char args_size[10] = "";
        char params_size[10] = "";
        sprintf(args_size, "%ld", funcCall->args.size());
        sprintf(params_size, "%ld", funcSym->params.size());
        throw std::runtime_error("Несоответствие количества аргументов " + funcCall->name + ", вызвана с " + args_size + "аргументами, ожидалось " + params_size);
    }

    // Проверка типов аргументов
    for (size_t i = 0; i < funcCall->args.size(); ++i) {
        funcCall->args[i]->Accept(this);
        if (currentExpressionType != funcSym->params[i].dataType) {
            throw std::runtime_error("Несоответсвие типа аргумента " + std::to_string(i+1) + 
                                   " в функции " + funcCall->name + ". Ожидалось: " + 
                                   TypeToStr(funcSym->params[i].dataType));
        }
    }

    currentExpressionType = funcSym->dataType; 
}

std::string TypeToStr(Types type) {
    switch (type) {
        case Types::INT: return "int";
        case Types::CHAR: return "char";
        case Types::FLOAT: return "float";
        default: return "<unknown type>";
    }
}
