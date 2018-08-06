#ifndef EASYLANG_ASTSTESTS_H
#define EASYLANG_ASTSTESTS_H

#define CATCH_CONFIG_RUNNER
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cmath>

#include "../include/Catch.h"
#include "../include/Lexer.h"
#include "../include/ASTs.h"

using namespace std;

TEST_CASE("Base test") {
	Tokinizer* tokinizer = new StandartTokinizer();
	AstParser* astParser = new AstParser();
	std::shared_ptr<std::vector<Token* > > tokens = make_shared<std::vector<Token* > >();
	std::shared_ptr<std::vector<Ast* > > asts = make_shared<std::vector<Ast* > >();


	SECTION("50 + 100") {
		tokinizer->Parse(_T("50 + 100"), tokens);
		astParser->Parse(tokens, asts);

		REQUIRE(asts.get()->size() == 1);

		REQUIRE(asts.get()->at(0)->GetType() == EASY_AST_TYPE::EXPR_STATEMENT);
		ExprStatementAst* stmt = static_cast<ExprStatementAst*>(asts.get()->at(0));
		BinaryAst* binary = static_cast<BinaryAst*>(stmt->Expr);
		REQUIRE(binary->Left != nullptr);
		REQUIRE(binary->Right != nullptr);
		REQUIRE(binary->Op == EASY_OPERATOR_TYPE::PLUS);
		REQUIRE(static_cast<PrimativeAst*>(binary->Left)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
		REQUIRE(static_cast<PrimativeAst*>(binary->Left)->Value->Integer == 50);
		REQUIRE(static_cast<PrimativeAst*>(binary->Right)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
		REQUIRE(static_cast<PrimativeAst*>(binary->Right)->Value->Integer == 100);
	}


	SECTION("[ ]") {
		tokinizer->Parse(_T("[ ]"), tokens);
		astParser->Parse(tokens, asts);

		REQUIRE(asts.get()->size() == 1);

		PrimativeAst* primative = static_cast<PrimativeAst*>(asts.get()->at(0));
		REQUIRE(primative != nullptr);
	}

	SECTION("50 - 100") {
		tokinizer->Parse(_T("50 - 100"), tokens);
		astParser->Parse(tokens, asts);

		REQUIRE(asts.get()->size() == 1);
		REQUIRE(asts.get()->at(0)->GetType() == EASY_AST_TYPE::EXPR_STATEMENT);
		ExprStatementAst* stmt = static_cast<ExprStatementAst*>(asts.get()->at(0));

		BinaryAst* binary = static_cast<BinaryAst*>(stmt->Expr);
		REQUIRE(binary->Left != nullptr);
		REQUIRE(binary->Right != nullptr);
		REQUIRE(binary->Op == EASY_OPERATOR_TYPE::MINUS);
		REQUIRE(static_cast<PrimativeAst*>(binary->Left)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
		REQUIRE(static_cast<PrimativeAst*>(binary->Left)->Value->Integer == 50);
		REQUIRE(static_cast<PrimativeAst*>(binary->Right)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
		REQUIRE(static_cast<PrimativeAst*>(binary->Right)->Value->Integer == 100);
	}

	SECTION("(50 - 100)") {
		tokinizer->Parse(_T("50 - 100"), tokens);
		astParser->Parse(tokens, asts);

		REQUIRE(asts.get()->size() == 1);
		REQUIRE(asts.get()->at(0)->GetType() == EASY_AST_TYPE::EXPR_STATEMENT);
		ExprStatementAst* stmt = static_cast<ExprStatementAst*>(asts.get()->at(0));

		BinaryAst* binary = static_cast<BinaryAst*>(stmt->Expr);
		REQUIRE(binary->Left != nullptr);
		REQUIRE(binary->Right != nullptr);
		REQUIRE(binary->Op == EASY_OPERATOR_TYPE::MINUS);
		REQUIRE(static_cast<PrimativeAst*>(binary->Left)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
		REQUIRE(static_cast<PrimativeAst*>(binary->Left)->Value->Integer == 50);
		REQUIRE(static_cast<PrimativeAst*>(binary->Right)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
		REQUIRE(static_cast<PrimativeAst*>(binary->Right)->Value->Integer == 100);
	}

	SECTION("(50 - (10 * 10))") {
		tokinizer->Parse(_T("(50 - (10 * 10))"), tokens);
		astParser->Parse(tokens, asts);

		REQUIRE(asts.get()->size() == 1);
		REQUIRE(asts.get()->at(0)->GetType() == EASY_AST_TYPE::EXPR_STATEMENT);
		ExprStatementAst* stmt = static_cast<ExprStatementAst*>(asts.get()->at(0));

		BinaryAst* binary = static_cast<BinaryAst*>(stmt->Expr);
		REQUIRE(binary->Left != nullptr);
		REQUIRE(binary->Right != nullptr);
		REQUIRE(binary->Op == EASY_OPERATOR_TYPE::MINUS);
		REQUIRE(static_cast<PrimativeAst*>(binary->Left)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
		REQUIRE(static_cast<PrimativeAst*>(binary->Left)->Value->Integer == 50);
		REQUIRE(binary->Right->GetType() == EASY_AST_TYPE::BINARY_OPERATION);
	}

	SECTION("((50 - 10) - 10)") {
		tokinizer->Parse(_T("((50 - 10) - 10)"), tokens);
		astParser->Parse(tokens, asts);

		REQUIRE(asts.get()->size() == 1);
		REQUIRE(asts.get()->at(0)->GetType() == EASY_AST_TYPE::EXPR_STATEMENT);
		ExprStatementAst* stmt = static_cast<ExprStatementAst*>(asts.get()->at(0));

		BinaryAst* binary = static_cast<BinaryAst*>(stmt->Expr);
		REQUIRE(binary->Left != nullptr);
		REQUIRE(binary->Right != nullptr);
		REQUIRE(binary->Op == EASY_OPERATOR_TYPE::MINUS);
		REQUIRE(binary->Left->GetType() == EASY_AST_TYPE::BINARY_OPERATION);
		REQUIRE(static_cast<PrimativeAst*>(binary->Right)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
	}

	SECTION("((50 - 10) - (10 * 10))") {
		tokinizer->Parse(_T("((50 - 10) - (10 * 10))"), tokens);
		astParser->Parse(tokens, asts);

		REQUIRE(asts.get()->size() == 1);
		REQUIRE(asts.get()->at(0)->GetType() == EASY_AST_TYPE::EXPR_STATEMENT);
		ExprStatementAst* stmt = static_cast<ExprStatementAst*>(asts.get()->at(0));

		BinaryAst* binary = static_cast<BinaryAst*>(stmt->Expr);
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
	tokinizer->Parse(_T("erhan = 50"), tokens);
	astParser->Parse(tokens, asts);

	REQUIRE(asts.get()->size() == 1);
	REQUIRE(asts.get()->at(0)->GetType() == EASY_AST_TYPE::EXPR_STATEMENT);
	ExprStatementAst* stmt = static_cast<ExprStatementAst*>(asts.get()->at(0));

	AssignmentAst* assignment = static_cast<AssignmentAst*>(stmt->Expr);
	REQUIRE(assignment->Name == _T("erhan"));
	REQUIRE(assignment->Data != nullptr);
	REQUIRE(assignment->Data->GetType() == EASY_AST_TYPE::PRIMATIVE);
	REQUIRE(static_cast<PrimativeAst*>(assignment->Data)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
	REQUIRE(static_cast<PrimativeAst*>(assignment->Data)->Value->Integer == 50);

	REQUIRE(assignment->Data != nullptr);

	tokinizer->Parse(_T("erhan = 150"), tokens);
	astParser->Parse(tokens, asts);

	REQUIRE(asts.get()->size() == 1);
	REQUIRE(asts.get()->at(0)->GetType() == EASY_AST_TYPE::EXPR_STATEMENT);
	stmt = static_cast<ExprStatementAst*>(asts.get()->at(0));

	assignment = static_cast<AssignmentAst*>(stmt->Expr);
	REQUIRE(assignment->Name == _T("erhan"));
	REQUIRE(assignment->Data != nullptr);
	REQUIRE(assignment->Data->GetType() == EASY_AST_TYPE::PRIMATIVE);
	REQUIRE(static_cast<PrimativeAst*>(assignment->Data)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
	REQUIRE(static_cast<PrimativeAst*>(assignment->Data)->Value->Integer == 150);

	tokinizer->Parse(_T("erhan = \"deneme\""), tokens);
	astParser->Parse(tokens, asts);

	REQUIRE(asts.get()->size() == 1);
	REQUIRE(asts.get()->at(0)->GetType() == EASY_AST_TYPE::EXPR_STATEMENT);
	stmt = static_cast<ExprStatementAst*>(asts.get()->at(0));

	assignment = static_cast<AssignmentAst*>(stmt->Expr);
	REQUIRE(assignment->Name == _T("erhan"));
	REQUIRE(assignment->Data != nullptr);
	REQUIRE(assignment->Data->GetType() == EASY_AST_TYPE::PRIMATIVE);
	REQUIRE(static_cast<PrimativeAst*>(assignment->Data)->Value->Type == PrimativeValue::Type::PRI_STRING);
	REQUIRE(*static_cast<PrimativeAst*>(assignment->Data)->Value->String == _T("deneme"));

	tokinizer->Parse(_T("erhan = \"deneme\""), tokens);
	astParser->Parse(tokens, asts);

	REQUIRE(asts.get()->size() == 1);
	REQUIRE(asts.get()->at(0)->GetType() == EASY_AST_TYPE::EXPR_STATEMENT);
	stmt = static_cast<ExprStatementAst*>(asts.get()->at(0));

	assignment = static_cast<AssignmentAst*>(stmt->Expr);
	REQUIRE(assignment->Name == _T("erhan"));
	REQUIRE(assignment->Data != nullptr);
	REQUIRE(assignment->Data->GetType() == EASY_AST_TYPE::PRIMATIVE);
	REQUIRE(static_cast<PrimativeAst*>(assignment->Data)->Value->Type == PrimativeValue::Type::PRI_STRING);
	REQUIRE(*static_cast<PrimativeAst*>(assignment->Data)->Value->String == _T("deneme"));


	tokinizer->Parse(_T("erhan = 0.15"), tokens);
	astParser->Parse(tokens, asts);

	REQUIRE(asts.get()->size() == 1);
	REQUIRE(asts.get()->at(0)->GetType() == EASY_AST_TYPE::EXPR_STATEMENT);
	stmt = static_cast<ExprStatementAst*>(asts.get()->at(0));

	assignment = static_cast<AssignmentAst*>(stmt->Expr);
	REQUIRE(assignment->Name == _T("erhan"));
	REQUIRE(assignment->Data != nullptr);
	REQUIRE(assignment->Data->GetType() == EASY_AST_TYPE::PRIMATIVE);
	REQUIRE(static_cast<PrimativeAst*>(assignment->Data)->Value->Type == PrimativeValue::Type::PRI_DOUBLE);
	REQUIRE(static_cast<PrimativeAst*>(assignment->Data)->Value->Double == 0.15);

	tokinizer->Parse(_T("erhan = .15"), tokens);
	astParser->Parse(tokens, asts);

	REQUIRE(asts.get()->size() == 1);
	REQUIRE(asts.get()->at(0)->GetType() == EASY_AST_TYPE::EXPR_STATEMENT);
	stmt = static_cast<ExprStatementAst*>(asts.get()->at(0));

	assignment = static_cast<AssignmentAst*>(stmt->Expr);
	REQUIRE(assignment->Name == _T("erhan"));
	REQUIRE(assignment->Data != nullptr);
	REQUIRE(assignment->Data->GetType() == EASY_AST_TYPE::PRIMATIVE);
	REQUIRE(static_cast<PrimativeAst*>(assignment->Data)->Value->Type == PrimativeValue::Type::PRI_DOUBLE);
	REQUIRE(static_cast<PrimativeAst*>(assignment->Data)->Value->Double == 0.15);


	tokinizer->Parse(_T("erhan = 50 + 5"), tokens);
	astParser->Parse(tokens, asts);

	REQUIRE(asts.get()->size() == 1);
	REQUIRE(asts.get()->at(0)->GetType() == EASY_AST_TYPE::EXPR_STATEMENT);
	stmt = static_cast<ExprStatementAst*>(asts.get()->at(0));

	assignment = static_cast<AssignmentAst*>(stmt->Expr);
	REQUIRE(assignment->Name == _T("erhan"));
	REQUIRE(assignment->Data != nullptr);
	REQUIRE(assignment->Data->GetType() == EASY_AST_TYPE::BINARY_OPERATION);
	REQUIRE(static_cast<BinaryAst*>(assignment->Data)->Left->GetType() == EASY_AST_TYPE::PRIMATIVE);
	REQUIRE(static_cast<BinaryAst*>(assignment->Data)->Right->GetType() == EASY_AST_TYPE::PRIMATIVE);
	REQUIRE(static_cast<BinaryAst*>(assignment->Data)->Op == EASY_OPERATOR_TYPE::PLUS);


	tokinizer->Parse(_T("erhan = 50 / 5"), tokens);
	astParser->Parse(tokens, asts);

	REQUIRE(asts.get()->size() == 1);
	REQUIRE(asts.get()->at(0)->GetType() == EASY_AST_TYPE::EXPR_STATEMENT);
	stmt = static_cast<ExprStatementAst*>(asts.get()->at(0));

	assignment = static_cast<AssignmentAst*>(stmt->Expr);
	REQUIRE(assignment->Name == _T("erhan"));
	REQUIRE(assignment->Data != nullptr);
	REQUIRE(assignment->Data->GetType() == EASY_AST_TYPE::BINARY_OPERATION);
	REQUIRE(static_cast<BinaryAst*>(assignment->Data)->Left->GetType() == EASY_AST_TYPE::PRIMATIVE);
	REQUIRE(static_cast<BinaryAst*>(assignment->Data)->Right->GetType() == EASY_AST_TYPE::PRIMATIVE);
	REQUIRE(static_cast<BinaryAst*>(assignment->Data)->Op == EASY_OPERATOR_TYPE::DIVISION);
	REQUIRE(static_cast<PrimativeAst*>(static_cast<BinaryAst*>(assignment->Data)->Left)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
	REQUIRE(static_cast<PrimativeAst*>(static_cast<BinaryAst*>(assignment->Data)->Left)->Value->Integer == 50);
	REQUIRE(static_cast<PrimativeAst*>(static_cast<BinaryAst*>(assignment->Data)->Right)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
	REQUIRE(static_cast<PrimativeAst*>(static_cast<BinaryAst*>(assignment->Data)->Right)->Value->Integer == 5);


	tokinizer->Parse(_T("erhan = 50 - 5"), tokens);
	astParser->Parse(tokens, asts);

	REQUIRE(asts.get()->size() == 1);
	REQUIRE(asts.get()->at(0)->GetType() == EASY_AST_TYPE::EXPR_STATEMENT);
	stmt = static_cast<ExprStatementAst*>(asts.get()->at(0));

	assignment = static_cast<AssignmentAst*>(stmt->Expr);
	REQUIRE(assignment->Name == _T("erhan"));
	REQUIRE(assignment->Data != nullptr);
	REQUIRE(assignment->Data->GetType() == EASY_AST_TYPE::BINARY_OPERATION);
	REQUIRE(static_cast<BinaryAst*>(assignment->Data)->Left->GetType() == EASY_AST_TYPE::PRIMATIVE);
	REQUIRE(static_cast<BinaryAst*>(assignment->Data)->Right->GetType() == EASY_AST_TYPE::PRIMATIVE);
	REQUIRE(static_cast<BinaryAst*>(assignment->Data)->Op == EASY_OPERATOR_TYPE::MINUS);

	REQUIRE(assignment->Data != nullptr);
}

TEST_CASE("Block test") {
	Tokinizer* tokinizer = new StandartTokinizer();
	AstParser* astParser = new AstParser();

	std::shared_ptr<std::vector<Token* > > tokens = make_shared<std::vector<Token* > >();
	std::shared_ptr<std::vector<Ast* > > asts = make_shared<std::vector<Ast* > >();

	tokinizer->Parse(_T("{ erhan = 50 - 5 }"), tokens);
	astParser->Parse(tokens, asts);
	REQUIRE(asts.get()->size() == 1);

	auto* block = static_cast<BlockAst*>(asts.get()->at(0));
	REQUIRE(block->Blocks.size() == 1);
	
	REQUIRE(block->Blocks.at(0)->GetType() == EASY_AST_TYPE::EXPR_STATEMENT);
	auto* stmt = static_cast<ExprStatementAst*>(block->Blocks.at(0));

	AssignmentAst* assignment = static_cast<AssignmentAst*>(stmt->Expr);
	REQUIRE(assignment->Name == _T("erhan"));
	REQUIRE(assignment->Data != nullptr);
	REQUIRE(assignment->Data->GetType() == EASY_AST_TYPE::BINARY_OPERATION);
	REQUIRE(static_cast<BinaryAst*>(assignment->Data)->Left->GetType() == EASY_AST_TYPE::PRIMATIVE);
	REQUIRE(static_cast<BinaryAst*>(assignment->Data)->Right->GetType() == EASY_AST_TYPE::PRIMATIVE);
	REQUIRE(static_cast<BinaryAst*>(assignment->Data)->Op == EASY_OPERATOR_TYPE::MINUS);
	REQUIRE(static_cast<PrimativeAst*>(static_cast<BinaryAst*>(assignment->Data)->Left)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
	REQUIRE(static_cast<PrimativeAst*>(static_cast<BinaryAst*>(assignment->Data)->Left)->Value->Integer == 50);
	REQUIRE(static_cast<PrimativeAst*>(static_cast<BinaryAst*>(assignment->Data)->Right)->Value->Type == PrimativeValue::Type::PRI_INTEGER);
	REQUIRE(static_cast<PrimativeAst*>(static_cast<BinaryAst*>(assignment->Data)->Right)->Value->Integer == 5);

	tokinizer->Parse(_T("{ } { } { io::print (123) }"), tokens);
	astParser->Parse(tokens, asts);
	REQUIRE(asts.get()->size() == 3);

	REQUIRE(static_cast<BlockAst*>(asts.get()->at(0))->Blocks.size() == 0);
	REQUIRE(static_cast<BlockAst*>(asts.get()->at(1))->Blocks.size() == 0);
	REQUIRE(static_cast<BlockAst*>(asts.get()->at(2))->Blocks.size() == 1);
}

TEST_CASE("loop test 1") {
	Tokinizer* tokinizer = new StandartTokinizer();
	AstParser* astParser = new AstParser();

	std::shared_ptr<std::vector<Token* > > tokens = make_shared<std::vector<Token* > >();
	std::shared_ptr<std::vector<Ast* > > asts = make_shared<std::vector<Ast* > >();

	tokinizer->Parse(_T("for i in 1 to 10 then core::print (i)"), tokens);
	astParser->Parse(tokens, asts);
	REQUIRE(asts.get()->size() == 1);
}

TEST_CASE("If test") {
	Tokinizer* tokinizer = new StandartTokinizer();
	AstParser* astParser = new AstParser();

	std::shared_ptr<std::vector<Token* > > tokens = make_shared<std::vector<Token* > >();
	std::shared_ptr<std::vector<Ast* > > asts = make_shared<std::vector<Ast* > >();

	SECTION("if erhan > 15 then io::print (123)") {
		tokinizer->Parse(_T("if erhan > 15 then io::print (123)"), tokens);
		astParser->Parse(tokens, asts);
		REQUIRE(asts.get()->size() == 1);

		auto* ifStatement = static_cast<IfStatementAst*>(asts.get()->at(0));

		REQUIRE(ifStatement->True != nullptr);
		REQUIRE(ifStatement->False == nullptr);
		REQUIRE(ifStatement->ControlOpt != nullptr);
		REQUIRE(ifStatement->ControlOpt->GetType() == EASY_AST_TYPE::CONTROL_OPERATION);
		auto * controlOpt = static_cast<ControlAst*>(ifStatement->ControlOpt);
		REQUIRE(controlOpt->Left != nullptr);
		REQUIRE(controlOpt->Right != nullptr);
		REQUIRE(controlOpt->Op == EASY_OPERATOR_TYPE::GREATOR);
	}

	SECTION("if erhan > 15 then io::print (123) else io::print (321)") {
		tokinizer->Parse(_T("if erhan > 15 then io::print (123) else io::print (321)"), tokens);
		astParser->Parse(tokens, asts);
		REQUIRE(asts.get()->size() == 1);

		auto* ifStatement = static_cast<IfStatementAst*>(asts.get()->at(0));

		REQUIRE(ifStatement->True != nullptr);
		REQUIRE(ifStatement->False != nullptr);
		REQUIRE(ifStatement->ControlOpt != nullptr);
		REQUIRE(ifStatement->ControlOpt->GetType() == EASY_AST_TYPE::CONTROL_OPERATION);
		auto * controlOpt = static_cast<ControlAst*>(ifStatement->ControlOpt);
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


	SECTION("io::print (\"test\")") {
		tokinizer->Parse(_T("io::print (\"test\")"), tokens);
		astParser->Parse(tokens, asts);
		REQUIRE(asts.get()->size() == 1);

		REQUIRE(asts.get()->at(0)->GetType() == EASY_AST_TYPE::EXPR_STATEMENT);
		ExprStatementAst* stmt = static_cast<ExprStatementAst*>(asts.get()->at(0));
		auto* functionCall = static_cast<FunctionCallAst*>(stmt->Expr);

		REQUIRE(functionCall->Function == _T("print"));
		REQUIRE(functionCall->Package == _T("io"));
		REQUIRE(functionCall->Args.size() == 1);
		REQUIRE(functionCall->Args.at(0)->GetType() == EASY_AST_TYPE::PRIMATIVE);
		REQUIRE(static_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->Type == PrimativeValue::Type::PRI_STRING);
		REQUIRE(*static_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->String == _T("test"));
	}

	SECTION("io::print (5.5)") {
		tokinizer->Parse(_T("io::print (5.5)"), tokens);
		astParser->Parse(tokens, asts);
		REQUIRE(asts.get()->size() == 1);
		REQUIRE(asts.get()->at(0)->GetType() == EASY_AST_TYPE::EXPR_STATEMENT);
		ExprStatementAst* stmt = static_cast<ExprStatementAst*>(asts.get()->at(0));

		auto* functionCall = static_cast<FunctionCallAst*>(stmt->Expr);

		REQUIRE(functionCall->Function == _T("print"));
		REQUIRE(functionCall->Package == _T("io"));
		REQUIRE(functionCall->Args.size() == 1);
		REQUIRE(functionCall->Args.at(0)->GetType() == EASY_AST_TYPE::PRIMATIVE);
		REQUIRE(static_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->Type == PrimativeValue::Type::PRI_DOUBLE);
		REQUIRE(static_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->Double == 5.5);
	}

	SECTION("io::print (123)") {
		tokinizer->Parse(_T("io::print (123)"), tokens);
		astParser->Parse(tokens, asts);
		REQUIRE(asts.get()->size() == 1);
		REQUIRE(asts.get()->at(0)->GetType() == EASY_AST_TYPE::EXPR_STATEMENT);
		ExprStatementAst* stmt = static_cast<ExprStatementAst*>(asts.get()->at(0));
		auto* functionCall = static_cast<FunctionCallAst*>(stmt->Expr);

		REQUIRE(functionCall->Function == _T("print"));
		REQUIRE(functionCall->Args.size() == 1);
		REQUIRE(functionCall->Args.at(0)->GetType() == EASY_AST_TYPE::PRIMATIVE);
		REQUIRE(static_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->Type == PrimativeValue::Type::PRI_INTEGER);
		REQUIRE(static_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->Integer == 123);
	}

	SECTION("io::print (10 + 20)") {
		tokinizer->Parse(_T("io::print (10 + 20)"), tokens);
		astParser->Parse(tokens, asts);
		REQUIRE(asts.get()->size() == 1);
		REQUIRE(asts.get()->at(0)->GetType() == EASY_AST_TYPE::EXPR_STATEMENT);
		ExprStatementAst* stmt = static_cast<ExprStatementAst*>(asts.get()->at(0));
		auto* functionCall = static_cast<FunctionCallAst*>(stmt->Expr);

		REQUIRE(functionCall->Function == _T("print"));
		REQUIRE(functionCall->Args.size() == 1);
		REQUIRE(functionCall->Args.at(0)->GetType() == EASY_AST_TYPE::BINARY_OPERATION);
	}

	SECTION("core::isDouble (5.5)") {
		tokinizer->Parse(_T("core::isDouble (5.5)"), tokens);
		astParser->Parse(tokens, asts);
		REQUIRE(asts.get()->size() == 1);
		REQUIRE(asts.get()->at(0)->GetType() == EASY_AST_TYPE::EXPR_STATEMENT);
		ExprStatementAst* stmt = static_cast<ExprStatementAst*>(asts.get()->at(0));
		auto* functionCall = static_cast<FunctionCallAst*>(stmt->Expr);

		REQUIRE(functionCall->Function == _T("isDouble"));
		REQUIRE(functionCall->Package == _T("core"));
		REQUIRE(functionCall->Args.size() == 1);
		REQUIRE(functionCall->Args.at(0)->GetType() == EASY_AST_TYPE::PRIMATIVE);
		REQUIRE(static_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->Type == PrimativeValue::Type::PRI_DOUBLE);
		REQUIRE(static_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->Double == 5.5);
	}
}



TEST_CASE("Bool ast test") {
    Tokinizer* tokinizer = new StandartTokinizer();
    AstParser* astParser = new AstParser();

    std::shared_ptr<std::vector<Token* > > tokens = make_shared<std::vector<Token* > >();
    std::shared_ptr<std::vector<Ast* > > asts = make_shared<std::vector<Ast* > >();


    SECTION("io::print (true)") {
        tokinizer->Parse(_T("io::print (true)"), tokens);
        astParser->Parse(tokens, asts);
        REQUIRE(asts.get()->size() == 1);
		REQUIRE(asts.get()->at(0)->GetType() == EASY_AST_TYPE::EXPR_STATEMENT);
		ExprStatementAst* stmt = static_cast<ExprStatementAst*>(asts.get()->at(0));
        auto* functionCall = static_cast<FunctionCallAst*>(stmt->Expr);

        REQUIRE(functionCall->Function == _T("print"));
		REQUIRE(functionCall->Package == _T("io"));
        REQUIRE(functionCall->Args.size() == 1);
        REQUIRE(functionCall->Args.at(0)->GetType() == EASY_AST_TYPE::PRIMATIVE);
        REQUIRE(static_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->Type == PrimativeValue::Type::PRI_BOOL);
        REQUIRE(static_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->Bool == true);
    }
    
    SECTION("io::print (false)") {
        tokinizer->Parse(_T("io::print (false)"), tokens);
        astParser->Parse(tokens, asts);
        REQUIRE(asts.get()->size() == 1);
		REQUIRE(asts.get()->at(0)->GetType() == EASY_AST_TYPE::EXPR_STATEMENT);
		ExprStatementAst* stmt = static_cast<ExprStatementAst*>(asts.get()->at(0));
        auto* functionCall = static_cast<FunctionCallAst*>(stmt->Expr);
        
        REQUIRE(functionCall->Function == _T("print"));
		REQUIRE(functionCall->Package == _T("io"));
        REQUIRE(functionCall->Args.size() == 1);
        REQUIRE(functionCall->Args.at(0)->GetType() == EASY_AST_TYPE::PRIMATIVE);
        REQUIRE(static_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->Type == PrimativeValue::Type::PRI_BOOL);
        REQUIRE(static_cast<PrimativeAst*>(functionCall->Args.at(0))->Value->Bool == false);
    }
}

TEST_CASE( "Function asd test" ) {
    Tokinizer* tokinizer = new StandartTokinizer();
    AstParser* astParser = new AstParser();
    
    std::shared_ptr<std::vector<Token* > > tokens = make_shared<std::vector<Token* > >();
    std::shared_ptr<std::vector<Ast* > > asts = make_shared<std::vector<Ast* > >();
    
    SECTION( "func test () return 1" ) {
        tokinizer->Parse(_T("func test () return 1"), tokens);
        astParser->Parse(tokens, asts);
        
        REQUIRE(asts->size() == 1);
        REQUIRE(asts->at(0)->GetType() == EASY_AST_TYPE::FUNCTION_DECLERATION);
        
        auto* decl = static_cast<FunctionDefinetionAst*>(asts->at(0));
        REQUIRE(decl->Name == _T("test"));
        REQUIRE(decl->Args.size() == 0);
        REQUIRE(decl->Body != nullptr);
        REQUIRE(decl->Body->GetType() == EASY_AST_TYPE::RETURN);
        
        auto* ret = static_cast<ReturnAst*>(decl->Body);
        REQUIRE(ret->Data != nullptr);
        REQUIRE(ret->Data->GetType() == EASY_AST_TYPE::PRIMATIVE);
    }

	SECTION( "func test ( ) return 1" ) {
		tokinizer->Parse(_T("func test () return 1"), tokens);
		astParser->Parse(tokens, asts);

		REQUIRE(asts->size() == 1);
		REQUIRE(asts->at(0)->GetType() == EASY_AST_TYPE::FUNCTION_DECLERATION);

		auto* decl = static_cast<FunctionDefinetionAst*>(asts->at(0));
		REQUIRE(decl->Name == _T("test"));
		REQUIRE(decl->Args.size() == 0);
		REQUIRE(decl->Body != nullptr);
		REQUIRE(decl->Body->GetType() == EASY_AST_TYPE::RETURN);

		auto* ret = static_cast<ReturnAst*>(decl->Body);
		REQUIRE(ret->Data != nullptr);
		REQUIRE(ret->Data->GetType() == EASY_AST_TYPE::PRIMATIVE);
	}
    
    SECTION( "func test ()  { return 1 }" ) {
        tokinizer->Parse(_T("func test ()  { return 1 }"), tokens);
        astParser->Parse(tokens, asts);
        
        REQUIRE(asts->size() == 1);
        REQUIRE(asts->at(0)->GetType() == EASY_AST_TYPE::FUNCTION_DECLERATION);
        
        auto* decl = static_cast<FunctionDefinetionAst*>(asts->at(0));
        REQUIRE(decl->Name == _T("test"));
        REQUIRE(decl->Args.size() == 0);
        REQUIRE(decl->Body != nullptr);
        REQUIRE(decl->Body->GetType() == EASY_AST_TYPE::BLOCK);
        
        auto* block = static_cast<BlockAst*>(decl->Body);
        REQUIRE(block->Blocks.size() == 1);
        REQUIRE(block->Blocks.at(0)->GetType() == EASY_AST_TYPE::RETURN);
        
        auto* ret = static_cast<ReturnAst*>(block->Blocks.at(0));
        REQUIRE(ret->Data != nullptr);
        REQUIRE(ret->Data->GetType() == EASY_AST_TYPE::PRIMATIVE);
    }

	SECTION( "func test (data) { return data }" ) {
		tokinizer->Parse(_T("func test (data) { return data }"), tokens);
		astParser->Parse(tokens, asts);

		REQUIRE(asts->size() == 1);
		REQUIRE(asts->at(0)->GetType() == EASY_AST_TYPE::FUNCTION_DECLERATION);

		auto* decl = static_cast<FunctionDefinetionAst*>(asts->at(0));
		REQUIRE(decl->Name == _T("test"));
		REQUIRE(decl->Args.size() == 1);
		REQUIRE(decl->Args.at(0)->Name == _T("data"));
		REQUIRE(decl->Body != nullptr);
		REQUIRE(decl->Body->GetType() == EASY_AST_TYPE::BLOCK);

		auto* block = static_cast<BlockAst*>(decl->Body);
		REQUIRE(block->Blocks.size() == 1);
		REQUIRE(block->Blocks.at(0)->GetType() == EASY_AST_TYPE::RETURN);

		auto* ret = static_cast<ReturnAst*>(block->Blocks.at(0));
		REQUIRE(ret->Data != nullptr);
		REQUIRE(ret->Data->GetType() == EASY_AST_TYPE::VARIABLE);
	}
}

#endif
