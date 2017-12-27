#ifndef EASYLANG_LEXERTESTS_H
#define EASYLANG_LEXERTESTS_H

#define CATCH_CONFIG_RUNNER
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cmath>

#include "../include/Catch.h"
#include "../include/Lexer.h"

using namespace std;

TEST_CASE( "Double test" ) {
    Tokinizer* parser = new StandartTokinizer();
    auto tokens = make_shared<std::vector<Token*>>();
    parser->Parse(_T(".1"), tokens);
    REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::DOUBLE);
    REQUIRE(reinterpret_cast<DoubleToken*>(tokens->at(0))->Value == 0.1);
    
    parser->Parse(_T("1.1"), tokens);
    REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::DOUBLE);
    REQUIRE(reinterpret_cast<DoubleToken*>(tokens->at(0))->Value == 1.1);
    
    parser->Parse(_T("1990.0001"), tokens);
    REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::DOUBLE);
    REQUIRE(reinterpret_cast<DoubleToken*>(tokens->at(0))->Value == 1990.0001);
    
    parser->Parse(_T("-10.1"), tokens);

    REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::OPERATOR);
    REQUIRE(reinterpret_cast<OperatorToken*>(tokens->at(0))->Value == EASY_OPERATOR_TYPE::MINUS);

    REQUIRE(tokens->at(1)->GetType() == EASY_TOKEN_TYPE::DOUBLE);
    REQUIRE(reinterpret_cast<DoubleToken*>(tokens->at(1))->Value == 10.1);
    
    parser->Parse(_T("-.1"), tokens);
    REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::OPERATOR);
    REQUIRE(reinterpret_cast<OperatorToken*>(tokens->at(0))->Value == EASY_OPERATOR_TYPE::MINUS);

    REQUIRE(tokens->at(1)->GetType() == EASY_TOKEN_TYPE::DOUBLE);
    REQUIRE(reinterpret_cast<DoubleToken*>(tokens->at(1))->Value == 0.1);
}

TEST_CASE( "Integer test" ) {
    Tokinizer* parser = new StandartTokinizer();
    auto tokens = make_shared<std::vector<Token*>>();
    parser->Parse(_T("1"), tokens);
    REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::INTEGER);
    REQUIRE(reinterpret_cast<IntegerToken*>(tokens->at(0))->Value == 1);
    
    parser->Parse(_T("12345"), tokens);
    REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::INTEGER);
    REQUIRE(reinterpret_cast<IntegerToken*>(tokens->at(0))->Value == 12345);
    
    parser->Parse(_T("-12345"), tokens);
    REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::OPERATOR);
    REQUIRE(reinterpret_cast<OperatorToken*>(tokens->at(0))->Value == EASY_OPERATOR_TYPE::MINUS);

    REQUIRE(tokens->at(1)->GetType() == EASY_TOKEN_TYPE::INTEGER);
    REQUIRE(reinterpret_cast<IntegerToken*>(tokens->at(1))->Value == 12345);
}

