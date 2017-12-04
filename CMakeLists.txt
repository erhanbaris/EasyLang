cmake_minimum_required(VERSION 3.6)
project(EasyLang)

set(CMAKE_CXX_STANDARD 11)

add_executable(EasyLang main.cpp Lexer.cpp ASTs.cpp ASTs.h Tests/LexerTests.h Macros.h)