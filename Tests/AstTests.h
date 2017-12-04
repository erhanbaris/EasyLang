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

TEST_CASE( "Assignment test" ) {
	Tokinizer* tokinizer = new StandartTokinizer();
	AstParser* astParser = new AstParser();
	std::shared_ptr<std::vector<Token* > > tokens = make_shared<std::vector<Token* > >();
	std::shared_ptr<std::vector<Ast* > > asts = make_shared<std::vector<Ast* > >();
	tokinizer->Parse(L"atama erhan'a 50", tokens);
	astParser->Parse(tokens, asts);

	REQUIRE(asts.get()->size() == 1);

	AssignmentAst* assignment = reinterpret_cast<AssignmentAst*>(asts.get()->at(0));
	REQUIRE(assignment->Name == L"erhan");
	REQUIRE(assignment->Data != nullptr);
	REQUIRE(assignment->Data->GetType() == AstType::PRIMATIVE);
	REQUIRE(reinterpret_cast<PrimativeAst*>(assignment->Data)->ValueType == PrimativeValueType::PRI_INTEGER);
	REQUIRE(reinterpret_cast<PrimativeAst*>(assignment->Data)->Value->Integer == 50);

	REQUIRE(assignment->Data != nullptr);
}

#endif