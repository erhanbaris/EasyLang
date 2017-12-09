cmake_minimum_required(VERSION 3.4)
project(EasyLang)

set(CMAKE_CXX_STANDARD 11)

add_executable(EasyLang main.cpp Lexer.h Lexer.cpp ASTs.cpp ASTs.h Tests/LexerTests.h Tests/AstTests.h Macros.h Backend.h PrimativeValue.cpp PrimativeValue.h)