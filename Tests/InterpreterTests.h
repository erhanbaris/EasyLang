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


	SECTION("fibonacci test") {
		tokinizer->Parse(L"func fibonacci(num) { if num <= 1 then return 1 left = fibonacci(num - 1) right = fibonacci(num - 2) return left + right }", tokens);
		astParser->Parse(tokens, asts);
		backend->Prepare(asts);
		backend->Execute();

		tokinizer->Parse(L"fibonacci(10)", tokens);
		astParser->Parse(tokens, asts);
		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();
		REQUIRE(result != nullptr);
		REQUIRE(result->IsInteger());
		REQUIRE(result->Integer == 89);
	}

	SECTION("sum test 1") {
		tokinizer->Parse(L"func sum(a,b) return a + b", tokens);
		astParser->Parse(tokens, asts);
		backend->Prepare(asts);
		backend->Execute();

		tokinizer->Parse(L"sum(10, 10)", tokens);
		astParser->Parse(tokens, asts);
		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();
		REQUIRE(result != nullptr);
		REQUIRE(result->IsInteger());
		REQUIRE(result->Integer == 20);
	}

	SECTION("sum test 2") {
		tokinizer->Parse(L"func sum(a,b) return a + b", tokens);
		astParser->Parse(tokens, asts);
		backend->Prepare(asts);
		backend->Execute();

		tokinizer->Parse(L"sum(\"a\", \"b\")", tokens);
		astParser->Parse(tokens, asts);
		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();
		REQUIRE(result != nullptr);
		REQUIRE(result->IsString());
		REQUIRE(*result->String == L"ab");
	}

	SECTION("((10 - 10) - (10 * 10))") {
		tokinizer->Parse(L"((10 - 10) - (10 * 10))", tokens);
		astParser->Parse(tokens, asts);

		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();

		REQUIRE(result != nullptr);
		REQUIRE(result->IsInteger());
		REQUIRE(result->Integer == -100);
	}

	SECTION("(40 - (10 * 10))") {
		tokinizer->Parse(L"(40 - (10 * 10))", tokens);
		astParser->Parse(tokens, asts);

		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();

		REQUIRE(result != nullptr);
		REQUIRE(result->IsInteger());
		REQUIRE(result->Integer == -60);
	}

	SECTION("((10 * 10) * 40)") {
		tokinizer->Parse(L"((10 * 10) * 40)", tokens);
		astParser->Parse(tokens, asts);

		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();

		REQUIRE(result != nullptr);
		REQUIRE(result->IsInteger());
		REQUIRE(result->Integer == 4000);
	}

	SECTION("((10 - 10) * 40)") {
		tokinizer->Parse(L"((10 - 10) * 40)", tokens);
		astParser->Parse(tokens, asts);

		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();

		REQUIRE(result != nullptr);
		REQUIRE(result->IsInteger());
		REQUIRE(result->Integer == 0);
	}
}

#endif //EASYLANG_INTERPRETERTESTS_H