TEST_CASE( "String test" ) {
    Tokinizer* parser = new StandartTokinizer();
    auto tokens = make_shared<std::vector<Token*>>();
    parser->Parse(_T("\"erhan\""), tokens);
    REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::TEXT);
    REQUIRE(reinterpret_cast<TextToken*>(tokens->at(0))->Value == _T("erhan"));
    
    parser->Parse(_T("\"'erhan'\""), tokens);
    REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::TEXT);
    REQUIRE(reinterpret_cast<TextToken*>(tokens->at(0))->Value == _T("'erhan'"));
    
    parser->Parse(_T("\"\\\"erhan\\\"\""), tokens);
    REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::TEXT);
    REQUIRE(reinterpret_cast<TextToken*>(tokens->at(0))->Value == _T("\"erhan\""));
    
    SECTION( "Sembo_T(" ) {
        parser->Parse(_T("17 <= 40"), tokens);
        REQUIRE(tokens->size() == 3);
        REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::INTEGER);
        REQUIRE(reinterpret_cast<IntegerToken*>(tokens->at(0))->Value == 17);
        
        REQUIRE(tokens->at(1)->GetType() == EASY_TOKEN_TYPE::OPERATOR);
        REQUIRE(reinterpret_cast<OperatorToken*>(tokens->at(1))->Value == EASY_OPERATOR_TYPE::LOWER_EQUAL);
        
        REQUIRE(tokens->at(2)->GetType() == EASY_TOKEN_TYPE::INTEGER);
        REQUIRE(reinterpret_cast<IntegerToken*>(tokens->at(2))->Value == 40);
    }
    
    SECTION( "17 > 40" ) {
        parser->Parse(_T("17 > 40"), tokens);
        REQUIRE(tokens->size() == 3);
        REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::INTEGER);
        REQUIRE(reinterpret_cast<IntegerToken*>(tokens->at(0))->Value == 17);
        
        REQUIRE(tokens->at(1)->GetType() == EASY_TOKEN_TYPE::OPERATOR);
        REQUIRE(reinterpret_cast<OperatorToken*>(tokens->at(1))->Value == EASY_OPERATOR_TYPE::GREATOR);
        
        REQUIRE(tokens->at(2)->GetType() == EASY_TOKEN_TYPE::INTEGER);
        REQUIRE(reinterpret_cast<IntegerToken*>(tokens->at(2))->Value == 40);
    }
}

TEST_CASE( "Operator lexer test" ) {
    Tokinizer* parser = new StandartTokinizer();
    auto tokens = make_shared<std::vector<Token*>>();
    
    SECTION( "10 / 10" ) {
        parser->Parse(_T("10 / 10"), tokens);
        REQUIRE(tokens->size() == 3);
        REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::INTEGER);
        REQUIRE(reinterpret_cast<IntegerToken*>(tokens->at(0))->Value == 10);
        
        REQUIRE(tokens->at(1)->GetType() == EASY_TOKEN_TYPE::OPERATOR);
        REQUIRE(reinterpret_cast<OperatorToken*>(tokens->at(1))->Value == EASY_OPERATOR_TYPE::DIVISION);
        
        REQUIRE(tokens->at(2)->GetType() == EASY_TOKEN_TYPE::INTEGER);
        REQUIRE(reinterpret_cast<IntegerToken*>(tokens->at(2))->Value == 10);
    }
    
    SECTION( "10 - 10" ) {
        parser->Parse(_T("10 - 10"), tokens);
        REQUIRE(tokens->size() == 3);
        REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::INTEGER);
        REQUIRE(reinterpret_cast<IntegerToken*>(tokens->at(0))->Value == 10);
        
        REQUIRE(tokens->at(1)->GetType() == EASY_TOKEN_TYPE::OPERATOR);
        REQUIRE(reinterpret_cast<OperatorToken*>(tokens->at(1))->Value == EASY_OPERATOR_TYPE::MINUS);
        
        REQUIRE(tokens->at(2)->GetType() == EASY_TOKEN_TYPE::INTEGER);
        REQUIRE(reinterpret_cast<IntegerToken*>(tokens->at(2))->Value == 10);
    }
    
    SECTION( "10 + 10" ) {
        parser->Parse(_T("10 + 10"), tokens);
        REQUIRE(tokens->size() == 3);
        REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::INTEGER);
        REQUIRE(reinterpret_cast<IntegerToken*>(tokens->at(0))->Value == 10);
        
        REQUIRE(tokens->at(1)->GetType() == EASY_TOKEN_TYPE::OPERATOR);
        REQUIRE(reinterpret_cast<OperatorToken*>(tokens->at(1))->Value == EASY_OPERATOR_TYPE::PLUS);
        
        REQUIRE(tokens->at(2)->GetType() == EASY_TOKEN_TYPE::INTEGER);
        REQUIRE(reinterpret_cast<IntegerToken*>(tokens->at(2))->Value == 10);
    }

    SECTION( "10 * 10" ) {
        parser->Parse(_T("10 * 10"), tokens);
        REQUIRE(tokens->size() == 3);
        REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::INTEGER);
        REQUIRE(reinterpret_cast<IntegerToken*>(tokens->at(0))->Value == 10);

        REQUIRE(tokens->at(1)->GetType() == EASY_TOKEN_TYPE::OPERATOR);
        REQUIRE(reinterpret_cast<OperatorToken*>(tokens->at(1))->Value == EASY_OPERATOR_TYPE::MULTIPLICATION);

        REQUIRE(tokens->at(2)->GetType() == EASY_TOKEN_TYPE::INTEGER);
        REQUIRE(reinterpret_cast<IntegerToken*>(tokens->at(2))->Value == 10);
    }
}

