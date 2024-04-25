cmake_minimum_required(VERSION 3.21)

set(SOURCES
        main.cpp
        lex.cpp
        parser.cpp
)
    
add_executable(yard ${SOURCES})