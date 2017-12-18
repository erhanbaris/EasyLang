#ifndef EASYLANG_INTERPRETERTESTS_H
#define EASYLANG_INTERPRETERTESTS_H

#define CATCH_CONFIG_RUNNER
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cmath>

#include "../Catch.h"
#include "../Lexer.h"
#include "../ASTs.h"
#include "../InterpreterBackend.h"

using namespace std;

#define PREPARE(text)\
    tokinizer->Parse( L##text , tokens);\
    astParser->Parse(tokens, asts);\
    backend->Prepare(asts);

TEST_CASE("Interpreter tests") {
    Tokinizer* tokinizer = new StandartTokinizer();
    AstParser* astParser = new AstParser();
    InterpreterBackend* backend = new InterpreterBackend();
    PrimativeValue* value = nullptr;
    
    std::shared_ptr<std::vector<Token* > > tokens = make_shared<std::vector<Token* > >();
    std::shared_ptr<std::vector<Ast* > > asts = make_shared<std::vector<Ast* > >();
    
	SECTION("if 100 > 15 then return 123") {
		tokinizer->Parse(L"if 100 > 15 then return 123", tokens);
		astParser->Parse(tokens, asts);
		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();
		REQUIRE(result != nullptr);
		REQUIRE(result->IsInteger());
		REQUIRE(result->Integer == 123);
	}

	SECTION("if 100 < 15 then return 123") {
		tokinizer->Parse(L"if 100 < 15 then return 123", tokens);
		astParser->Parse(tokens, asts);
		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();
		REQUIRE(result == nullptr);
	}

	SECTION("func test(data) { return 1 + data }") {
		tokinizer->Parse(L"func test(data) { return 1 + data }", tokens);
		astParser->Parse(tokens, asts);
		backend->Prepare(asts);
		backend->Execute();

		tokinizer->Parse(L"test(2)", tokens);
		astParser->Parse(tokens, asts);
		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();
		REQUIRE(result != nullptr);
		REQUIRE(result->IsInteger());
		REQUIRE(result->Integer == 3);
	}
}

#endif //EASYLANG_INTERPRETERTESTS_H