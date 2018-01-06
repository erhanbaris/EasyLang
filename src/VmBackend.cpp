#include "Exceptions.h"
#include "VmBackend.h"
#include "System.h"
#include "Vm.h"

class NullBuffer : public std::streambuf
{
public:
	int overflow(int c) { return c; }
};

class OptVar
{
public:
	enum {
		VARIABLE,
		METHOD,
		INT
	} Type;
};

class IntOptVar : public OptVar
{
public:
	size_t Data;
	IntOptVar() { Type = INT; }
	IntOptVar(size_t data) { Type = INT; Data = data; }
};

class MethodOptVar : public OptVar
{
public:
	string_type Data;
	MethodOptVar() { Type = METHOD; }
	MethodOptVar(string_type data) { Type = METHOD; Data = data; }
};

class VariableOptVar : public OptVar
{
public:
	string_type Data;
	VariableOptVar() { Type = VARIABLE; }
	VariableOptVar(string_type data) { Type = VARIABLE; Data = data; }
};

class OpcodeItem {
public:
	vm_inst OpCode;
	OptVar* Opt{ nullptr };

	OpcodeItem() {}
	OpcodeItem(vm_inst opCode) : OpCode(opCode) { }
	OpcodeItem(vm_inst opCode, OptVar* opt) : OpCode(opCode), Opt(opt) { }
};

class VmBackendImpl
{
public:
	std::unordered_map<string_type, size_t> variables;
	std::unordered_map<string_type, size_t> methods;

	std::vector<OpcodeItem*> opcodes;
	size_t opCodeIndex;

