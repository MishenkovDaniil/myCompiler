#include <string>

#include "program.hpp"

#include "visitors/print_visitor.hpp"
#include "visitors/interpreter.hpp"
#include "visitors/llvm_codegen_visitor.hpp"
#include "visitors/scope_tree_visitor.hpp"

Program::Program(std::string& program_fn, std::string& ast_fn) :
    program_fn(program_fn), ast_fn(ast_fn)
{
    std::ifstream program_str(program_fn);
    // ast = std::ofstream(ast_fn); 
    std::string code;
    std::string temp;
    while (program_str) {
        std::getline(program_str, temp);
        code += temp + "\n";
    }
    program = std::istringstream(code);
    lexer = new Lexer(program);
    parser = new Parser(*lexer);
}

void Program::Run() {
    programBlocks = parser->parse();
    SymbolTreeVisitor print_visitor(ast_fn);
    Interpreter interpreter{};
    programBlocks->Accept(&print_visitor);
    programBlocks->Accept(&interpreter);
    ScopeTreeVisitor scope_visitor{};
    programBlocks->Accept(&scope_visitor);
    std::string scopeDump ="scope_dump.txt";
    scope_visitor.Dump(scopeDump);
    LLVMCodeGenVisitor llvmVisitor;
    programBlocks->Accept(&llvmVisitor);
    llvmVisitor.generateIR("output.ll");
}