#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <vector>

#include "../tokenization/tokenize.hpp"
#include "ast.hpp"

class Parser {
private:
    Lexer& lexer;
    Token currentToken;

    void advance() { 
        currentToken = lexer.nextToken(); 
    }

    std::unique_ptr<StatementList> parseStatementList();
    
    std::unique_ptr<IfStatement> parseIfStatement();
    
    std::unique_ptr<Statement> parseStatement();
    
    std::unique_ptr<Statement> parseDeclaration();
    
    std::unique_ptr<Statement> parsePrintStatement();
    
    std::unique_ptr<Statement> parseAssignment();
    
    std::unique_ptr<Expression> parseExpression();
    
    std::unique_ptr<Expression> parseSubAdd();
    
    std::unique_ptr<Expression> parseTerm();
    
    std::unique_ptr<Expression> parseFactor();
    
    std::unique_ptr<Expression> parsePrimary();


public:
    Parser(Lexer& lex) : lexer(lex) { advance(); }

    std::unique_ptr<StatementList> parse();
};
