#ifndef EASYLANG_LEXERTESTS_H
#define EASYLANG_LEXERTESTS_H

#define CATCH_CONFIG_RUNNER
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cmath>

#include "../Catch.h"
#include "../Lexer.h"

using namespace std;

TEST_CASE( "Double test" ) {
    Tokinizer* parser = new StandartTokinizer();
    auto tokens = make_shared<std::vector<Token*>>();
    parser->Parse(L".1", tokens);
    REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::DOUBLE);
    REQUIRE(reinterpret_cast<DoubleToken*>(tokens->at(0))->Value == 0.1);
    
    parser->Parse(L"1.1", tokens);
    REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::DOUBLE);
    REQUIRE(reinterpret_cast<DoubleToken*>(tokens->at(0))->Value == 1.1);
    
    parser->Parse(L"1990.0001", tokens);
    REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::DOUBLE);
    REQUIRE(reinterpret_cast<DoubleToken*>(tokens->at(0))->Value == 1990.0001);
    
    parser->Parse(L"-10.1", tokens);

    REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::OPERATOR);
    REQUIRE(reinterpret_cast<OperatorToken*>(tokens->at(0))->Value == EASY_OPERATOR_TYPE::MINUS);

    REQUIRE(tokens->at(1)->GetType() == EASY_TOKEN_TYPE::DOUBLE);
    REQUIRE(reinterpret_cast<DoubleToken*>(tokens->at(1))->Value == 10.1);
    
    parser->Parse(L"-.1", tokens);
    REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::OPERATOR);
    REQUIRE(reinterpret_cast<OperatorToken*>(tokens->at(0))->Value == EASY_OPERATOR_TYPE::MINUS);

    REQUIRE(tokens->at(1)->GetType() == EASY_TOKEN_TYPE::DOUBLE);
    REQUIRE(reinterpret_cast<DoubleToken*>(tokens->at(1))->Value == 0.1);
}

TEST_CASE( "Integer test" ) {
    Tokinizer* parser = new StandartTokinizer();
    auto tokens = make_shared<std::vector<Token*>>();
    parser->Parse(L"1", tokens);
    REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::INTEGER);
    REQUIRE(reinterpret_cast<IntegerToken*>(tokens->at(0))->Value == 1);
    
    parser->Parse(L"12345", tokens);
    REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::INTEGER);
    REQUIRE(reinterpret_cast<IntegerToken*>(tokens->at(0))->Value == 12345);
    
    parser->Parse(L"-12345", tokens);
    REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::OPERATOR);
    REQUIRE(reinterpret_cast<OperatorToken*>(tokens->at(0))->Value == EASY_OPERATOR_TYPE::MINUS);

    REQUIRE(tokens->at(1)->GetType() == EASY_TOKEN_TYPE::INTEGER);
    REQUIRE(reinterpret_cast<IntegerToken*>(tokens->at(1))->Value == 12345);
}

TEST_CASE( "Bool test" ) {
    Tokinizer* parser = new StandartTokinizer();
    auto tokens = make_shared<std::vector<Token*>>();
    parser->Parse(L"olumsuz + olumsuz", tokens);
    REQUIRE(tokens->size() == 5);
}

