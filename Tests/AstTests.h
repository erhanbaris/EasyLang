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

TEST_CASE("Base test") {
	Tokinizer* tokinizer = new StandartTokinizer();
	AstParser* astParser = new AstParser();
	std::shared_ptr<std::vector<Token* > > tokens = make_shared<std::vector<Token* > >();
	std::shared_ptr<std::vector<Ast* > > asts = make_shared<std::vector<Ast* > >();


	SECTION("50 + 100") {
		tokinizer->Parse(L"50 + 100", tokens);
		astParser->Parse(tokens, asts);

		REQUIRE(asts.get()->size() == 1);

		BinaryAst* binary = reinterpret_cast<BinaryAst*>(asts.get()->at(0));
		REQUIRE(binary->Left != nullptr);
		REQUIRE(binary->Right != nullptr);
		REQUIRE(binary->Op == EASY_OPERATOR_TYPE::PLUS);
		REQUIRE(reinterpret_cast<PrimativeAst*>(binary->Left)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
		REQUIRE(reinterpret_cast<PrimativeAst*>(binary->Left)->Value->Integer == 50);
		REQUIRE(reinterpret_cast<PrimativeAst*>(binary->Right)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
		REQUIRE(reinterpret_cast<PrimativeAst*>(binary->Right)->Value->Integer == 100);
	}


	SECTION("[ ]") {
		tokinizer->Parse(L"[ ]", tokens);
		astParser->Parse(tokens, asts);

		REQUIRE(asts.get()->size() == 1);

		PrimativeAst* primative = reinterpret_cast<PrimativeAst*>(asts.get()->at(0));
		REQUIRE(primative != nullptr);
	}

	SECTION("50 - 100") {
		tokinizer->Parse(L"50 - 100", tokens);
		astParser->Parse(tokens, asts);

		REQUIRE(asts.get()->size() == 1);

		BinaryAst* binary = reinterpret_cast<BinaryAst*>(asts.get()->at(0));
		REQUIRE(binary->Left != nullptr);
		REQUIRE(binary->Right != nullptr);
		REQUIRE(binary->Op == EASY_OPERATOR_TYPE::MINUS);
		REQUIRE(reinterpret_cast<PrimativeAst*>(binary->Left)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
		REQUIRE(reinterpret_cast<PrimativeAst*>(binary->Left)->Value->Integer == 50);
		REQUIRE(reinterpret_cast<PrimativeAst*>(binary->Right)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
		REQUIRE(reinterpret_cast<PrimativeAst*>(binary->Right)->Value->Integer == 100);
	}

	SECTION("(50 - 100)") {
		tokinizer->Parse(L"50 - 100", tokens);
		astParser->Parse(tokens, asts);

		REQUIRE(asts.get()->size() == 1);

		BinaryAst* binary = reinterpret_cast<BinaryAst*>(asts.get()->at(0));
		REQUIRE(binary->Left != nullptr);
		REQUIRE(binary->Right != nullptr);
		REQUIRE(binary->Op == EASY_OPERATOR_TYPE::MINUS);
		REQUIRE(reinterpret_cast<PrimativeAst*>(binary->Left)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
		REQUIRE(reinterpret_cast<PrimativeAst*>(binary->Left)->Value->Integer == 50);
		REQUIRE(reinterpret_cast<PrimativeAst*>(binary->Right)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
		REQUIRE(reinterpret_cast<PrimativeAst*>(binary->Right)->Value->Integer == 100);
	}

	SECTION("(50 - (10 * 10))") {
		tokinizer->Parse(L"(50 - (10 * 10))", tokens);
		astParser->Parse(tokens, asts);

		REQUIRE(asts.get()->size() == 1);

		BinaryAst* binary = reinterpret_cast<BinaryAst*>(asts.get()->at(0));
		REQUIRE(binary->Left != nullptr);
		REQUIRE(binary->Right != nullptr);
		REQUIRE(binary->Op == EASY_OPERATOR_TYPE::MINUS);
		REQUIRE(reinterpret_cast<PrimativeAst*>(binary->Left)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
		REQUIRE(reinterpret_cast<PrimativeAst*>(binary->Left)->Value->Integer == 50);
		REQUIRE(binary->Right->GetType() == EASY_AST_TYPE::BINARY_OPERATION);
	}

	SECTION("((50 - 10) - 10)") {
		tokinizer->Parse(L"((50 - 10) - 10)", tokens);
		astParser->Parse(tokens, asts);

		REQUIRE(asts.get()->size() == 1);

		BinaryAst* binary = reinterpret_cast<BinaryAst*>(asts.get()->at(0));
		REQUIRE(binary->Left != nullptr);
		REQUIRE(binary->Right != nullptr);
		REQUIRE(binary->Op == EASY_OPERATOR_TYPE::MINUS);
		REQUIRE(binary->Left->GetType() == EASY_AST_TYPE::BINARY_OPERATION);
		REQUIRE(reinterpret_cast<PrimativeAst*>(binary->Right)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
	}

	SECTION("((50 - 10) - (10 * 10))") {
		tokinizer->Parse(L"((50 - 10) - (10 * 10))", tokens);
		astParser->Parse(tokens, asts);

		REQUIRE(asts.get()->size() == 1);

		BinaryAst* binary = reinterpret_cast<BinaryAst*>(asts.get()->at(0));
		REQUIRE(binary->Left != nullptr);
		REQUIRE(binary->Right != nullptr);
		REQUIRE(binary->Op == EASY_OPERATOR_TYPE::MINUS);
		REQUIRE(binary->Left->GetType() == EASY_AST_TYPE::BINARY_OPERATION); 
		REQUIRE(binary->Right->GetType() == EASY_AST_TYPE::BINARY_OPERATION);
	}
}

TEST_CASE( "Assignment test" ) {
	Tokinizer* tokinizer = new StandartTokinizer();
	AstParser* astParser = new AstParser();
	std::shared_ptr<std::vector<Token* > > tokens = make_shared<std::vector<Token* > >();
	std::shared_ptr<std::vector<Ast* > > asts = make_shared<std::vector<Ast* > >();
	tokinizer->Parse(L"erhan = 50", tokens);
	astParser->Parse(tokens, asts);

	REQUIRE(asts.get()->size() == 1);

	AssignmentAst* assignment = reinterpret_cast<AssignmentAst*>(asts.get()->at(0));
	REQUIRE(assignment->Name == L"erhan");
	REQUIRE(assignment->Data != nullptr);
	REQUIRE(assignment->Data->GetType() == EASY_AST_TYPE::PRIMATIVE);
	REQUIRE(reinterpret_cast<PrimativeAst*>(assignment->Data)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
	REQUIRE(reinterpret_cast<PrimativeAst*>(assignment->Data)->Value->Integer == 50);

	REQUIRE(assignment->Data != nullptr);

	tokinizer->Parse(L"erhan = 150", tokens);
	astParser->Parse(tokens, asts);

	REQUIRE(asts.get()->size() == 1);
	assignment = reinterpret_cast<AssignmentAst*>(asts.get()->at(0));
	REQUIRE(assignment->Name == L"erhan");
	REQUIRE(assignment->Data != nullptr);
	REQUIRE(assignment->Data->GetType() == EASY_AST_TYPE::PRIMATIVE);
	REQUIRE(reinterpret_cast<PrimativeAst*>(assignment->Data)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
	REQUIRE(reinterpret_cast<PrimativeAst*>(assignment->Data)->Value->Integer == 150);

	tokinizer->Parse(L"erhan = \"deneme\"", tokens);
	astParser->Parse(tokens, asts);

	REQUIRE(asts.get()->size() == 1);
	assignment = reinterpret_cast<AssignmentAst*>(asts.get()->at(0));
	REQUIRE(assignment->Name == L"erhan");
	REQUIRE(assignment->Data != nullptr);
	REQUIRE(assignment->Data->GetType() == EASY_AST_TYPE::PRIMATIVE);
	REQUIRE(reinterpret_cast<PrimativeAst*>(assignment->Data)->Value->Type == PrimativeValue::Type::PRI_STRING);
	REQUIRE(*reinterpret_cast<PrimativeAst*>(assignment->Data)->Value->String == L"deneme");

	tokinizer->Parse(L"erhan = \"deneme\"", tokens);
	astParser->Parse(tokens, asts);

	REQUIRE(asts.get()->size() == 1);
	assignment = reinterpret_cast<AssignmentAst*>(asts.get()->at(0));
	REQUIRE(assignment->Name == L"erhan");
	REQUIRE(assignment->Data != nullptr);
	REQUIRE(assignment->Data->GetType() == EASY_AST_TYPE::PRIMATIVE);
	REQUIRE(reinterpret_cast<PrimativeAst*>(assignment->Data)->Value->Type == PrimativeValue::Type::PRI_STRING);
	REQUIRE(*reinterpret_cast<PrimativeAst*>(assignment->Data)->Value->String == L"deneme");


	tokinizer->Parse(L"erhan = 0.15", tokens);
	astParser->Parse(tokens, asts);

	REQUIRE(asts.get()->size() == 1);
	assignment = reinterpret_cast<AssignmentAst*>(asts.get()->at(0));
	REQUIRE(assignment->Name == L"erhan");
	REQUIRE(assignment->Data != nullptr);
	REQUIRE(assignment->Data->GetType() == EASY_AST_TYPE::PRIMATIVE);
	REQUIRE(reinterpret_cast<PrimativeAst*>(assignment->Data)->Value->Type == PrimativeValue::Type::PRI_DOUBLE);
	REQUIRE(reinterpret_cast<PrimativeAst*>(assignment->Data)->Value->Double == 0.15);

	tokinizer->Parse(L"erhan = .15", tokens);
	astParser->Parse(tokens, asts);

	REQUIRE(asts.get()->size() == 1);
	assignment = reinterpret_cast<AssignmentAst*>(asts.get()->at(0));
	REQUIRE(assignment->Name == L"erhan");
	REQUIRE(assignment->Data != nullptr);
	REQUIRE(assignment->Data->GetType() == EASY_AST_TYPE::PRIMATIVE);
	REQUIRE(reinterpret_cast<PrimativeAst*>(assignment->Data)->Value->Type == PrimativeValue::Type::PRI_DOUBLE);
	REQUIRE(reinterpret_cast<PrimativeAst*>(assignment->Data)->Value->Double == 0.15);


	tokinizer->Parse(L"erhan = 50 + 5", tokens);
	astParser->Parse(tokens, asts);

	REQUIRE(asts.get()->size() == 1);

	assignment = reinterpret_cast<AssignmentAst*>(asts.get()->at(0));
	REQUIRE(assignment->Name == L"erhan");
	REQUIRE(assignment->Data != nullptr);
	REQUIRE(assignment->Data->GetType() == EASY_AST_TYPE::BINARY_OPERATION);
	REQUIRE(reinterpret_cast<BinaryAst*>(assignment->Data)->Left->GetType() == EASY_AST_TYPE::PRIMATIVE);
	REQUIRE(reinterpret_cast<BinaryAst*>(assignment->Data)->Right->GetType() == EASY_AST_TYPE::PRIMATIVE);
	REQUIRE(reinterpret_cast<BinaryAst*>(assignment->Data)->Op == EASY_OPERATOR_TYPE::PLUS);


	tokinizer->Parse(L"erhan = 50 / 5", tokens);
	astParser->Parse(tokens, asts);

	REQUIRE(asts.get()->size() == 1);

	assignment = reinterpret_cast<AssignmentAst*>(asts.get()->at(0));
	REQUIRE(assignment->Name == L"erhan");
	REQUIRE(assignment->Data != nullptr);
	REQUIRE(assignment->Data->GetType() == EASY_AST_TYPE::BINARY_OPERATION);
	REQUIRE(reinterpret_cast<BinaryAst*>(assignment->Data)->Left->GetType() == EASY_AST_TYPE::PRIMATIVE);
	REQUIRE(reinterpret_cast<BinaryAst*>(assignment->Data)->Right->GetType() == EASY_AST_TYPE::PRIMATIVE);
	REQUIRE(reinterpret_cast<BinaryAst*>(assignment->Data)->Op == EASY_OPERATOR_TYPE::DIVISION);
	REQUIRE(reinterpret_cast<PrimativeAst*>(reinterpret_cast<BinaryAst*>(assignment->Data)->Left)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
	REQUIRE(reinterpret_cast<PrimativeAst*>(reinterpret_cast<BinaryAst*>(assignment->Data)->Left)->Value->Integer == 50);
	REQUIRE(reinterpret_cast<PrimativeAst*>(reinterpret_cast<BinaryAst*>(assignment->Data)->Right)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
	REQUIRE(reinterpret_cast<PrimativeAst*>(reinterpret_cast<BinaryAst*>(assignment->Data)->Right)->Value->Integer == 5);


	tokinizer->Parse(L"erhan = 50 - 5", tokens);
	astParser->Parse(tokens, asts);

	REQUIRE(asts.get()->size() == 1);

	assignment = reinterpret_cast<AssignmentAst*>(asts.get()->at(0));
	REQUIRE(assignment->Name == L"erhan");
	REQUIRE(assignment->Data != nullptr);
	REQUIRE(assignment->Data->GetType() == EASY_AST_TYPE::BINARY_OPERATION);
	REQUIRE(reinterpret_cast<BinaryAst*>(assignment->Data)->Left->GetType() == EASY_AST_TYPE::PRIMATIVE);
	REQUIRE(reinterpret_cast<BinaryAst*>(assignment->Data)->Right->GetType() == EASY_AST_TYPE::PRIMATIVE);
	REQUIRE(reinterpret_cast<BinaryAst*>(assignment->Data)->Op == EASY_OPERATOR_TYPE::MINUS);

	REQUIRE(assignment->Data != nullptr);
}

TEST_CASE("Block test") {
	Tokinizer* tokinizer = new StandartTokinizer();
	AstParser* astParser = new AstParser();

	std::shared_ptr<std::vector<Token* > > tokens = make_shared<std::vector<Token* > >();
	std::shared_ptr<std::vector<Ast* > > asts = make_shared<std::vector<Ast* > >();

	tokinizer->Parse(L"{ erhan = 50 - 5 }", tokens);
	astParser->Parse(tokens, asts);
	REQUIRE(asts.get()->size() == 1);

	auto* block = reinterpret_cast<BlockAst*>(asts.get()->at(0));
	REQUIRE(block->Blocks->size() == 1);

	AssignmentAst* assignment = reinterpret_cast<AssignmentAst*>(block->Blocks->at(0));
	REQUIRE(assignment->Name == L"erhan");
	REQUIRE(assignment->Data != nullptr);
	REQUIRE(assignment->Data->GetType() == EASY_AST_TYPE::BINARY_OPERATION);
	REQUIRE(reinterpret_cast<BinaryAst*>(assignment->Data)->Left->GetType() == EASY_AST_TYPE::PRIMATIVE);
	REQUIRE(reinterpret_cast<BinaryAst*>(assignment->Data)->Right->GetType() == EASY_AST_TYPE::PRIMATIVE);
	REQUIRE(reinterpret_cast<BinaryAst*>(assignment->Data)->Op == EASY_OPERATOR_TYPE::MINUS);
	REQUIRE(reinterpret_cast<PrimativeAst*>(reinterpret_cast<BinaryAst*>(assignment->Data)->Left)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
	REQUIRE(reinterpret_cast<PrimativeAst*>(reinterpret_cast<BinaryAst*>(assignment->Data)->Left)->Value->Integer == 50);
	REQUIRE(reinterpret_cast<PrimativeAst*>(reinterpret_cast<BinaryAst*>(assignment->Data)->Right)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
	REQUIRE(reinterpret_cast<PrimativeAst*>(reinterpret_cast<BinaryAst*>(assignment->Data)->Right)->Value->Integer == 5);

	tokinizer->Parse(L"{ } { } { io::print 123 }", tokens);
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

	SECTION("if erhan > 15 then io::print 123") {
		tokinizer->Parse(L"if erhan > 15 then io::print 123", tokens);
		astParser->Parse(tokens, asts);
		REQUIRE(asts.get()->size() == 1);

		auto* ifStatement = reinterpret_cast<IfStatementAst*>(asts.get()->at(0));

		REQUIRE(ifStatement->True != nullptr);
		REQUIRE(ifStatement->False == nullptr);
		REQUIRE(ifStatement->ControlOpt != nullptr);
		REQUIRE(ifStatement->ControlOpt->GetType() == EASY_AST_TYPE::CONTROL_OPERATION);
		auto * controlOpt = reinterpret_cast<ControlAst*>(ifStatement->ControlOpt);
		REQUIRE(controlOpt->Left != nullptr);
		REQUIRE(controlOpt->Right != nullptr);
		REQUIRE(controlOpt->Op == EASY_OPERATOR_TYPE::GREATOR);
	}

	SECTION("if erhan > 15 then io::print 123 else io::print 321") {
		tokinizer->Parse(L"if erhan > 15 then io::print 123 else io::print 321", tokens);
		astParser->Parse(tokens, asts);
		REQUIRE(asts.get()->size() == 1);

		auto* ifStatement = reinterpret_cast<IfStatementAst*>(asts.get()->at(0));

		REQUIRE(ifStatement->True != nullptr);
		REQUIRE(ifStatement->False != nullptr);
		REQUIRE(ifStatement->ControlOpt != nullptr);
		REQUIRE(ifStatement->ControlOpt->GetType() == EASY_AST_TYPE::CONTROL_OPERATION);
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


	SECTION("io::print \"test\"") {
		tokinizer->Parse(L"io::print \"test\"", tokens);
		astParser->Parse(tokens, asts);
		REQUIRE(asts.get()->size() == 1);

		auto* functionCall = reinterpret_cast<FunctionCallAst*>(asts.get()->at(0));

		REQUIRE(functionCall->Function == L"print");
		REQUIRE(functionCall->Package == L"io");
		REQUIRE(functionCall->Args.size() == 1);
		REQUIRE(functionCall->Args.at(0)->GetType() == EASY_AST_TYPE::PRIMATIVE);
		REQUIRE(reinterpret_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->Type == PrimativeValue::Type::PRI_STRING);
		REQUIRE(*reinterpret_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->String == L"test");
	}

	SECTION("io::print 5.5") {
		tokinizer->Parse(L"io::print 5.5", tokens);
		astParser->Parse(tokens, asts);
		REQUIRE(asts.get()->size() == 1);

		auto* functionCall = reinterpret_cast<FunctionCallAst*>(asts.get()->at(0));

		REQUIRE(functionCall->Function == L"print");
		REQUIRE(functionCall->Package == L"io");
		REQUIRE(functionCall->Args.size() == 1);
		REQUIRE(functionCall->Args.at(0)->GetType() == EASY_AST_TYPE::PRIMATIVE);
		REQUIRE(reinterpret_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->Type == PrimativeValue::Type::PRI_DOUBLE);
		REQUIRE(reinterpret_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->Double == 5.5);
	}

	SECTION("io::print 123") {
		tokinizer->Parse(L"io::print 123", tokens);
		astParser->Parse(tokens, asts);
		REQUIRE(asts.get()->size() == 1);

		auto* functionCall = reinterpret_cast<FunctionCallAst*>(asts.get()->at(0));

		REQUIRE(functionCall->Function == L"print");
		REQUIRE(functionCall->Args.size() == 1);
		REQUIRE(functionCall->Args.at(0)->GetType() == EASY_AST_TYPE::PRIMATIVE);
		REQUIRE(reinterpret_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->Type == PrimativeValue::Type::PRI_INTEGER);
		REQUIRE(reinterpret_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->Integer == 123);
	}

	SECTION("io::print 10 + 20") {
		tokinizer->Parse(L"io::print 10 + 20", tokens);
		astParser->Parse(tokens, asts);
		REQUIRE(asts.get()->size() == 1);

		auto* functionCall = reinterpret_cast<FunctionCallAst*>(asts.get()->at(0));

		REQUIRE(functionCall->Function == L"print");
		REQUIRE(functionCall->Args.size() == 1);
		REQUIRE(functionCall->Args.at(0)->GetType() == EASY_AST_TYPE::BINARY_OPERATION);
	}

	SECTION("core::isDouble 5.5") {
		tokinizer->Parse(L"core::isDouble 5.5", tokens);
		astParser->Parse(tokens, asts);
		REQUIRE(asts.get()->size() == 1);

		auto* functionCall = reinterpret_cast<FunctionCallAst*>(asts.get()->at(0));

		REQUIRE(functionCall->Function == L"isDouble");
		REQUIRE(functionCall->Package == L"core");
		REQUIRE(functionCall->Args.size() == 1);
		REQUIRE(functionCall->Args.at(0)->GetType() == EASY_AST_TYPE::PRIMATIVE);
		REQUIRE(reinterpret_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->Type == PrimativeValue::Type::PRI_DOUBLE);
		REQUIRE(reinterpret_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->Double == 5.5);
	}
}



TEST_CASE("Bool ast test") {
    Tokinizer* tokinizer = new StandartTokinizer();
    AstParser* astParser = new AstParser();

    std::shared_ptr<std::vector<Token* > > tokens = make_shared<std::vector<Token* > >();
    std::shared_ptr<std::vector<Ast* > > asts = make_shared<std::vector<Ast* > >();


    SECTION("io::print true") {
        tokinizer->Parse(L"io::print true", tokens);
        astParser->Parse(tokens, asts);
        REQUIRE(asts.get()->size() == 1);

        auto* functionCall = reinterpret_cast<FunctionCallAst*>(asts.get()->at(0));

        REQUIRE(functionCall->Function == L"print");
		REQUIRE(functionCall->Package == L"io");
        REQUIRE(functionCall->Args.size() == 1);
        REQUIRE(functionCall->Args.at(0)->GetType() == EASY_AST_TYPE::PRIMATIVE);
        REQUIRE(reinterpret_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->Type == PrimativeValue::Type::PRI_BOOL);
        REQUIRE(reinterpret_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->Bool == true);
    }
    
    SECTION("io::print false") {
        tokinizer->Parse(L"io::print false", tokens);
        astParser->Parse(tokens, asts);
        REQUIRE(asts.get()->size() == 1);
        
        auto* functionCall = reinterpret_cast<FunctionCallAst*>(asts.get()->at(0));
        
        REQUIRE(functionCall->Function == L"print");
		REQUIRE(functionCall->Package == L"io");
        REQUIRE(functionCall->Args.size() == 1);
        REQUIRE(functionCall->Args.at(0)->GetType() == EASY_AST_TYPE::PRIMATIVE);
        REQUIRE(reinterpret_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->Type == PrimativeValue::Type::PRI_BOOL);
        REQUIRE(reinterpret_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->Bool == false);
    }
}

TEST_CASE( "Function asd test" ) {
    Tokinizer* tokinizer = new StandartTokinizer();
    AstParser* astParser = new AstParser();
    
    std::shared_ptr<std::vector<Token* > > tokens = make_shared<std::vector<Token* > >();
    std::shared_ptr<std::vector<Ast* > > asts = make_shared<std::vector<Ast* > >();
    
    SECTION( "func test () return 1" ) {
        tokinizer->Parse(L"func test () return 1", tokens);
        astParser->Parse(tokens, asts);
        
        REQUIRE(asts->size() == 1);
        REQUIRE(asts->at(0)->GetType() == EASY_AST_TYPE::FUNCTION_DECLERATION);
        
        auto* decl = reinterpret_cast<FunctionDefinetionAst*>(asts->at(0));
        REQUIRE(decl->Name == L"test");
        REQUIRE(decl->Args.size() == 0);
        REQUIRE(decl->Body != nullptr);
        REQUIRE(decl->Body->GetType() == EASY_AST_TYPE::RETURN);
        
        auto* ret = reinterpret_cast<ReturnAst*>(decl->Body);
        REQUIRE(ret->Data != nullptr);
        REQUIRE(ret->Data->GetType() == EASY_AST_TYPE::PRIMATIVE);
    }

	SECTION( "func test ( ) return 1" ) {
		tokinizer->Parse(L"func test () return 1", tokens);
		astParser->Parse(tokens, asts);

		REQUIRE(asts->size() == 1);
		REQUIRE(asts->at(0)->GetType() == EASY_AST_TYPE::FUNCTION_DECLERATION);

		auto* decl = reinterpret_cast<FunctionDefinetionAst*>(asts->at(0));
		REQUIRE(decl->Name == L"test");
		REQUIRE(decl->Args.size() == 0);
		REQUIRE(decl->Body != nullptr);
		REQUIRE(decl->Body->GetType() == EASY_AST_TYPE::RETURN);

		auto* ret = reinterpret_cast<ReturnAst*>(decl->Body);
		REQUIRE(ret->Data != nullptr);
		REQUIRE(ret->Data->GetType() == EASY_AST_TYPE::PRIMATIVE);
	}
    
    SECTION( "func test () { return 1 }" ) {
        tokinizer->Parse(L"func test () { return 1 }", tokens);
        astParser->Parse(tokens, asts);
        
        REQUIRE(asts->size() == 1);
        REQUIRE(asts->at(0)->GetType() == EASY_AST_TYPE::FUNCTION_DECLERATION);
        
        auto* decl = reinterpret_cast<FunctionDefinetionAst*>(asts->at(0));
        REQUIRE(decl->Name == L"test");
        REQUIRE(decl->Args.size() == 0);
        REQUIRE(decl->Body != nullptr);
        REQUIRE(decl->Body->GetType() == EASY_AST_TYPE::BLOCK);
        
        auto* block = reinterpret_cast<BlockAst*>(decl->Body);
        REQUIRE(block->Blocks->size() == 1);
        REQUIRE(block->Blocks->at(0)->GetType() == EASY_AST_TYPE::RETURN);
        
        auto* ret = reinterpret_cast<ReturnAst*>(block->Blocks->at(0));
        REQUIRE(ret->Data != nullptr);
        REQUIRE(ret->Data->GetType() == EASY_AST_TYPE::PRIMATIVE);
    }

	SECTION( "func test (data) { return data }" ) {
		tokinizer->Parse(L"func test (data) { return data }", tokens);
		astParser->Parse(tokens, asts);

		REQUIRE(asts->size() == 1);
		REQUIRE(asts->at(0)->GetType() == EASY_AST_TYPE::FUNCTION_DECLERATION);

		auto* decl = reinterpret_cast<FunctionDefinetionAst*>(asts->at(0));
		REQUIRE(decl->Name == L"test");
		REQUIRE(decl->Args.size() == 1);
		REQUIRE(decl->Args.at(0) == L"data");
		REQUIRE(decl->Body != nullptr);
		REQUIRE(decl->Body->GetType() == EASY_AST_TYPE::BLOCK);

		auto* block = reinterpret_cast<BlockAst*>(decl->Body);
		REQUIRE(block->Blocks->size() == 1);
		REQUIRE(block->Blocks->at(0)->GetType() == EASY_AST_TYPE::RETURN);

		auto* ret = reinterpret_cast<ReturnAst*>(block->Blocks->at(0));
		REQUIRE(ret->Data != nullptr);
		REQUIRE(ret->Data->GetType() == EASY_AST_TYPE::VARIABLE);
	}
}

#endif
