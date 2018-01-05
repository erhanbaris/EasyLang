#include "Exceptions.h"
#include "VmBackend.h"
#include "System.h"
#include "Vm.h"

class NullBuffer : public std::streambuf
{
public:
	int overflow(int c) { return c; }
};

void VmBackend::Prepare(std::shared_ptr<std::vector<Ast*>> pAsts)
{
	temporaryAsts.clear();
	temporaryAsts.insert(temporaryAsts.end(), pAsts.get()->begin(), pAsts.get()->end());
}

PrimativeValue* VmBackend::getPrimative(Ast* ast)
{
	auto* primative = static_cast<PrimativeAst*>(ast)->Value;
	return primative;
}

PrimativeValue* VmBackend::getData(Ast* ast, Scope & scope)
{
	if (ast == nullptr)
		return nullptr;

	switch (ast->GetType())
	{
	case EASY_AST_TYPE::UNARY:
	{
		auto* unary = static_cast<UnaryAst*>(ast);
		unary->accept(generator);
		if (unary->Opt == EASY_OPERATOR_TYPE::MINUS)
		{
			auto* unaryData = getData(unary->Data, scope);
			return (*unaryData) * (-1);
		}

		return nullptr;
	}
	break;

	case EASY_AST_TYPE::PRIMATIVE:
		static_cast<PrimativeAst*>(ast)->accept(generator);
		break;

	case EASY_AST_TYPE::RETURN:
		static_cast<ReturnAst*>(ast)->accept(generator);
		break;

	case EASY_AST_TYPE::PARENTHESES_BLOCK:
		static_cast<ParenthesesGroupAst*>(ast)->accept(generator);
		break;

	case EASY_AST_TYPE::EXPR_STATEMENT:
		static_cast<ExprStatementAst*>(ast)->accept(generator);
		break;

	case EASY_AST_TYPE::VARIABLE:
		static_cast<VariableAst*>(ast)->accept(generator);
		break;

	case EASY_AST_TYPE::ASSIGNMENT:
		static_cast<AssignmentAst*>(ast)->accept(generator);
		break;

	case EASY_AST_TYPE::BLOCK:
		static_cast<BlockAst*>(ast)->accept(generator);
		break;

	case EASY_AST_TYPE::FUNCTION_DECLERATION:
		static_cast<FunctionDefinetionAst*>(ast)->accept(generator);
		break;

	case EASY_AST_TYPE::FUNCTION_CALL:
		static_cast<FunctionCallAst*>(ast)->accept(generator);
		break;

	case EASY_AST_TYPE::IF_STATEMENT:
		static_cast<IfStatementAst*>(ast)->accept(generator);
		break;


	case EASY_AST_TYPE::FOR:
		static_cast<ForStatementAst*>(ast)->accept(generator);
		break;

	case EASY_AST_TYPE::BINARY_OPERATION:
		static_cast<BinaryAst*>(ast)->accept(generator);
		break;

	case EASY_AST_TYPE::STRUCT_OPERATION:
		static_cast<StructAst*>(ast)->accept(generator);
		break;

	case EASY_AST_TYPE::CONTROL_OPERATION:
		static_cast<ControlAst*>(ast)->accept(generator);
		break;
	}

	return nullptr;
}

PrimativeValue* VmBackend::Execute()
{
	if (generator != nullptr)
		delete generator;

	generator = new CodeGenerator;
	PrimativeValue* result = nullptr;
	auto astsEnd = temporaryAsts.cend();
	for (auto it = temporaryAsts.cbegin(); astsEnd != it; ++it)
	{
		result = getData(*it, *Scope::GlobalScope);
		asts.push_back(*it);
	}

	temporaryAsts.clear();

	std::vector<size_t> codes;
	generator->Generate(codes);
	codes.push_back(vm_inst::iHALT);
	vm_system system;
	system.execute(&codes[0], codes.size());
	auto data = system.getUInt();

	return result;
}

VmBackend::VmBackend()
{
	Scope::GlobalScope = new Scope;
}

VmBackend::~VmBackend()
{

}
