//https://github.com/catchorg/Catch2
//https://github.com/DeadMG/Wide/blob/master/Wide/Lexer/Lexer.cpp

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cmath>
#define CATCH_CONFIG_RUNNER

#include "Catch.h"
#include "System.h"
#include "EasyEngine.h"

#ifdef _DEBUG
#include "Tests/LexerTests.h"
#include "Tests/AstTests.h"
#include "Tests/InterpreterTests.h"
#endif

using namespace std;

int main(int argc, char* argv[]) {
	System::WarmUp();
#ifdef _DEBUG
	// Unit tests
	Catch::Session().run(argc, argv);
#endif

    auto* engine = EasyEngine::Interpreter();

	std::wstring line;
	std::wcout << "EasyLang Interpreter\n\n";
	std::wcout << "easy > ";
	while (std::getline(std::wcin, line))
	{
        try {
			engine->Execute(line);
        } catch (exception& e) {
            std::wcout << "#ERROR " << e.what() << '\n';
        }

		std::wcout << "easy > ";
	}

    getchar();
    return 0;
}
