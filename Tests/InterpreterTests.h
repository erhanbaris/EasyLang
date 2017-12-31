#ifndef EASYLANG_INTERPRETERTESTS_H
#define EASYLANG_INTERPRETERTESTS_H

#define CATCH_CONFIG_RUNNER
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cmath>

#include "../include/Catch.h"
#include "../include/Lexer.h"
#include "../include/ASTs.h"
#include "../include/InterpreterBackend.h"
#include "../include/Definitions.h"

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
		tokinizer->Parse(_T("if 100 > 15 then return 123"), tokens);
		astParser->Parse(tokens, asts);
		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();
		REQUIRE(result != nullptr);
		REQUIRE(result->IsInteger());
		REQUIRE(result->Integer == 123);
	}

	SECTION("if 100 < 15 then return 123") {
		tokinizer->Parse(_T("if 100 < 15 then return 123"), tokens);
		astParser->Parse(tokens, asts);
		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();
		REQUIRE(result == nullptr);
	}

	SECTION("func test(data) { return 1 + data }") {
		tokinizer->Parse(_T("func test(data) { return 1 + data }"), tokens);
		astParser->Parse(tokens, asts);
		backend->Prepare(asts);
		backend->Execute();

		tokinizer->Parse(_T("test(2)"), tokens);
		astParser->Parse(tokens, asts);
		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();
		REQUIRE(result != nullptr);
		REQUIRE(result->IsInteger());
		REQUIRE(result->Integer == 3);

		tokinizer->Parse(_T("test 2"), tokens);
		astParser->Parse(tokens, asts);
		backend->Prepare(asts);
		result = backend->Execute();
		REQUIRE(result != nullptr);
		REQUIRE(result->IsInteger());
		REQUIRE(result->Integer == 3);
	}

	SECTION("fibonacci test") {
		tokinizer->Parse(_T("func fibonacci(num) { if num <= 1 then return 1 left = fibonacci(num - 1) right = fibonacci(num - 2) return left + right }"), tokens);
		astParser->Parse(tokens, asts);
		backend->Prepare(asts);
		backend->Execute();

		tokinizer->Parse(_T("fibonacci(10)"), tokens);
		astParser->Parse(tokens, asts);
		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();
		REQUIRE(result != nullptr);
		REQUIRE(result->IsInteger());
		REQUIRE(result->Integer == 89);
	}

	SECTION("sum test 1") {
		tokinizer->Parse(_T("func sum(a,b) return a + b"), tokens);
		astParser->Parse(tokens, asts);
		backend->Prepare(asts);
		backend->Execute();

		tokinizer->Parse(_T("sum(10, 10)"), tokens);
		astParser->Parse(tokens, asts);
		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();
		REQUIRE(result != nullptr);
		REQUIRE(result->IsInteger());
		REQUIRE(result->Integer == 20);
	}

	SECTION("sum test 2") {
		tokinizer->Parse(_T("func sum(a,b) return a + b"), tokens);
		astParser->Parse(tokens, asts);
		backend->Prepare(asts);
		backend->Execute();

		tokinizer->Parse(_T("sum(\"a\", \"b\")"), tokens);
		astParser->Parse(tokens, asts);
		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();
		REQUIRE(result != nullptr);
		REQUIRE(result->IsString());
		REQUIRE(*result->String == _T("ab"));
	}

	SECTION("((10 - 10) - (10 * 10))") {
		tokinizer->Parse(_T("((10 - 10) - (10 * 10))"), tokens);
		astParser->Parse(tokens, asts);

		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();

		REQUIRE(result != nullptr);
		REQUIRE(result->IsInteger());
		REQUIRE(result->Integer == -100);
	}

	SECTION("(40 - (10 * 10))") {
		tokinizer->Parse(_T("(40 - (10 * 10))"), tokens);
		astParser->Parse(tokens, asts);

		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();

		REQUIRE(result != nullptr);
		REQUIRE(result->IsInteger());
		REQUIRE(result->Integer == -60);
	}

    SECTION("(40 - (10 * 10) * 2)") {
        tokinizer->Parse(_T("(40 - (10 * 10) * 2)"), tokens);
        astParser->Parse(tokens, asts);

        backend->Prepare(asts);
        PrimativeValue* result = backend->Execute();

        REQUIRE(result != nullptr);
        REQUIRE(result->IsInteger());
        REQUIRE(result->Integer == -160);
    }

	SECTION("((10 * 10) * 40)") {
		tokinizer->Parse(_T("((10 * 10) * 40)"), tokens);
		astParser->Parse(tokens, asts);

		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();

		REQUIRE(result != nullptr);
		REQUIRE(result->IsInteger());
		REQUIRE(result->Integer == 4000);
	}

	SECTION("((10 - 10) * 40)") {
		tokinizer->Parse(_T("((10 - 10) * 40)"), tokens);
		astParser->Parse(tokens, asts);

		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();

		REQUIRE(result != nullptr);
		REQUIRE(result->IsInteger());
		REQUIRE(result->Integer == 0);
	}

	SECTION("2 + 7 * 4") {
		tokinizer->Parse(_T("2 + 7 * 4"), tokens);
		astParser->Parse(tokens, asts);

		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();
		REQUIRE(result->Integer == 30);
	}

	SECTION("7 + 3 * (10 / (12 / (3 + 1) - 1))") {
		tokinizer->Parse(_T("7 + 3 * (10 / (12 / (3 + 1) - 1))"), tokens);
		astParser->Parse(tokens, asts);

		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();
		REQUIRE(result->Double == 22);
	}

	SECTION("(7 + (3 + 2))") {
		tokinizer->Parse(_T("(7 + (3 + 2))"), tokens);
		astParser->Parse(tokens, asts);

		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();
		REQUIRE(result->Integer == 12);
	}

	SECTION("testvar = 10 testvar * 10 - 10") {
		tokinizer->Parse(_T("testvar = 10 testvar * 10 - 10"), tokens);
		astParser->Parse(tokens, asts);

		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();
		REQUIRE(result->Integer == 90);
	}

	SECTION("data = 10  func setData(a) { data = data + 1 + a } setData(1)") {
		tokinizer->Parse(_T("data = 10  func setData(a) { data = data + 1 + a } setData(10)"), tokens);
		astParser->Parse(tokens, asts);
		backend->Prepare(asts);
		backend->Execute();

		tokinizer->Parse(_T("data"), tokens);
		astParser->Parse(tokens, asts);
		backend->Prepare(asts);

		PrimativeValue* result = backend->Execute();
		REQUIRE(result->Integer == 21);
	}

	SECTION("20-10") {
		tokinizer->Parse(_T("20-10"), tokens);
		astParser->Parse(tokens, asts);

		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();
		REQUIRE(result->Integer == 10);
	}

	SECTION("20-10-5") {
		tokinizer->Parse(_T("20-10-5"), tokens);
		astParser->Parse(tokens, asts);

		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();
		REQUIRE(result->Integer == 5);
	}

	SECTION("20-10-5/2") {
		tokinizer->Parse(_T("20-10-5/2"), tokens);
		astParser->Parse(tokens, asts);

		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();
		REQUIRE(result->Double == 7.5);
	}



    SECTION( "(-1*2)" ) {
        tokinizer->Parse(_T("(-1*2)"), tokens);
        astParser->Parse(tokens, asts);

        backend->Prepare(asts);
        PrimativeValue* result = backend->Execute();
        REQUIRE(result->Integer == -2);
    }

	SECTION( "-1*-1" ) {
		tokinizer->Parse(_T("-1*-1"), tokens);
		astParser->Parse(tokens, asts);

		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();
		REQUIRE(result->Integer == 1);
	}

	SECTION( "-0.1" ) {
		tokinizer->Parse(_T("-0.1"), tokens);
		astParser->Parse(tokens, asts);

		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();
		REQUIRE(result->Double == -0.1);
	}

	SECTION("core::length(\"test\")") {
		tokinizer->Parse(_T("core::length(\"test\")"), tokens);
		astParser->Parse(tokens, asts);
		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();
		REQUIRE(result != nullptr);
		REQUIRE(result->IsInteger());
		REQUIRE(result->Integer == 4);
	}

	SECTION("core::length(123)") {
		tokinizer->Parse(_T("core::length(123)"), tokens);
		astParser->Parse(tokens, asts);
		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();
		REQUIRE(result->Integer == 1);
	}

	SECTION("data = [] core::length(data)") {
		tokinizer->Parse(_T("data = [] core::length(data)"), tokens);
		astParser->Parse(tokens, asts);
		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();
		REQUIRE(result != nullptr);
		REQUIRE(result->IsInteger());
		REQUIRE(result->Integer == 0);
	}
	
	SECTION("data = 0 for i in 1 to 10 then data = data + i data") {
		tokinizer->Parse(_T("data = 0 for i in 1 to 10 then data = data + i data"), tokens);
		astParser->Parse(tokens, asts);
		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();
		REQUIRE(result != nullptr);
		REQUIRE(result->IsInteger());
		REQUIRE(result->Integer == 55);
	}

	SECTION("core::length([])") {
		tokinizer->Parse(_T("core::length([])"), tokens);
		astParser->Parse(tokens, asts);
		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();
		REQUIRE(result != nullptr);
		REQUIRE(result->IsInteger());
		REQUIRE(result->Integer == 0);
	}

	SECTION("data = [] data <+ \"test\" core::length(data)") {
		tokinizer->Parse(_T("data = [] data <+ \"test\" core::length(data)"), tokens);
		astParser->Parse(tokens, asts);
		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();
		REQUIRE(result != nullptr);
		REQUIRE(result->IsInteger());
		REQUIRE(result->Integer == 1);
	}
}

#endif //EASYLANG_INTERPRETERTESTS_H
