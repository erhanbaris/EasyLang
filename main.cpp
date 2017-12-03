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

using namespace std;


int main( int argc, char* argv[] ) {
    Catch::Session().run( argc, argv );

    /* eğer 127 > 80 sonra yaz büyük değilse yaz küçük */
    Tokinizer* parser = new StandartTokinizer();
    auto Tokens = make_shared<std::vector<Token*>>();
    parser->Parse(L"erhan'a 15 ata; eger erhan buyukise 13 sonra ekranayaz \"buyuk\" degilse ekranayaz \"kucuk\"", Tokens);

    AstParser* astParser = new AstParser;
    astParser->Parse(Tokens);


    auto TokenEnd = Tokens->cend();

    for (auto it = Tokens->cbegin(); it != TokenEnd; ++it)
    {
        switch ((*it)->GetType())
        {
            case TokenType::DOUBLE:
                std::wcout << L"DOUBLE : " << reinterpret_cast<DoubleToken*>(*it)->Value << std::endl;
                break;

            case TokenType::INTEGER:
                std::wcout << L"INTEGER : " << reinterpret_cast<IntegerToken*>(*it)->Value << std::endl;
                break;

            case TokenType::OPERATOR:
                std::wcout << L"OPERATOR : " << (int)(reinterpret_cast<OperatorToken*>(*it)->Value) << std::endl;
                break;

            case TokenType::SYMBOL:
                std::wcout << L"SYMBOL : " << reinterpret_cast<SymbolToken*>(*it)->Value << std::endl;
                break;

            case TokenType::TEXT:
                std::wcout << L"TEXT : " << reinterpret_cast<TextToken*>(*it)->Value << std::endl;
                break;

            case TokenType::VARIABLE:
                std::wcout << L"VARIABLE : " << reinterpret_cast<VariableToken*>(*it)->Value << std::endl;
                break;

            case TokenType::NONE:
                break;
        }
    }

    getchar();
    return 0;
}