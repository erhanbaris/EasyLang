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

#include "../Tests/LexerTests.h"
#include "../Tests/AstTests.h"
#include "../Tests/InterpreterTests.h"

using namespace std;

int main(int argc, char* argv[]) {
	System::WarmUp();
	// Unit tests
	Catch::Session().run(argc, argv);

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
