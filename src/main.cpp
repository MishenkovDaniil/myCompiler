#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include "parsing/ast.hpp"
#include "parsing/parser.hpp"
#include "tokenization/tokenize.hpp"

int main() {
    std::string code = R"(
        declare x: int;
        x = 3232 + 5;
        if (x == 3233  - 1  + 3  + 2) {
                print   (x );
        } else {
            x = 42;
        }
    )";

    std::ofstream ast_file;
    ast_file.open("ast.txt");

    std::istringstream input(code);
    Lexer lexer(input);
    Parser parser(lexer);

    auto statements = parser.parse();
    statements->print(ast_file);
    ast_file.close();
    std::map<std::string, int> maaap;
    statements->fastExecute(maaap);
}
