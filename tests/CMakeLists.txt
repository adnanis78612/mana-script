# Add tests
add_executable(test_lexer test_lexer.cpp)
target_link_libraries(test_lexer manascript_lib)

# Add tests to CTest
add_test(NAME LexerTest COMMAND test_lexer)