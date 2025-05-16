#include "lexer.hpp"
#include "parser.hpp"
#include "ast.hpp"
#include "codegen.hpp"
#include "transpiler.hpp"
#include "jit.hpp"
#include "error.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <memory>
#include <vector>
#include <filesystem>

void printUsage() {
    std::cout << "Usage: manascript [options] <file>\n";
    std::cout << "Options:\n";
    std::cout << "  --help         Show this help message\n";
    std::cout << "  --emit-llvm    Emit LLVM IR\n";
    std::cout << "  --emit-cpp     Emit C++ code\n";
    std::cout << "  --dump-ast     Dump AST\n";
    std::cout << "  --dump-tokens  Dump tokens\n";
    std::cout << "  --jit          JIT compile and run\n";
    std::cout << "  -o <file>      Output file\n";
}

std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file: " << path << std::endl;
        exit(1);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void writeFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << path << std::endl;
        exit(1);
    }
    
    file << content;
}

void dumpTokens(const std::vector<mana::Token>& tokens) {
    std::cout << "Tokens:\n";
    for (const auto& token : tokens) {
        std::cout << token << "\n";
    }
    std::cout << std::endl;
}

void dumpAST(const std::vector<mana::StmtPtr>& statements) {
    // This is a placeholder for AST dumping functionality
    std::cout << "AST Dump (not implemented yet)\n";
    std::cout << "Number of statements: " << statements.size() << std::endl;
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    std::string input_file;
    std::string output_file;
    bool emit_llvm = false;
    bool emit_cpp = false;
    bool dump_ast = false;
    bool dump_tokens = false;
    bool jit_compile = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printUsage();
            return 0;
        }
        else if (arg == "--emit-llvm") {
            emit_llvm = true;
        }
        else if (arg == "--emit-cpp") {
            emit_cpp = true;
        }
        else if (arg == "--dump-ast") {
            dump_ast = true;
        }
        else if (arg == "--dump-tokens") {
            dump_tokens = true;
        }
        else if (arg == "--jit") {
            jit_compile = true;
        }
        else if (arg == "-o" && i + 1 < argc) {
            output_file = argv[++i];
        }
        else if (arg[0] == '-') {
            std::cerr << "Unknown option: " << arg << std::endl;
            printUsage();
            return 1;
        }
        else {
            input_file = arg;
        }
    }
    
    if (input_file.empty()) {
        std::cerr << "Error: No input file specified\n";
        printUsage();
        return 1;
    }
    
    // Read input file
    std::string source = readFile(input_file);
    
    // Initialize lexer
    mana::Lexer lexer(source, input_file);
    
    // Scan tokens
    auto tokens = lexer.scanTokens();
    
    // Check for lexical errors
    if (mana::diagnostics.hasErrors()) {
        std::cerr << "Lexical errors:\n";
        mana::diagnostics.printDiagnostics();
        return 1;
    }
    
    // Dump tokens if requested
    if (dump_tokens) {
        dumpTokens(tokens);
    }
    
    // Initialize parser
    mana::Parser parser(tokens, input_file);
    
    // Parse tokens
    auto statements = parser.parse();
    
    // Check for syntax errors
    if (mana::diagnostics.hasErrors()) {
        std::cerr << "Syntax errors:\n";
        mana::diagnostics.printDiagnostics();
        return 1;
    }
    
    // Dump AST if requested
    if (dump_ast) {
        dumpAST(statements);
    }
    
    // Emit C++ code if requested
    if (emit_cpp) {
        mana::Transpiler transpiler;
        std::string cpp_code = transpiler.transpile(statements);
        
        if (!output_file.empty()) {
            writeFile(output_file, cpp_code);
        } else {
            std::filesystem::path input_path(input_file);
            std::string cpp_file = input_path.stem().string() + ".cpp";
            writeFile(cpp_file, cpp_code);
            std::cout << "C++ code written to " << cpp_file << std::endl;
        }
    }
    
    // Emit LLVM IR or JIT compile
    if (emit_llvm || jit_compile) {
        // Initialize code generator
        mana::CodeGenerator codegen;
        codegen.initialize(std::filesystem::path(input_file).stem().string());
        
        // Generate LLVM IR
        codegen.generate(statements);
        
        // Check for code generation errors
        if (mana::diagnostics.hasErrors()) {
            std::cerr << "Code generation errors:\n";
            mana::diagnostics.printDiagnostics();
            return 1;
        }
        
        // Emit LLVM IR if requested
        if (emit_llvm) {
            std::string ir_code = codegen.dumpIR();
            
            if (!output_file.empty()) {
                writeFile(output_file, ir_code);
            } else {
                std::filesystem::path input_path(input_file);
                std::string ir_file = input_path.stem().string() + ".ll";
                writeFile(ir_file, ir_code);
                std::cout << "LLVM IR written to " << ir_file << std::endl;
            }
        }
        
        // JIT compile and run if requested
        if (jit_compile) {
            std::cout << "JIT compiling and running...\n";
            
            // Create JIT
            auto jit = mana::JIT::create();
            if (!jit) {
                std::cerr << "Failed to create JIT\n";
                return 1;
            }
            
            // Add the module to the JIT
            auto module = std::unique_ptr<llvm::Module>(codegen.getModule());
            auto err = (*jit)->addModule(std::move(module));
            if (err) {
                std::cerr << "Failed to add module to JIT\n";
                return 1;
            }
            
            // Look up the main function
            auto main_symbol = (*jit)->lookup("main");
            if (!main_symbol) {
                std::cerr << "Failed to find main function\n";
                return 1;
            }
            
            // Cast the symbol to a function pointer and call it
            auto main_fn = reinterpret_cast<int(*)()>(main_symbol->getAddress());
            int result = main_fn();
            
            std::cout << "Program exited with code " << result << std::endl;
        }
    }
    
    return 0;
}