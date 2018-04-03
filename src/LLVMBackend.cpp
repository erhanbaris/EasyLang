#if defined(EASYLANG_JIT_ACTIVE)
#define NOMINMAX
#include <fstream>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <limits>

#include "Exceptions.h"
#include "LLVMBackend.h"
#include "System.h"

#include "llvm/ADT/APInt.h"
#include "llvm/IR/Verifier.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

class LLVMBackendImpl
{
public:
	Module* module;
	LLVMContext context;

	LLVMBackendImpl()
	{
		//LLVM
		InitializeNativeTarget();
		InitializeNativeTargetAsmPrinter();

		module = new Module("SYSTEM", context);
		LLVMContext Context;
	}

	~LLVMBackendImpl()
	{
		delete module;
	}
};



void LLVMBackend::Prepare(std::shared_ptr<std::vector<Ast*>> pAsts)
{
	temporaryAsts.clear();
	temporaryAsts.insert(temporaryAsts.end(), pAsts.get()->begin(), pAsts.get()->end());
}

PrimativeValue* LLVMBackend::getPrimative(Ast* ast)
{
	auto* primative = static_cast<PrimativeAst*>(ast)->Value;
	return primative;
}

tmp::Type LLVMBackend::operationResultType(tmp::Type from, tmp::Type to)
{
	if (from == tmp::Type::INT && to == tmp::Type::DOUBLE)
		return tmp::Type::DOUBLE;
	else if (to == tmp::Type::INT && from == tmp::Type::DOUBLE)
		return tmp::Type::DOUBLE;
	else if (to == tmp::Type::INT && from == tmp::Type::INT)
		return tmp::Type::INT;
	else if (from == tmp::Type::INT && to == tmp::Type::BOOL)
		return tmp::Type::INT;
	else if (to == tmp::Type::INT && from == tmp::Type::BOOL)
		return tmp::Type::INT;
	else if (to == tmp::Type::BOOL && from == tmp::Type::BOOL)
		return tmp::Type::BOOL;
	else if (from == tmp::Type::DOUBLE && to == tmp::Type::BOOL)
		return tmp::Type::DOUBLE;
	else if (to == tmp::Type::DOUBLE && from == tmp::Type::BOOL)
		return tmp::Type::DOUBLE;
	else if (to == tmp::Type::DOUBLE && from == tmp::Type::DOUBLE)
		return tmp::Type::DOUBLE;
}

PrimativeValue* LLVMBackend::getAstItem(Ast* ast)
{
	if (ast == nullptr)
		return nullptr;

	switch (ast->GetType())
	{
	case EASY_AST_TYPE::UNARY:
	{
		auto* unary = static_cast<UnaryAst*>(ast);
		getAstItem(unary->Data);
		unary->accept(this);
		return nullptr;
	}
	break;

	case EASY_AST_TYPE::PRIMATIVE:
		static_cast<PrimativeAst*>(ast)->accept(this);
		break;

	case EASY_AST_TYPE::RETURN:
		static_cast<ReturnAst*>(ast)->accept(this);
		break;

	case EASY_AST_TYPE::PARENTHESES_BLOCK:
		static_cast<ParenthesesGroupAst*>(ast)->accept(this);
		break;

	case EASY_AST_TYPE::EXPR_STATEMENT:
		static_cast<ExprStatementAst*>(ast)->accept(this);
		break;

	case EASY_AST_TYPE::VARIABLE:
		static_cast<VariableAst*>(ast)->accept(this);
		break;

	case EASY_AST_TYPE::ASSIGNMENT:
		static_cast<AssignmentAst*>(ast)->accept(this);
		break;

	case EASY_AST_TYPE::BLOCK:
		static_cast<BlockAst*>(ast)->accept(this);
		break;

	case EASY_AST_TYPE::FUNCTION_DECLERATION:
		static_cast<FunctionDefinetionAst*>(ast)->accept(this);
		break;

	case EASY_AST_TYPE::FUNCTION_CALL:
		static_cast<FunctionCallAst*>(ast)->accept(this);
		break;

	case EASY_AST_TYPE::IF_STATEMENT:
		static_cast<IfStatementAst*>(ast)->accept(this);
		break;

	case EASY_AST_TYPE::FOR:
		static_cast<ForStatementAst*>(ast)->accept(this);
		break;

	case EASY_AST_TYPE::BINARY_OPERATION:
		static_cast<BinaryAst*>(ast)->accept(this);
		break;

	case EASY_AST_TYPE::STRUCT_OPERATION:
		static_cast<StructAst*>(ast)->accept(this);
		break;

	case EASY_AST_TYPE::CONTROL_OPERATION:
		static_cast<ControlAst*>(ast)->accept(this);
		break;

	case EASY_AST_TYPE::NONE:
		break;
	}

	return nullptr;
}


void LLVMBackend::Compile(std::vector<char_type> & pOpcode)
{
	opcodes.assign(pOpcode.begin(), pOpcode.end());

	size_t totalAst = temporaryAsts.size();
	for (int i = 0; i < totalAst; ++i) {
		getAstItem(temporaryAsts[i]);
	}
	

	temporaryAsts.clear();
	pOpcode.assign(this->opcodes.begin(), this->opcodes.end());
	this->opcodes.clear();
}

/*
 *
 * func carp(a:int):int return a * 10
 * func test(a:int, b:int):int return a + b
 *
 * */
