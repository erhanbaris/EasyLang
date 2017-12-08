//https://github.com/catchorg/Catch2
//https://github.com/DeadMG/Wide/blob/master/Wide/Lexer/Lexer.cpp

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cmath>
#define CATCH_CONFIG_RUNNER

#include "Catch.h"
#include "Lexer.h"
#include "ASTs.h"
#include "Backend.h"
#include "Tests/LexerTests.h"
#include "Tests/AstTests.h"

using namespace std;

int main(int argc, char* argv[]) {
	// Unit tests
	Catch::Session().run(argc, argv);

	auto* tokinizer = new StandartTokinizer();
	auto tokens = make_shared<std::vector<Token*>>();
	auto asts = make_shared<std::vector<Ast*>>();

	tokinizer->Parse(L"\"erhan\" çarpı \"aysel\"", tokens);
	/*tokinizer->Parse(L"atama veri evet "
                      "atama erhan'a 15 artı 8 "
                      "eğer erhan büyükise 13 sonra "
                      "başla "
                          "yaz \"büyük\" "
                          "yaz \"block ikinci eleman\" "
                      "bitir "
                      "değilse "
                          "yaz \"küçük\"", tokens);*/
	//tokinizer->Dump(tokens);

    auto* astParser = new AstParser;

    astParser->Parse(tokens, asts);
    astParser->Dump(asts);

	BackendExecuter<InterpreterBackend> executer;
	executer.Prepare(asts);

	std::wstring line;
	std::wcout << "lang > ";
	while (std::getline(std::wcin, line))
	{
		tokinizer->Parse(line, tokens);
		astParser->Parse(tokens, asts);
		executer.Prepare(asts);

		std::wcout << "lang >  ";
	}

    getchar();

    delete tokinizer;
    delete astParser;
    return 0;
}
