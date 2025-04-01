#pragma once 
#include <iostream>
#include <fstream>
#include <string>
#include "parsing/ast.hpp"
#include "parsing/parser.hpp"
#include "tokenization/tokenize.hpp"
#include <sstream>

class Program {
public:
    Program(std::string& program_fn, std::string& ast_fn);
    void Run();
private:
    Lexer *lexer = nullptr; 
    Parser *parser = nullptr;
    std::istringstream program;
    std::string& program_fn;
    std::ofstream ast;
    std::string& ast_fn;
    std::unique_ptr<StatementList> statements;
    std::map<std::string, int> variables;
};