PrimativeValue* LLVMBackend::Execute()
{
	PrimativeValue* result = nullptr;
	size_t codeStart = temporaryAsts.size();

	this->Compile(temporaryAsts);
	impl->system.execute(&impl->codes[0], impl->codes.size(), codeStart);
	auto* lastItem = impl->system.getObject();

	if (lastItem != nullptr)
		switch (lastItem->Type)
		{
		case vm_object::vm_object_type::INT:
			result = new PrimativeValue(lastItem->Int);
			break;

		case vm_object::vm_object_type::DOUBLE:
			result = new PrimativeValue(lastItem->Double);
			break;

		case vm_object::vm_object_type::BOOL:
			result = new PrimativeValue(lastItem->Bool);
			break;

		case vm_object::vm_object_type::EMPTY:
			result = new PrimativeValue();
			break;

		case vm_object::vm_object_type::ARRAY:
			console_out << _T("(ARRAY) Size: ") << static_cast<vm_array*>(lastItem->Pointer)->Indicator << '\n';
			break;

		case vm_object::vm_object_type::STR:
			result = new PrimativeValue(string_type(static_cast<char_type*>(lastItem->Pointer)));
			break;
		}

	if (result != nullptr)
		console_out << result->Describe() << '\n';

	return result;
}

void LLVMBackend::Execute(std::vector<char_type> const & opcodes)
{
}

LLVMBackend::~LLVMBackend()
{
	delete impl;
	delete Scope::GlobalScope;
}

LLVMBackend::LLVMBackend()
{
	impl = new LLVMBackendImpl;
}

void LLVMBackend::visit(AssignmentAst* ast)
{
	
}

void LLVMBackend::visit(BlockAst* ast)
{
    //if data == 123 then { data = 111 } else {data = 999}
    size_t totalBlock = ast->Blocks.size();
    for (size_t i = 0; i < totalBlock; ++i) {
        getAstItem(ast->Blocks.at(i));
    }
}

void LLVMBackend::visit(IfStatementAst* ast)
{
	this->getAstItem(ast->ControlOpt);
	
}

void LLVMBackend::visit(FunctionDefinetionAst* ast)
{
	
}

void LLVMBackend::visit(ForStatementAst* ast)
{
   
}

void LLVMBackend::visit(VariableAst* ast)
{
	
}

void LLVMBackend::visit(PrimativeValue* value) {
	
}

void LLVMBackend::visit(PrimativeAst* ast) {
}

void LLVMBackend::visit(ControlAst* ast)
{
	
}

void LLVMBackend::visit(BinaryAst* ast)
{
	tmp::Type rightType = tmp::Type::INT;

	tmp::Type leftType = tmp::Type::INT;
	auto binaryResultType = tmp::Type::INT;

	auto leftValue = getPrimative(ast->Left);
	auto rightValue = getPrimative(ast->Right);

	Value *leftVal = ConstantInt::get(llvm::Type::getInt32Ty(impl->context), leftValue->Integer);
	Value *rightVal = ConstantInt::get(llvm::Type::getInt32Ty(impl->context), rightValue->Integer);


	switch (ast->Op)
	{
	case PLUS:
	{
		switch (binaryResultType)
		{
		case tmp::Type::INT:
			break;

		case tmp::Type::DOUBLE:
			//this->opcodes.push_back(vm_inst::OPT_dADD);
			break;

		case tmp::Type::BOOL:
			//this->opcodes.push_back(vm_inst::OPT_bADD);
			break;
		}
	}
	break;

	case MINUS:
	{
		switch (binaryResultType)
		{
		case tmp::Type::INT:
			//this->opcodes.push_back(vm_inst::OPT_iSUB);
			break;

		case tmp::Type::DOUBLE:
			//this->opcodes.push_back(vm_inst::OPT_dSUB);
			break;

		case tmp::Type::BOOL:
			//this->opcodes.push_back(vm_inst::OPT_bSUB);
			break;
		}
	}
	break;

	case MULTIPLICATION:
	{
		switch (binaryResultType)
		{
		case tmp::Type::INT:
			//this->opcodes.push_back(vm_inst::OPT_iMUL);
			break;

		case tmp::Type::DOUBLE:
			//this->opcodes.push_back(vm_inst::OPT_dMUL);
			break;

		case tmp::Type::BOOL:
			//this->opcodes.push_back(vm_inst::OPT_bMUL);
			break;
		}
	}
	break;

	case DIVISION:
	{
		switch (binaryResultType)
		{
		case tmp::Type::INT:
			//this->opcodes.push_back(vm_inst::OPT_iDIV);
			break;

		case tmp::Type::DOUBLE:
			//this->opcodes.push_back(vm_inst::OPT_dDIV);
			break;

		case tmp::Type::BOOL:
			//this->opcodes.push_back(vm_inst::OPT_bDIV);
			break;
		}
	}
	break;
	}
}

void LLVMBackend::visit(StructAst* ast) 
{
	
}

void LLVMBackend::visit(ReturnAst* ast)
{
	
}

void LLVMBackend::visit(ParenthesesGroupAst* ast) { }
void LLVMBackend::visit(FunctionCallAst* ast)
{
	
}

void LLVMBackend::visit(UnaryAst* ast)
{

}

void LLVMBackend::visit(ExprStatementAst* ast)
{
	ast->Expr->accept(this);
}

#endif