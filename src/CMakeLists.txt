cmake_minimum_required(VERSION 3.21)

set(SOURCES
        main.cpp
        lex.cpp
        parser.cpp
        interpreter.cpp
        lox_function.cpp
)
    
add_executable(lox ${SOURCES})