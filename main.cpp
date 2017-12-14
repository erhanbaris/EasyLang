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
#include "iolib.h"
#include "Lexer.h"
#include "ASTs.h"
#include "Backend.h"
#include "InterpreterBackend.h"

#include "Tests/LexerTests.h"
#include "Tests/AstTests.h"
#include "Tests/InterpreterTests.h"

using namespace std;

int main(int argc, char* argv[]) {
	System::WarmUp();
	// Unit tests
	Catch::Session().run(argc, argv);

	auto* tokinizer = new StandartTokinizer();
	auto tokens = make_shared<std::vector<Token*>>();
	auto asts = make_shared<std::vector<Ast*>>();

	//tokinizer->Parse(L"4 eşit 4", tokens);
	//tokinizer->Parse(L"yaz(1,2,3,4) ", tokens);
	//tokinizer->Dump(tokens);

    auto* astParser = new AstParser;

    //astParser->Parse(tokens, asts);
    //astParser->Dump(asts);

	BackendExecuter<InterpreterBackend> executer;
	//executer.Prepare(asts);

	std::wstring line;
	std::wcout << "easy > ";
	while (std::getline(std::wcin, line))
	{
        try {
            tokinizer->Parse(line, tokens);
            astParser->Parse(tokens, asts);
            executer.Prepare(asts);
        } catch (exception& e) {
            std::wcout << "#ERROR " << e.what() << '\n';
        }

		std::wcout << "easy >  ";
	}

    getchar();

    delete tokinizer;
    delete astParser;
    return 0;
}
