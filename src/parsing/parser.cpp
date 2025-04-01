#include "parser.hpp"
#include <iostream>

std::unique_ptr<StatementList> Parser::parse() {
    return parseStatementList();
}

std::unique_ptr<StatementList> Parser::parseStatementList() {
    auto statements = std::make_unique<StatementList>();
    
    while (currentToken.value != "}" && currentToken.type != TokenType::EndOfFile) {
        statements->statements.push_back(parseStatement());
    }

    return statements;
}

std::unique_ptr<Statement> Parser::parseStatement() {
    if (currentToken.value == "declare") {
        return parseDeclaration();
    }
    if (currentToken.value == "if") {
        return parseIfStatement();
    }
    if (currentToken.value == "print") {
        return parsePrintStatement();
    }
    if (currentToken.type == TokenType::Identifier) {
        return parseAssignment();
    }

    throw std::runtime_error("Ожидался оператор");
}

std::unique_ptr<Statement> Parser::parseAssignment() {
    std::string varName = currentToken.value;
    advance();
    if (currentToken.value != "=") throw std::runtime_error("Ожидался знак '='");
    advance();
    auto expr = parseExpression();
    if (currentToken.value != ";") throw std::runtime_error("Ожидалась ';'");
    advance();
    return std::make_unique<Assignment>(varName, std::move(expr));
}

std::unique_ptr<Statement> Parser::parsePrintStatement() {
    advance();
    if (currentToken.value != "(") throw std::runtime_error("Ожидалась '('");
    advance();
    auto var = parseExpression();
    if (currentToken.value != ")") throw std::runtime_error("Ожидалась ')'");
    advance();
    if (currentToken.value != ";") throw std::runtime_error("Ожидалась ';'");
    advance();
    return std::make_unique<PrintStatement>(std::move(var));
}

std::unique_ptr<Statement> Parser::parseDeclaration() {
    advance(); // Пропускаем "declare"
    
    if (currentToken.type != TokenType::Identifier)
        throw std::runtime_error("Ожидался идентификатор после 'declare'");

    std::string varName = currentToken.value;
    advance();

    if (currentToken.value != ":")
        throw std::runtime_error("Ожидался ':' после имени переменной");

    advance();

    if (currentToken.value != "int") // пока поддерживаем только int
        throw std::runtime_error("Ожидался тип 'int' после ':'");

    advance();

    if (currentToken.value != ";")
        throw std::runtime_error("Ожидалась ';' после объявления переменной");

    advance();
    
    return std::make_unique<Declaration>(varName);
}

std::unique_ptr<IfStatement> Parser::parseIfStatement() {
    advance(); // Пропускаем "if"

    if (currentToken.value != "(") throw std::runtime_error("Ожидалась '(' после if");
    advance();
    
    auto condition = parseExpression(); // Условие
    if (currentToken.value != ")") throw std::runtime_error("Ожидалась ')' после условия");
    advance();

    if (currentToken.value != "{") throw std::runtime_error("Ожидалась '{' перед блоком if");
    advance();

    auto thenBody = parseStatementList(); // Тело if
    if (currentToken.value != "}") throw std::runtime_error("Ожидалась '}' после блока if");
    advance();

    std::unique_ptr<StatementList> elseBody = nullptr;
    if (currentToken.value == "else") {
        advance();
        if (currentToken.value != "{") throw std::runtime_error("Ожидалась '{' перед блоком else");
        advance();
        
        elseBody = parseStatementList(); // Тело else
        if (currentToken.value != "}") throw std::runtime_error("Ожидалась '}' после блока else");
        advance();
    }

    return std::make_unique<IfStatement>(std::move(condition), std::move(thenBody), std::move(elseBody));
}

std::unique_ptr<Expression> Parser::parseExpression() {
    auto left = parseSubAdd();

    while ( currentToken.value == "==" || 
            currentToken.value == ">=" || 
            currentToken.value == "!=" || 
            currentToken.value == "<" || 
            currentToken.value == ">" || 
            currentToken.value == "<=") {
        std::string op = currentToken.value;
        advance();
        auto right = parseSubAdd();
        left = std::make_unique<Comparison>(op, std::move(left), std::move(right));
    }

    return left;
}

std::unique_ptr<Expression> Parser::parseSubAdd() {
    auto left = parseTerm();

    while (currentToken.value == "+" || currentToken.value == "-") {
        std::string op = currentToken.value;
        advance();
        auto right = parseTerm();
        left = std::make_unique<BinaryExpression>(op, std::move(left), std::move(right));
    }

    return left;
}

std::unique_ptr<Expression> Parser::parseTerm() {
    auto left = parseFactor();

    while (currentToken.value == "*" || currentToken.value == "/") {
        std::string op = currentToken.value;
        advance();
        auto right = parseFactor();
        left = std::make_unique<BinaryExpression>(op, std::move(left), std::move(right));
    }

    return left;
}

std::unique_ptr<Expression> Parser::parseFactor() {
    if (currentToken.type == TokenType::Number) {
        int value = std::stoi(currentToken.value);
        advance();
        return std::make_unique<Number>(value);
    }

    if (currentToken.type == TokenType::Identifier) {
        std::string name = currentToken.value;
        advance();
        return std::make_unique<Variable>(name);
    }

    if (currentToken.value == "(") {
        advance();
        auto expr = parseExpression();
        if (currentToken.value != ")") throw std::runtime_error("Ожидалась ')'");
        advance();
        return expr;
    }

    throw std::runtime_error("Ожидалось выражение");
}

std::unique_ptr<Expression> Parser::parsePrimary() {
    if (currentToken.type == TokenType::Number) {
        int value = std::stoi(currentToken.value);
        advance();
        return std::make_unique<Number>(value);
    }
    if (currentToken.type == TokenType::Identifier) {
        std::string name = currentToken.value;
        advance();
        return std::make_unique<Variable>(name);
    }
    throw std::runtime_error("Ожидалось число или переменная");
}