	VmBackendImpl()
	{
		opCodeIndex = 0;
	}
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

PrimativeValue* VmBackend::getData(Ast* ast)
{
	if (ast == nullptr)
		return nullptr;

	switch (ast->GetType())
	{
	case EASY_AST_TYPE::UNARY:
	{
		auto* unary = static_cast<UnaryAst*>(ast);
		unary->accept(this);
		if (unary->Opt == EASY_OPERATOR_TYPE::MINUS)
		{
			auto* unaryData = getData(unary->Data);
			return (*unaryData) * (-1);
		}

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

PrimativeValue* VmBackend::Execute()
{
	PrimativeValue* result = nullptr;
	auto astsEnd = temporaryAsts.cend();
	for (auto it = temporaryAsts.cbegin(); astsEnd != it; ++it)
	{
		result = getData(*it);
		asts.push_back(*it);
	}

	temporaryAsts.clear();

	std::vector<size_t> codes;

	Generate(codes);
	//vm_system system;
	//system.execute(&codes[0], codes.size());
	//auto data = system.getUInt();

	return result;
}


VmBackend::~VmBackend()
{
	delete impl;
}
// data = (123 * 23) + 123 - 2
VmBackend::VmBackend()
{
	impl = new VmBackendImpl;
	Scope::GlobalScope = new Scope;
}

void VmBackend::Generate(std::vector<size_t>& codes)
{
	size_t indexer = 0;
	impl->opcodes.push_back(new OpcodeItem(vm_inst::iHALT));
	for (int i = 0; i < this->impl->opcodes.size(); ++i) {
		console_out << _T("\t") << indexer << _T(" -> ") << vm_instToString(this->impl->opcodes[i]->OpCode);

		codes.push_back(this->impl->opcodes[i]->OpCode);
		++indexer;

		if (this->impl->opcodes[i]->Opt != nullptr)
		{
			switch (this->impl->opcodes[i]->Opt->Type)
			{
			case OptVar::VARIABLE:
				codes.push_back(this->impl->variables[((VariableOptVar*)this->impl->opcodes[i]->Opt)->Data]);
				console_out << _T(" ") << codes[codes.size() - 1];
				break;

			case OptVar::METHOD:
				codes.push_back(this->impl->methods[((MethodOptVar*)this->impl->opcodes[i]->Opt)->Data]);
				console_out << _T(" ") << codes[codes.size() - 1];
				break;

			case OptVar::INT:
				codes.push_back(((IntOptVar*)this->impl->opcodes[i]->Opt)->Data);
				console_out << _T(" ") << codes[codes.size() - 1];
				break;
			}

			++indexer;
		}

		console_out << '\n';
	}
}

void VmBackend::visit(AssignmentAst* ast)
{
	this->impl->variables[ast->Name] = this->impl->variables.size();
	this->getData(ast->Data);
	this->impl->opcodes.push_back(new OpcodeItem(vm_inst::iSTORE, new IntOptVar(static_cast<int>(this->impl->variables[ast->Name]))));
	this->impl->opCodeIndex += 2;
}

void VmBackend::visit(BlockAst* ast) { }
void VmBackend::visit(IfStatementAst* ast) 
{
	this->getData(ast->ControlOpt);
	auto lastOperator = this->impl->opcodes[this->impl->opcodes.size() - 1]->OpCode;

	OpcodeItem* condition = nullptr; 
	switch (lastOperator)
	{
	case vm_inst::iEQ:
		condition = new OpcodeItem(vm_inst::iIF_EQ, new IntOptVar(0));
		this->impl->opcodes.erase(this->impl->opcodes.begin() + (this->impl->opcodes.size() - 1));
		--this->impl->opCodeIndex;
		break;

	default:
		condition = new OpcodeItem(vm_inst::iJIF, new IntOptVar(0));
		break;
	}
	this->impl->opcodes.push_back(condition);
	this->impl->opCodeIndex += 2;
	this->getData(ast->True);
	((IntOptVar*)condition->Opt)->Data = this->impl->opCodeIndex;

	if (ast->False != nullptr)
	{
		((IntOptVar*)condition->Opt)->Data += 2;
		auto* trueStmt = new OpcodeItem(vm_inst::iJMP, new IntOptVar(0));
		this->impl->opcodes.push_back(trueStmt);
		this->getData(ast->False);
		((IntOptVar*)trueStmt->Opt)->Data = this->impl->opCodeIndex;
	}
}

void VmBackend::visit(FunctionDefinetionAst* ast)
{
	this->impl->methods[ast->Name] = this->impl->methods.size();
}

void VmBackend::visit(ForStatementAst* ast) { }
void VmBackend::visit(VariableAst* ast) { }

void VmBackend::visit(PrimativeAst* ast) {
	this->impl->opcodes.push_back(new OpcodeItem(vm_inst::iPUSH, new IntOptVar(ast->Value->Integer)));
	this->impl->opCodeIndex += 2;
}

void VmBackend::visit(ControlAst* ast) 
{ 
	getData(ast->Left);
	getData(ast->Right);
	switch (ast->Op)
	{
	case EQUAL:
		this->impl->opcodes.push_back(new OpcodeItem(vm_inst::iEQ));
		break;

	case NOT_EQUAL:
		this->impl->opcodes.push_back(new OpcodeItem(vm_inst::iSUB));
		break;

	case GREATOR:
		this->impl->opcodes.push_back(new OpcodeItem(vm_inst::iGT));
		break;

	case LOWER:
		this->impl->opcodes.push_back(new OpcodeItem(vm_inst::iLT));
		break;

	case GREATOR_EQUAL:
		this->impl->opcodes.push_back(new OpcodeItem(vm_inst::iGTE));
		break;

	case LOWER_EQUAL:
		this->impl->opcodes.push_back(new OpcodeItem(vm_inst::iLTE));
		break;

	case OR:
		this->impl->opcodes.push_back(new OpcodeItem(vm_inst::iOR));
		break;

	case AND:
		this->impl->opcodes.push_back(new OpcodeItem(vm_inst::iAND));
		break;
	}

	++this->impl->opCodeIndex;
}

void VmBackend::visit(BinaryAst* ast)
{
	getData(ast->Left);
	getData(ast->Right);
	switch (ast->Op)
	{
		case PLUS:
			this->impl->opcodes.push_back(new OpcodeItem(vm_inst::iADD));
			break;

		case MINUS:
			this->impl->opcodes.push_back(new OpcodeItem(vm_inst::iSUB));
			break;

		case MULTIPLICATION:
			this->impl->opcodes.push_back(new OpcodeItem(vm_inst::iMUL));
			break;

		case DIVISION:
			this->impl->opcodes.push_back(new OpcodeItem(vm_inst::iDIV));
			break;

		case ASSIGN:
			break;

		case APPEND:
			break;

		case INDEXER:
			break;
	}

	++this->impl->opCodeIndex;
}

void VmBackend::visit(StructAst* ast) { }
void VmBackend::visit(ReturnAst* ast) 
{ 
	if (ast->Data != nullptr)
		this->getData(ast->Data);

	this->impl->opcodes.push_back(new OpcodeItem(vm_inst::iRETURN));
	++this->impl->opCodeIndex;
}

void VmBackend::visit(ParenthesesGroupAst* ast) { }
void VmBackend::visit(FunctionCallAst* ast) { }
void VmBackend::visit(UnaryAst* ast) { }
void VmBackend::visit(ExprStatementAst* ast)
{
	ast->Expr->accept(this);
}