TEST_CASE( "String test" ) {
    Tokinizer* parser = new StandartTokinizer();
    auto tokens = make_shared<std::vector<Token*>>();
    parser->Parse(L"\"erhan\"", tokens);
    REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::TEXT);
    REQUIRE(reinterpret_cast<TextToken*>(tokens->at(0))->Value == L"erhan");
    
    parser->Parse(L"\"'erhan'\"", tokens);
    REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::TEXT);
    REQUIRE(reinterpret_cast<TextToken*>(tokens->at(0))->Value == L"'erhan'");
    
    parser->Parse(L"\"\\\"erhan\\\"\"", tokens);
    REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::TEXT);
    REQUIRE(reinterpret_cast<TextToken*>(tokens->at(0))->Value == L"\"erhan\"");
    
    SECTION( "Sembol" ) {
        parser->Parse(L"17 <= 40", tokens);
        REQUIRE(tokens->size() == 5);
        REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::INTEGER);
        REQUIRE(reinterpret_cast<IntegerToken*>(tokens->at(0))->Value == 17);
        
        REQUIRE(tokens->at(2)->GetType() == EASY_TOKEN_TYPE::OPERATOR);
        REQUIRE(reinterpret_cast<OperatorToken*>(tokens->at(2))->Value == EASY_OPERATOR_TYPE::LOWER_EQUAL);
        
        REQUIRE(tokens->at(4)->GetType() == EASY_TOKEN_TYPE::INTEGER);
        REQUIRE(reinterpret_cast<IntegerToken*>(tokens->at(4))->Value == 40);
    }
    
    SECTION( "17 > 40" ) {
        parser->Parse(L"17 > 40", tokens);
        REQUIRE(tokens->size() == 5);
        REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::INTEGER);
        REQUIRE(reinterpret_cast<IntegerToken*>(tokens->at(0))->Value == 17);
        
        REQUIRE(tokens->at(2)->GetType() == EASY_TOKEN_TYPE::OPERATOR);
        REQUIRE(reinterpret_cast<OperatorToken*>(tokens->at(2))->Value == EASY_OPERATOR_TYPE::GREATOR);
        
        REQUIRE(tokens->at(4)->GetType() == EASY_TOKEN_TYPE::INTEGER);
        REQUIRE(reinterpret_cast<IntegerToken*>(tokens->at(4))->Value == 40);
    }
}

TEST_CASE( "Operator lexer test" ) {
    Tokinizer* parser = new StandartTokinizer();
    auto tokens = make_shared<std::vector<Token*>>();
    
    SECTION( "10 / 10" ) {
        parser->Parse(L"10 / 10", tokens);
        REQUIRE(tokens->size() == 5);
        REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::INTEGER);
        REQUIRE(reinterpret_cast<IntegerToken*>(tokens->at(0))->Value == 10);
        
        REQUIRE(tokens->at(2)->GetType() == EASY_TOKEN_TYPE::OPERATOR);
        REQUIRE(reinterpret_cast<OperatorToken*>(tokens->at(2))->Value == EASY_OPERATOR_TYPE::DIVISION);
        
        REQUIRE(tokens->at(4)->GetType() == EASY_TOKEN_TYPE::INTEGER);
        REQUIRE(reinterpret_cast<IntegerToken*>(tokens->at(4))->Value == 10);
    }
    
    SECTION( "10 - 10" ) {
        parser->Parse(L"10 - 10", tokens);
        REQUIRE(tokens->size() == 5);
        REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::INTEGER);
        REQUIRE(reinterpret_cast<IntegerToken*>(tokens->at(0))->Value == 10);
        
        REQUIRE(tokens->at(2)->GetType() == EASY_TOKEN_TYPE::OPERATOR);
        REQUIRE(reinterpret_cast<OperatorToken*>(tokens->at(2))->Value == EASY_OPERATOR_TYPE::MINUS);
        
        REQUIRE(tokens->at(4)->GetType() == EASY_TOKEN_TYPE::INTEGER);
        REQUIRE(reinterpret_cast<IntegerToken*>(tokens->at(4))->Value == 10);
    }
    
    SECTION( "10 + 10" ) {
        parser->Parse(L"10 + 10", tokens);
        REQUIRE(tokens->size() == 5);
        REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::INTEGER);
        REQUIRE(reinterpret_cast<IntegerToken*>(tokens->at(0))->Value == 10);
        
        REQUIRE(tokens->at(2)->GetType() == EASY_TOKEN_TYPE::OPERATOR);
        REQUIRE(reinterpret_cast<OperatorToken*>(tokens->at(2))->Value == EASY_OPERATOR_TYPE::PLUS);
        
        REQUIRE(tokens->at(4)->GetType() == EASY_TOKEN_TYPE::INTEGER);
        REQUIRE(reinterpret_cast<IntegerToken*>(tokens->at(4))->Value == 10);
    }

    SECTION( "10 * 10" ) {
        parser->Parse(L"10 * 10", tokens);
        REQUIRE(tokens->size() == 5);
        REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::INTEGER);
        REQUIRE(reinterpret_cast<IntegerToken*>(tokens->at(0))->Value == 10);

        REQUIRE(tokens->at(2)->GetType() == EASY_TOKEN_TYPE::OPERATOR);
        REQUIRE(reinterpret_cast<OperatorToken*>(tokens->at(2))->Value == EASY_OPERATOR_TYPE::MULTIPLICATION);

        REQUIRE(tokens->at(4)->GetType() == EASY_TOKEN_TYPE::INTEGER);
        REQUIRE(reinterpret_cast<IntegerToken*>(tokens->at(4))->Value == 10);
    }
}

