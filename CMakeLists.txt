cmake_minimum_required(VERSION 3.4)
project(EasyLang)

set(CMAKE_CXX_STANDARD 11)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(EasyLang
        src/main.cpp
        include/Lexer.h
        src/Lexer.cpp
        src/ASTs.cpp
        include/ASTs.h
        Tests/LexerTests.h
        Tests/AstTests.h
        include/Macros.h
        include/Backend.h
        src/PrimativeValue.cpp
        include/PrimativeValue.h
        include/Exceptions.h
        src/InterpreterBackend.cpp
        include/InterpreterBackend.h
        Tests/InterpreterTests.h
        src/lib/iolib.cpp
        include/lib/iolib.h
        src/lib/corelib.cpp
        include/lib/corelib.h
        src/lib/arraylib.cpp
        include/lib/arraylib.h
        src/System.cpp
        include/Scope.h
        src/Scope.cpp
        src/EasyEngine.cpp
        include/EasyEngine.h
        include/Catch.h
		include/Definitions.h
		include/FunctionDispatch.h
        )