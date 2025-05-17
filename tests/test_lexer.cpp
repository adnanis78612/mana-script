#include "lexer.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <string>

using namespace mana;

void test_simple_tokens() {
    std::string source = "var x = 42;";
    Lexer lexer(source);
    
    std::vector<Token> tokens = lexer.scanTokens();
    
    assert(tokens.size() == 6); // var, x, =, 42, ;, EOF
    assert(tokens[0].type == TokenType::VAR);
    assert(tokens[1].type == TokenType::IDENTIFIER);
    assert(tokens[2].type == TokenType::EQUAL);
    assert(tokens[3].type == TokenType::INTEGER_LITERAL);
    assert(tokens[4].type == TokenType::SEMICOLON);
    assert(tokens[5].type == TokenType::END_OF_FILE);
}

void test_operators() {
    std::string source = "+ - * / % == != < <= > >= && ||";
    Lexer lexer(source);
    
    std::vector<Token> tokens = lexer.scanTokens();
    
    assert(tokens.size() == 13); // 12 operators + EOF
    assert(tokens[0].type == TokenType::PLUS);
    assert(tokens[1].type == TokenType::MINUS);
    assert(tokens[2].type == TokenType::STAR);
    assert(tokens[3].type == TokenType::SLASH);
    assert(tokens[4].type == TokenType::PERCENT);
    assert(tokens[5].type == TokenType::EQUAL_EQUAL);
    assert(tokens[6].type == TokenType::BANG_EQUAL);
    assert(tokens[7].type == TokenType::LESS);
    assert(tokens[8].type == TokenType::LESS_EQUAL);
    assert(tokens[9].type == TokenType::GREATER);
    assert(tokens[10].type == TokenType::GREATER_EQUAL);
    assert(tokens[11].type == TokenType::AND);
    assert(tokens[12].type == TokenType::OR);
}

void test_keywords() {
    std::string source = "function var const if else while for return break continue true false nil";
    Lexer lexer(source);
    
    std::vector<Token> tokens = lexer.scanTokens();
    
    assert(tokens.size() == 14); // 13 keywords + EOF
    assert(tokens[0].type == TokenType::FUNCTION);
    assert(tokens[1].type == TokenType::VAR);
    assert(tokens[2].type == TokenType::CONST);
    assert(tokens[3].type == TokenType::IF);
    assert(tokens[4].type == TokenType::ELSE);
    assert(tokens[5].type == TokenType::WHILE);
    assert(tokens[6].type == TokenType::FOR);
    assert(tokens[7].type == TokenType::RETURN);
    assert(tokens[8].type == TokenType::BREAK);
    assert(tokens[9].type == TokenType::CONTINUE);
    assert(tokens[10].type == TokenType::TRUE);
    assert(tokens[11].type == TokenType::FALSE);
    assert(tokens[12].type == TokenType::NIL);
}

void test_literals() {
    std::string source = "42 3.14 \"hello\" true false";
    Lexer lexer(source);
    
    std::vector<Token> tokens = lexer.scanTokens();
    
    assert(tokens.size() == 6); // 5 literals + EOF
    assert(tokens[0].type == TokenType::INTEGER_LITERAL);
    assert(tokens[1].type == TokenType::FLOAT_LITERAL);
    assert(tokens[2].type == TokenType::STRING_LITERAL);
    assert(tokens[3].type == TokenType::BOOL_LITERAL);
    assert(tokens[4].type == TokenType::BOOL_LITERAL);
}

void test_comments() {
    std::string source = "// This is a line comment\nvar x = 42; /* This is a block comment */";
    Lexer lexer(source);
    
    std::vector<Token> tokens = lexer.scanTokens();
    
    assert(tokens.size() == 6); // var, x, =, 42, ;, EOF
    assert(tokens[0].type == TokenType::VAR);
    assert(tokens[1].type == TokenType::IDENTIFIER);
    assert(tokens[2].type == TokenType::EQUAL);
    assert(tokens[3].type == TokenType::INTEGER_LITERAL);
    assert(tokens[4].type == TokenType::SEMICOLON);
}

void test_error_handling() {
    std::string source = "var x = @;";
    Lexer lexer(source);
    
    std::vector<Token> tokens = lexer.scanTokens();
    
    // Should have an ERROR token
    bool has_error = false;
    for (const auto& token : tokens) {
        if (token.type == TokenType::ERROR) {
            has_error = true;
            break;
        }
    }
    
    assert(has_error);
    assert(diagnostics.hasErrors());
}

int main() {
    test_simple_tokens();
    test_operators();
    test_keywords();
    test_literals();
    test_comments();
    test_error_handling();
    
    std::cout << "All lexer tests passed!\n";
    return 0;
}