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
	REQUIRE(reinterpret_cast<PrimativeAst*>(assignment->Data)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
	REQUIRE(reinterpret_cast<PrimativeAst*>(assignment->Data)->Value->Integer == 50);

	REQUIRE(assignment->Data != nullptr);

	tokinizer->Parse(L"atama erhan 50", tokens);
	astParser->Parse(tokens, asts);

	REQUIRE(asts.get()->size() == 1);
	assignment = reinterpret_cast<AssignmentAst*>(asts.get()->at(0));
	REQUIRE(assignment->Name == L"erhan");
	REQUIRE(assignment->Data != nullptr);
	REQUIRE(assignment->Data->GetType() == AstType::PRIMATIVE);
	REQUIRE(reinterpret_cast<PrimativeAst*>(assignment->Data)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
	REQUIRE(reinterpret_cast<PrimativeAst*>(assignment->Data)->Value->Integer == 50);

	tokinizer->Parse(L"atama erhan \"deneme\"", tokens);
	astParser->Parse(tokens, asts);

	REQUIRE(asts.get()->size() == 1);
	assignment = reinterpret_cast<AssignmentAst*>(asts.get()->at(0));
	REQUIRE(assignment->Name == L"erhan");
	REQUIRE(assignment->Data != nullptr);
	REQUIRE(assignment->Data->GetType() == AstType::PRIMATIVE);
	REQUIRE(reinterpret_cast<PrimativeAst*>(assignment->Data)->Value->Type == PrimativeValue::Type::PRI_STRING);
	REQUIRE(reinterpret_cast<PrimativeAst*>(assignment->Data)->Value->String == L"deneme");

	tokinizer->Parse(L"atama erhan'a \"deneme\"", tokens);
	astParser->Parse(tokens, asts);

	REQUIRE(asts.get()->size() == 1);
	assignment = reinterpret_cast<AssignmentAst*>(asts.get()->at(0));
	REQUIRE(assignment->Name == L"erhan");
	REQUIRE(assignment->Data != nullptr);
	REQUIRE(assignment->Data->GetType() == AstType::PRIMATIVE);
	REQUIRE(reinterpret_cast<PrimativeAst*>(assignment->Data)->Value->Type == PrimativeValue::Type::PRI_STRING);
	REQUIRE(reinterpret_cast<PrimativeAst*>(assignment->Data)->Value->String == L"deneme");


	tokinizer->Parse(L"atama erhan'a 0.15", tokens);
	astParser->Parse(tokens, asts);

	REQUIRE(asts.get()->size() == 1);
	assignment = reinterpret_cast<AssignmentAst*>(asts.get()->at(0));
	REQUIRE(assignment->Name == L"erhan");
	REQUIRE(assignment->Data != nullptr);
	REQUIRE(assignment->Data->GetType() == AstType::PRIMATIVE);
	REQUIRE(reinterpret_cast<PrimativeAst*>(assignment->Data)->Value->Type == PrimativeValue::Type::PRI_DOUBLE);
	REQUIRE(reinterpret_cast<PrimativeAst*>(assignment->Data)->Value->Double == 0.15);

	tokinizer->Parse(L"atama erhan'a .15", tokens);
	astParser->Parse(tokens, asts);

	REQUIRE(asts.get()->size() == 1);
	assignment = reinterpret_cast<AssignmentAst*>(asts.get()->at(0));
	REQUIRE(assignment->Name == L"erhan");
	REQUIRE(assignment->Data != nullptr);
	REQUIRE(assignment->Data->GetType() == AstType::PRIMATIVE);
	REQUIRE(reinterpret_cast<PrimativeAst*>(assignment->Data)->Value->Type == PrimativeValue::Type::PRI_DOUBLE);
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
	REQUIRE(reinterpret_cast<PrimativeAst*>(reinterpret_cast<BinaryAst*>(assignment->Data)->Left)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
	REQUIRE(reinterpret_cast<PrimativeAst*>(reinterpret_cast<BinaryAst*>(assignment->Data)->Left)->Value->Integer == 50);
	REQUIRE(reinterpret_cast<PrimativeAst*>(reinterpret_cast<BinaryAst*>(assignment->Data)->Right)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
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

TEST_CASE("Block test") {
	Tokinizer* tokinizer = new StandartTokinizer();
	AstParser* astParser = new AstParser();

	std::shared_ptr<std::vector<Token* > > tokens = make_shared<std::vector<Token* > >();
	std::shared_ptr<std::vector<Ast* > > asts = make_shared<std::vector<Ast* > >();

	tokinizer->Parse(L"başla atama erhan'a 50 eksi 5 bitir", tokens);
	astParser->Parse(tokens, asts);
	REQUIRE(asts.get()->size() == 1);

	auto* block = reinterpret_cast<BlockAst*>(asts.get()->at(0));
	REQUIRE(block->Blocks->size() == 1);

	AssignmentAst* assignment = reinterpret_cast<AssignmentAst*>(block->Blocks->at(0));
	REQUIRE(assignment->Name == L"erhan");
	REQUIRE(assignment->Data != nullptr);
	REQUIRE(assignment->Data->GetType() == AstType::BINARY_OPERATION);
	REQUIRE(reinterpret_cast<BinaryAst*>(assignment->Data)->Left->GetType() == AstType::PRIMATIVE);
	REQUIRE(reinterpret_cast<BinaryAst*>(assignment->Data)->Right->GetType() == AstType::PRIMATIVE);
	REQUIRE(reinterpret_cast<BinaryAst*>(assignment->Data)->Op == EASY_OPERATOR_TYPE::MINUS);
	REQUIRE(reinterpret_cast<PrimativeAst*>(reinterpret_cast<BinaryAst*>(assignment->Data)->Left)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
	REQUIRE(reinterpret_cast<PrimativeAst*>(reinterpret_cast<BinaryAst*>(assignment->Data)->Left)->Value->Integer == 50);
	REQUIRE(reinterpret_cast<PrimativeAst*>(reinterpret_cast<BinaryAst*>(assignment->Data)->Right)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
	REQUIRE(reinterpret_cast<PrimativeAst*>(reinterpret_cast<BinaryAst*>(assignment->Data)->Right)->Value->Integer == 5);

	tokinizer->Parse(L"başla bitir başla bitir başla yaz 123 bitir", tokens);
	astParser->Parse(tokens, asts);
	REQUIRE(asts.get()->size() == 3);

	REQUIRE(reinterpret_cast<BlockAst*>(asts.get()->at(0))->Blocks->size() == 0);
	REQUIRE(reinterpret_cast<BlockAst*>(asts.get()->at(1))->Blocks->size() == 0);
	REQUIRE(reinterpret_cast<BlockAst*>(asts.get()->at(2))->Blocks->size() == 1);
}

TEST_CASE("If test") {
	Tokinizer* tokinizer = new StandartTokinizer();
	AstParser* astParser = new AstParser();

	std::shared_ptr<std::vector<Token* > > tokens = make_shared<std::vector<Token* > >();
	std::shared_ptr<std::vector<Ast* > > asts = make_shared<std::vector<Ast* > >();

	SECTION("eğer erhan büyükise 15 sonra yaz 123") {
		tokinizer->Parse(L"eğer erhan büyükise 15 sonra yaz 123", tokens);
		astParser->Parse(tokens, asts);
		REQUIRE(asts.get()->size() == 1);

		auto* ifStatement = reinterpret_cast<IfStatementAst*>(asts.get()->at(0));

		REQUIRE(ifStatement->True != nullptr);
		REQUIRE(ifStatement->False == nullptr);
		REQUIRE(ifStatement->ControlOpt != nullptr);
		REQUIRE(ifStatement->ControlOpt->GetType() == AstType::CONTROL_OPERATION);
		auto * controlOpt = reinterpret_cast<ControlAst*>(ifStatement->ControlOpt);
		REQUIRE(controlOpt->Left != nullptr);
		REQUIRE(controlOpt->Right != nullptr);
		REQUIRE(controlOpt->Op == EASY_OPERATOR_TYPE::GREATOR);
	}

	SECTION("eğer erhan büyükise 15 sonra yaz 123 değilse yaz 321") {
		tokinizer->Parse(L"eğer erhan büyükise 15 sonra yaz 123 değilse yaz 321", tokens);
		astParser->Parse(tokens, asts);
		REQUIRE(asts.get()->size() == 1);

		auto* ifStatement = reinterpret_cast<IfStatementAst*>(asts.get()->at(0));

		REQUIRE(ifStatement->True != nullptr);
		REQUIRE(ifStatement->False != nullptr);
		REQUIRE(ifStatement->ControlOpt != nullptr);
		REQUIRE(ifStatement->ControlOpt->GetType() == AstType::CONTROL_OPERATION);
		auto * controlOpt = reinterpret_cast<ControlAst*>(ifStatement->ControlOpt);
		REQUIRE(controlOpt->Left != nullptr);
		REQUIRE(controlOpt->Right != nullptr);
		REQUIRE(controlOpt->Op == EASY_OPERATOR_TYPE::GREATOR);
	}
}


TEST_CASE("Function call test") {
	Tokinizer* tokinizer = new StandartTokinizer();
	AstParser* astParser = new AstParser();

	std::shared_ptr<std::vector<Token* > > tokens = make_shared<std::vector<Token* > >();
	std::shared_ptr<std::vector<Ast* > > asts = make_shared<std::vector<Ast* > >();


	SECTION("yaz \"test\"") {
		tokinizer->Parse(L"yaz \"test\"", tokens);
		astParser->Parse(tokens, asts);
		REQUIRE(asts.get()->size() == 1);

		auto* functionCall = reinterpret_cast<FunctionCallAst*>(asts.get()->at(0));

		REQUIRE(functionCall->Function == L"yaz");
		REQUIRE(functionCall->Args.size() == 1);
		REQUIRE(functionCall->Args.at(0)->GetType() == AstType::PRIMATIVE);
		REQUIRE(reinterpret_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->Type == PrimativeValue::Type::PRI_STRING);
		REQUIRE(reinterpret_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->String == L"test");
	}

	SECTION("yaz 5.5") {
		tokinizer->Parse(L"yaz 5.5", tokens);
		astParser->Parse(tokens, asts);
		REQUIRE(asts.get()->size() == 1);

		auto* functionCall = reinterpret_cast<FunctionCallAst*>(asts.get()->at(0));

		REQUIRE(functionCall->Function == L"yaz");
		REQUIRE(functionCall->Args.size() == 1);
		REQUIRE(functionCall->Args.at(0)->GetType() == AstType::PRIMATIVE);
		REQUIRE(reinterpret_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->Type == PrimativeValue::Type::PRI_DOUBLE);
		REQUIRE(reinterpret_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->Double == 5.5);
	}

	SECTION("yaz 123") {
		tokinizer->Parse(L"yaz 123", tokens);
		astParser->Parse(tokens, asts);
		REQUIRE(asts.get()->size() == 1);

		auto* functionCall = reinterpret_cast<FunctionCallAst*>(asts.get()->at(0));

		REQUIRE(functionCall->Function == L"yaz");
		REQUIRE(functionCall->Args.size() == 1);
		REQUIRE(functionCall->Args.at(0)->GetType() == AstType::PRIMATIVE);
		REQUIRE(reinterpret_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->Type == PrimativeValue::Type::PRI_INTEGER);
		REQUIRE(reinterpret_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->Integer == 123);
	}
}



TEST_CASE("Bool test") {
    Tokinizer* tokinizer = new StandartTokinizer();
    AstParser* astParser = new AstParser();

    std::shared_ptr<std::vector<Token* > > tokens = make_shared<std::vector<Token* > >();
    std::shared_ptr<std::vector<Ast* > > asts = make_shared<std::vector<Ast* > >();


    SECTION("yaz evet") {
        tokinizer->Parse(L"yaz evet", tokens);
        astParser->Parse(tokens, asts);
        REQUIRE(asts.get()->size() == 1);

        auto* functionCall = reinterpret_cast<FunctionCallAst*>(asts.get()->at(0));

        REQUIRE(functionCall->Function == L"yaz");
        REQUIRE(functionCall->Args.size() == 1);
        REQUIRE(functionCall->Args.at(0)->GetType() == AstType::PRIMATIVE);
        REQUIRE(reinterpret_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->Type == PrimativeValue::Type::PRI_BOOL);
        REQUIRE(reinterpret_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->Bool == true);
    }
    
    SECTION("yaz hayır") {
        tokinizer->Parse(L"yaz hayır", tokens);
        astParser->Parse(tokens, asts);
        REQUIRE(asts.get()->size() == 1);
        
        auto* functionCall = reinterpret_cast<FunctionCallAst*>(asts.get()->at(0));
        
        REQUIRE(functionCall->Function == L"yaz");
        REQUIRE(functionCall->Args.size() == 1);
        REQUIRE(functionCall->Args.at(0)->GetType() == AstType::PRIMATIVE);
        REQUIRE(reinterpret_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->Type == PrimativeValue::Type::PRI_BOOL);
        REQUIRE(reinterpret_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->Bool == false);
    }
    
    
    SECTION("yaz doğru") {
        tokinizer->Parse(L"yaz doğru", tokens);
        astParser->Parse(tokens, asts);
        REQUIRE(asts.get()->size() == 1);
        
        auto* functionCall = reinterpret_cast<FunctionCallAst*>(asts.get()->at(0));
        
        REQUIRE(functionCall->Function == L"yaz");
        REQUIRE(functionCall->Args.size() == 1);
        REQUIRE(functionCall->Args.at(0)->GetType() == AstType::PRIMATIVE);
        REQUIRE(reinterpret_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->Type == PrimativeValue::Type::PRI_BOOL);
        REQUIRE(reinterpret_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->Bool == true);
    }
    
    SECTION("yaz yanlış") {
        tokinizer->Parse(L"yaz yanlış", tokens);
        astParser->Parse(tokens, asts);
        REQUIRE(asts.get()->size() == 1);
        
        auto* functionCall = reinterpret_cast<FunctionCallAst*>(asts.get()->at(0));
        
        REQUIRE(functionCall->Function == L"yaz");
        REQUIRE(functionCall->Args.size() == 1);
        REQUIRE(functionCall->Args.at(0)->GetType() == AstType::PRIMATIVE);
        REQUIRE(reinterpret_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->Type == PrimativeValue::Type::PRI_BOOL);
        REQUIRE(reinterpret_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->Bool == false);
    }
}

#endif
