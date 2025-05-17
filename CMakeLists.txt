cmake_minimum_required(VERSION 3.10)
project(Manascript VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find LLVM package
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Include LLVM directories
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Add include directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/lexer.cpp
    src/parser.cpp
    src/ast.cpp
    src/transpiler.cpp
    src/codegen.cpp
    src/error.cpp
    src/jit.cpp
    src/symbol_table.cpp
)

# Main executable
add_executable(manascript ${SOURCES})

# Link against LLVM libraries
llvm_map_components_to_libnames(LLVM_LIBS support core irreader executionengine mcjit x86codegen)
target_link_libraries(manascript ${LLVM_LIBS})

# Add tests
enable_testing()
add_subdirectory(tests)

# Install
install(TARGETS manascript DESTINATION bin)