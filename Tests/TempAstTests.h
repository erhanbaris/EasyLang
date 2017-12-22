#ifndef EASYLANG_TEMP_ASTSTESTS_H
#define EASYLANG_TEMP_ASTSTESTS_H

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

TEST_CASE("Temp ast test") {
	Tokinizer* tokinizer = new StandartTokinizer();
	AstParser* astParser = new AstParser();
	InterpreterBackend* backend = new InterpreterBackend();

	std::shared_ptr<std::vector<Token* > > tokens = make_shared<std::vector<Token* > >();
	std::shared_ptr<std::vector<Ast* > > asts = make_shared<std::vector<Ast* > >();


	SECTION("2 + 7 * 4") {
		tokinizer->Parse(L"2 + 7 * 4", tokens);
		astParser->TempParse(tokens, asts);

		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();
		REQUIRE(result->Integer == 30);
	}

	SECTION("7 + 3 * (10 / (12 / (3 + 1) - 1))") {
		tokinizer->Parse(L"7 + 3 * (10 / (12 / (3 + 1) - 1))", tokens);
		astParser->TempParse(tokens, asts);

		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();
		REQUIRE(result->Double == 22);
	}

	SECTION("7 + (3 + 2)") {
		tokinizer->Parse(L"7 + (3 + 2)", tokens);
		astParser->TempParse(tokens, asts);

		backend->Prepare(asts);
		PrimativeValue* result = backend->Execute();
		REQUIRE(result->Integer == 12);
	}
}

#endif
