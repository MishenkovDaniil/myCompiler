#pragma once 

#include "visitor.hpp"

#include <fstream>
#include <string>
#include <stdexcept>
#include <stack>
#include <vector>
#include <unordered_map>
#include <memory>

// Типы символов
enum class SymbolType { Variable, Function, Class, Method, Constructor, Array };

struct Symbol {
    SymbolType type;
    std::string name;
    Types dataType; // Тип данных (int, string, или имя класса)
    std::vector<Symbol> params; // Для функций/методов
    size_t size; // Для массивов и классов
    bool isPrivate; // Для полей класса

    Symbol(SymbolType t, const std::string& n, Types type, 
           size_t s = 0, bool priv = false);
};

class Scope {
    std::unordered_map<std::string, Symbol> symbols;
    Scope* parent;

public:
    Scope(Scope* p = nullptr) : parent(p) {}

    void add(const Symbol& sym);

    Symbol* find(const std::string& name);
    friend class ScopeTree;
};

// Дерево областей видимости
class ScopeTree {
    std::unique_ptr<Scope> globalScope;
    std::stack<Scope*> activeScopes;

public:
    ScopeTree();

    // Войти в новый scope (например, при входе в функцию)
    void enterScope();

    // Выйти из текущего scope
    void exitScope();

    Scope* currentScope() const ;
    void dumpToFile(const std::string& filename);

private:
    void dumpScope(Scope* scope, std::ostream& out, int indent);
    std::string symbolToString(const Symbol& sym);
};


class ScopeTreeVisitor: public Visitor {
public:
    ScopeTreeVisitor();
    ~ScopeTreeVisitor();

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
    void Dump(std::string& filename) {
        scopeTree.dumpToFile(filename);
    }
private:
    ScopeTree scopeTree;
    std::stack<Scope*> scopes;
    Types currentExpressionType; 
    Symbol* currentFunction = nullptr;

    void checkVariableDecl(const std::string& name) {
        if (scopes.top()->find(name)) {
            throw std::runtime_error("Повторное объявление переменной: " + name);
        }
    }
};
std::string TypeToStr(Types type);