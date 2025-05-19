#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include "parsing/ast.hpp"
#include "parsing/parser.hpp"
#include "tokenization/tokenize.hpp"
#include "program.hpp"

int main(int argc, const char **argv) {
    if (argc != 3) {
        std::cout << "Некорректное количество аргументов: требуется 2 - путь к файлу с исходным кодом и к файлу для дампа ast-дерева.\n";
        return 0;
    }

    std::string progname = argv[1];
    std::string astname = argv[2];
    Program prog(progname, astname);
    prog.Run();
}
