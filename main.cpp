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
#include "Tests/LexerTests.h"
#include "Tests/AstTests.h"

using namespace std;

void ekranaYaz(std::wstring const & message)
{
    std::wcout << message << std::endl;
}

int main( int argc, char* argv[] ) {
    // Unit tests
	Catch::Session().run( argc, argv );

	auto* tokinizer = new StandartTokinizer();
	auto tokens = make_shared<std::vector<Token*>>();
    auto asts = make_shared<std::vector<Ast*>>();
	tokinizer->Parse(L"atama erhan'a 15 eğer erhan büyükise 13 sonra yaz \"büyük\" değilse yaz \"küçük\"", tokens);
	tokinizer->Dump(tokens);

    auto* astParser = new AstParser;
    astParser->AddMethod(L"yaz", &ekranaYaz);

    astParser->Parse(tokens, asts);
    astParser->Dump(asts);

    getchar();

    delete tokinizer;
    delete astParser;
    return 0;
}