TEST_CASE( "Function lexer test" ) {
    Tokinizer* parser = new StandartTokinizer();
    auto tokens = make_shared<std::vector<Token*>>();
    
    SECTION( "func test () return 1" ) {
        parser->Parse(_T("func test () return 1"), tokens);
        REQUIRE(tokens->size() == 6);
        REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::KEYWORD);
        REQUIRE(reinterpret_cast<KeywordToken*>(tokens->at(0))->Value == EASY_KEYWORD_TYPE::FUNC);
        
        REQUIRE(tokens->at(1)->GetType() == EASY_TOKEN_TYPE::SYMBOL);
        REQUIRE(reinterpret_cast<SymbolToken*>(tokens->at(1))->Value == _T("test"));
        
        REQUIRE(tokens->at(4)->GetType() == EASY_TOKEN_TYPE::KEYWORD);
        REQUIRE(reinterpret_cast<KeywordToken*>(tokens->at(4))->Value == EASY_KEYWORD_TYPE::RETURN);
        
        REQUIRE(tokens->at(5)->GetType() == EASY_TOKEN_TYPE::INTEGER);
        REQUIRE(reinterpret_cast<IntegerToken*>(tokens->at(5))->Value == 1);
    }
}


TEST_CASE( "Package lexer test" ) {
    Tokinizer* parser = new StandartTokinizer();
    auto tokens = make_shared<std::vector<Token*>>();

    SECTION( "package test" ) {
        parser->Parse(_T("package test"), tokens);
        REQUIRE(tokens->size() == 2);
        REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::KEYWORD);
        REQUIRE(reinterpret_cast<KeywordToken*>(tokens->at(0))->Value == EASY_KEYWORD_TYPE::PACKAGE);

        REQUIRE(tokens->at(1)->GetType() == EASY_TOKEN_TYPE::SYMBOL);
        REQUIRE(reinterpret_cast<SymbolToken*>(tokens->at(1))->Value == _T("test"));
    }

    SECTION( "core::isBool" ) {
        parser->Parse(_T("core::isBool"), tokens);
        REQUIRE(tokens->size() == 3);

        REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::SYMBOL);
        REQUIRE(reinterpret_cast<SymbolToken*>(tokens->at(0))->Value == _T("core"));

        REQUIRE(tokens->at(1)->GetType() == EASY_TOKEN_TYPE::OPERATOR);
        REQUIRE(reinterpret_cast<OperatorToken*>(tokens->at(1))->Value == EASY_OPERATOR_TYPE::DOUBLE_COLON);

        REQUIRE(tokens->at(2)->GetType() == EASY_TOKEN_TYPE::SYMBOL);
        REQUIRE(reinterpret_cast<SymbolToken*>(tokens->at(2))->Value == _T("isBool"));
    }
}

TEST_CASE("for i to 1 to 10 then core::print i") {
	Tokinizer* parser = new StandartTokinizer();
	auto tokens = make_shared<std::vector<Token*>>();
	parser->Parse(_T("for i to 1 to 10 then core::print i"), tokens);
	REQUIRE(tokens->size() == 11);
}

#endif