TEST_CASE( "Function lexer test" ) {
    Tokinizer* parser = new StandartTokinizer();
    auto tokens = make_shared<std::vector<Token*>>();
    
    SECTION( "func test () return 1" ) {
        parser->Parse(L"func test () return 1", tokens);
        REQUIRE(tokens->size() == 10);
        REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::KEYWORD);
        REQUIRE(reinterpret_cast<KeywordToken*>(tokens->at(0))->Value == EASY_KEYWORD_TYPE::FUNC);
        
        REQUIRE(tokens->at(2)->GetType() == EASY_TOKEN_TYPE::SYMBOL);
        REQUIRE(reinterpret_cast<SymbolToken*>(tokens->at(2))->Value == L"test");
        
        REQUIRE(tokens->at(7)->GetType() == EASY_TOKEN_TYPE::KEYWORD);
        REQUIRE(reinterpret_cast<KeywordToken*>(tokens->at(7))->Value == EASY_KEYWORD_TYPE::RETURN);
        
        REQUIRE(tokens->at(9)->GetType() == EASY_TOKEN_TYPE::INTEGER);
        REQUIRE(reinterpret_cast<IntegerToken*>(tokens->at(9))->Value == 1);
    }
}


TEST_CASE( "Package lexer test" ) {
    Tokinizer* parser = new StandartTokinizer();
    auto tokens = make_shared<std::vector<Token*>>();

    SECTION( "package test" ) {
        parser->Parse(L"package test", tokens);
        REQUIRE(tokens->size() == 3);
        REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::KEYWORD);
        REQUIRE(reinterpret_cast<KeywordToken*>(tokens->at(0))->Value == EASY_KEYWORD_TYPE::PACKAGE);

        REQUIRE(tokens->at(2)->GetType() == EASY_TOKEN_TYPE::SYMBOL);
        REQUIRE(reinterpret_cast<SymbolToken*>(tokens->at(2))->Value == L"test");
    }

    SECTION( "core::isBool" ) {
        parser->Parse(L"core::isBool", tokens);
        REQUIRE(tokens->size() == 3);

        REQUIRE(tokens->at(0)->GetType() == EASY_TOKEN_TYPE::SYMBOL);
        REQUIRE(reinterpret_cast<SymbolToken*>(tokens->at(0))->Value == L"core");

        REQUIRE(tokens->at(1)->GetType() == EASY_TOKEN_TYPE::OPERATOR);
        REQUIRE(reinterpret_cast<OperatorToken*>(tokens->at(1))->Value == EASY_OPERATOR_TYPE::DOUBLE_COLON);

        REQUIRE(tokens->at(2)->GetType() == EASY_TOKEN_TYPE::SYMBOL);
        REQUIRE(reinterpret_cast<SymbolToken*>(tokens->at(2))->Value == L"isBool");
    }
}

#endif
