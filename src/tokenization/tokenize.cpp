#include "tokenize.hpp"

Token Lexer::readNumber() {
    std::string value;
    while (std::isdigit(currentChar)) {
        value += currentChar;
        advance();
    }
    return {TokenType::Number, value};
}

Token Lexer::readOperator() {
    char first = currentChar;
    advance();

    if (first == '+' ||
        first == '-' ||
        first == '/' ||
        first == '*') {
        return {TokenType::Operator, std::string(1, first)};
    } else {
        // advance();
        if ((first == '=' || first == '!' || first == '<' || first == '>') && currentChar == '=') {
            std::string op = std::string(1, first) + "=";
            advance();
            return {TokenType::Operator, op};
        }
    }

    return {TokenType::Operator, std::string(1, first)};
}

Token Lexer::readIdentifier() {
    std::string value;
    while (std::isalnum(currentChar) || currentChar == '_') {
        value += currentChar;
        advance();
    }

    if (value == "if" ||
        value == "else" ||
        value == "declare" ||
        value == "print" ||
        value == "return" ||
        value == "int" ||
        value == "func") {
        return {TokenType::Keyword, value};
    }

    return {TokenType::Identifier, value};
}

Token Lexer::nextToken(){
    skipWhitespace();

    if (std::isdigit(currentChar)) {
        return readNumber();
    }
    if (std::isalpha(currentChar)) {
        return readIdentifier();
    }
    if (currentChar == '=' ||
        currentChar == '!' ||
        currentChar == '<' ||
        currentChar == '+' ||
        currentChar == '-' ||
        currentChar == '*' ||
        currentChar == '/' ||
        currentChar == '>') {
        return readOperator();
    } 
    
    if (std::string(";:{}(),").find(currentChar) != std::string::npos) {
        char symbol = currentChar;
        advance();
        return {TokenType::Symbol, std::string(1, symbol)};
    }

    if (input.eof()) {
        return {TokenType::EndOfFile, ""};
    }

    std::cerr << "Ошибка: неизвестный символ '" << currentChar << "'\n";
    advance();
    return nextToken();
}