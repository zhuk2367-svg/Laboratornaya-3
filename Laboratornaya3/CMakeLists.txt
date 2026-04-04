cmake_minimum_required(VERSION 3.10)
project(ExpressionCalculator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SOURCES
    src/utils.cpp
    src/lexer.cpp
    src/ast_nodes.cpp
    src/parser.cpp
    src/evaluator.cpp
    main.cpp
)

add_executable(calculator ${SOURCES})