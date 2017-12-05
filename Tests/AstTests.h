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

	tokinizer->Parse(L"atama erhan 50", tokens);
	astParser->Parse(tokens, asts);

	REQUIRE(asts.get()->size() == 1);
	assignment = reinterpret_cast<AssignmentAst*>(asts.get()->at(0));
	REQUIRE(assignment->Name == L"erhan");
	REQUIRE(assignment->Data != nullptr);
	REQUIRE(assignment->Data->GetType() == AstType::PRIMATIVE);
	REQUIRE(reinterpret_cast<PrimativeAst*>(assignment->Data)->ValueType == PrimativeValueType::PRI_INTEGER);
	REQUIRE(reinterpret_cast<PrimativeAst*>(assignment->Data)->Value->Integer == 50);

	tokinizer->Parse(L"atama erhan \"deneme\"", tokens);
	astParser->Parse(tokens, asts);

	REQUIRE(asts.get()->size() == 1);
	assignment = reinterpret_cast<AssignmentAst*>(asts.get()->at(0));
	REQUIRE(assignment->Name == L"erhan");
	REQUIRE(assignment->Data != nullptr);
	REQUIRE(assignment->Data->GetType() == AstType::PRIMATIVE);
	REQUIRE(reinterpret_cast<PrimativeAst*>(assignment->Data)->ValueType == PrimativeValueType::PRI_STRING);
	REQUIRE(reinterpret_cast<PrimativeAst*>(assignment->Data)->Value->String == L"deneme");

	tokinizer->Parse(L"atama erhan'a \"deneme\"", tokens);
	astParser->Parse(tokens, asts);

	REQUIRE(asts.get()->size() == 1);
	assignment = reinterpret_cast<AssignmentAst*>(asts.get()->at(0));
	REQUIRE(assignment->Name == L"erhan");
	REQUIRE(assignment->Data != nullptr);
	REQUIRE(assignment->Data->GetType() == AstType::PRIMATIVE);
	REQUIRE(reinterpret_cast<PrimativeAst*>(assignment->Data)->ValueType == PrimativeValueType::PRI_STRING);
	REQUIRE(reinterpret_cast<PrimativeAst*>(assignment->Data)->Value->String == L"deneme");


	tokinizer->Parse(L"atama erhan'a 0.15", tokens);
	astParser->Parse(tokens, asts);

	REQUIRE(asts.get()->size() == 1);
	assignment = reinterpret_cast<AssignmentAst*>(asts.get()->at(0));
	REQUIRE(assignment->Name == L"erhan");
	REQUIRE(assignment->Data != nullptr);
	REQUIRE(assignment->Data->GetType() == AstType::PRIMATIVE);
	REQUIRE(reinterpret_cast<PrimativeAst*>(assignment->Data)->ValueType == PrimativeValueType::PRI_DOUBLE);
	REQUIRE(reinterpret_cast<PrimativeAst*>(assignment->Data)->Value->Double == 0.15);

	tokinizer->Parse(L"atama erhan'a .15", tokens);
	astParser->Parse(tokens, asts);

	REQUIRE(asts.get()->size() == 1);
	assignment = reinterpret_cast<AssignmentAst*>(asts.get()->at(0));
	REQUIRE(assignment->Name == L"erhan");
	REQUIRE(assignment->Data != nullptr);
	REQUIRE(assignment->Data->GetType() == AstType::PRIMATIVE);
	REQUIRE(reinterpret_cast<PrimativeAst*>(assignment->Data)->ValueType == PrimativeValueType::PRI_DOUBLE);
	REQUIRE(reinterpret_cast<PrimativeAst*>(assignment->Data)->Value->Double == 0.15);


	tokinizer->Parse(L"atama erhan'a 50 artı 5", tokens);
	astParser->Parse(tokens, asts);

	REQUIRE(asts.get()->size() == 1);

	assignment = reinterpret_cast<AssignmentAst*>(asts.get()->at(0));
	REQUIRE(assignment->Name == L"erhan");
	REQUIRE(assignment->Data != nullptr);
	REQUIRE(assignment->Data->GetType() == AstType::BINARY_OPERATION);
	REQUIRE(reinterpret_cast<BinaryAst*>(assignment->Data)->Left->GetType() == AstType::PRIMATIVE);
	REQUIRE(reinterpret_cast<BinaryAst*>(assignment->Data)->Right->GetType() == AstType::PRIMATIVE);
	REQUIRE(reinterpret_cast<BinaryAst*>(assignment->Data)->Op == EASY_OPERATOR_TYPE::PLUS);


	tokinizer->Parse(L"atama erhan'a 50 bölü 5", tokens);
	astParser->Parse(tokens, asts);

	REQUIRE(asts.get()->size() == 1);

	assignment = reinterpret_cast<AssignmentAst*>(asts.get()->at(0));
	REQUIRE(assignment->Name == L"erhan");
	REQUIRE(assignment->Data != nullptr);
	REQUIRE(assignment->Data->GetType() == AstType::BINARY_OPERATION);
	REQUIRE(reinterpret_cast<BinaryAst*>(assignment->Data)->Left->GetType() == AstType::PRIMATIVE);
	REQUIRE(reinterpret_cast<BinaryAst*>(assignment->Data)->Right->GetType() == AstType::PRIMATIVE);
	REQUIRE(reinterpret_cast<BinaryAst*>(assignment->Data)->Op == EASY_OPERATOR_TYPE::DIVISION);
	REQUIRE(reinterpret_cast<PrimativeAst*>(reinterpret_cast<BinaryAst*>(assignment->Data)->Left)->ValueType == PrimativeValueType::PRI_INTEGER);
	REQUIRE(reinterpret_cast<PrimativeAst*>(reinterpret_cast<BinaryAst*>(assignment->Data)->Left)->Value->Integer == 50);
	REQUIRE(reinterpret_cast<PrimativeAst*>(reinterpret_cast<BinaryAst*>(assignment->Data)->Right)->ValueType == PrimativeValueType::PRI_INTEGER);
	REQUIRE(reinterpret_cast<PrimativeAst*>(reinterpret_cast<BinaryAst*>(assignment->Data)->Right)->Value->Integer == 5);


	tokinizer->Parse(L"atama erhan'a 50 eksi 5", tokens);
	astParser->Parse(tokens, asts);

	REQUIRE(asts.get()->size() == 1);

	assignment = reinterpret_cast<AssignmentAst*>(asts.get()->at(0));
	REQUIRE(assignment->Name == L"erhan");
	REQUIRE(assignment->Data != nullptr);
	REQUIRE(assignment->Data->GetType() == AstType::BINARY_OPERATION);
	REQUIRE(reinterpret_cast<BinaryAst*>(assignment->Data)->Left->GetType() == AstType::PRIMATIVE);
	REQUIRE(reinterpret_cast<BinaryAst*>(assignment->Data)->Right->GetType() == AstType::PRIMATIVE);
	REQUIRE(reinterpret_cast<BinaryAst*>(assignment->Data)->Op == EASY_OPERATOR_TYPE::MINUS);


	REQUIRE(assignment->Data != nullptr);

}

#endif