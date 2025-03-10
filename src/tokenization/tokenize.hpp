#pragma once 
#include <iostream>
#include <cctype>
#include <string>
#include <vector>

enum class TokenType {
    Identifier, 
    Number,
    Keyword,
    Operator,
    Symbol,
    EndOfFile
};

struct Token {
    TokenType type;
    std::string value;
    // Token(TokenType t, std::string v) : type(t), value(std::move(v)) {}
};

class Lexer {
    std::istream& input;
    char currentChar;

    void advance() { 
        currentChar = input.get();
    }

    void skipWhitespace() {
        while (std::isspace(currentChar)) advance();
    }

    Token readNumber();
    Token readIdentifier();
    Token readOperator();

public:
    explicit Lexer(std::istream& in) : input(in), currentChar(' ') { advance(); }

    Token nextToken();
};
