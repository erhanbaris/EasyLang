#ifndef EASYLANG_ASTSTESTS_H
#define EASYLANG_ASTSTESTS_H

#define CATCH_CONFIG_RUNNER
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cmath>

#include "../Catch.h"
#include "../Lexer.h"
#include "../ASTs.h"

using namespace std;

TEST_CASE("Temp ast test") {
	Tokinizer* tokinizer = new StandartTokinizer();
	AstParser* astParser = new AstParser();
	std::shared_ptr<std::vector<Token* > > tokens = make_shared<std::vector<Token* > >();
	std::shared_ptr<std::vector<Ast* > > asts = make_shared<std::vector<Ast* > >();


	SECTION("50*100") {
		tokinizer->Parse(L"50*100", tokens);
		astParser->TempParse(tokens, asts);

		REQUIRE(asts.get()->size() == 1);

		BinaryAst* binary = reinterpret_cast<BinaryAst*>(asts.get()->at(0));
		REQUIRE(binary->Left != nullptr);
		REQUIRE(binary->Right != nullptr);
		REQUIRE(binary->Op == EASY_OPERATOR_TYPE::MULTIPLICATION);
		REQUIRE(reinterpret_cast<PrimativeAst*>(binary->Left)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
		REQUIRE(reinterpret_cast<PrimativeAst*>(binary->Left)->Value->Integer == 50);
		REQUIRE(reinterpret_cast<PrimativeAst*>(binary->Right)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
		REQUIRE(reinterpret_cast<PrimativeAst*>(binary->Right)->Value->Integer == 100);
	}
}

#endif
