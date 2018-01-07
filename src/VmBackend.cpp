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
    vm_system system;
	std::unordered_map<string_type, size_t> variables;
	std::unordered_map<string_type, size_t> methods;
    std::vector<size_t> codes;

	std::vector<OpcodeItem*> intermediateCode;
	size_t opCodeIndex;
    bool inClass;

	VmBackendImpl()
	{
        inClass = false;
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
    size_t codeStart = impl->codes.size();
	auto astsEnd = temporaryAsts.cend();
	for (auto it = temporaryAsts.cbegin(); astsEnd != it; ++it)
        getData(*it);

	temporaryAsts.clear();
    //func test(a,b) return a + b test(10,20)
	
	this->Generate();
    
    size_t totalIntermediateCode = this->impl->intermediateCode.size();
    for (size_t i = 0; i < totalIntermediateCode; ++i) {
        if (this->impl->intermediateCode[i]->Opt != nullptr)
            delete this->impl->intermediateCode[i]->Opt;
        
        delete this->impl->intermediateCode[i];
    }
    
    this->impl->intermediateCode.clear();
    
	impl->system.execute(&impl->codes[0], impl->codes.size(), codeStart);
	auto data = impl->system.getUInt();

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

void VmBackend::Generate()
{
	size_t indexer = 0;
	impl->intermediateCode.push_back(new OpcodeItem(vm_inst::iHALT));
    size_t totalIntermediateCode = this->impl->intermediateCode.size();
    
	for (int i = 0; i < totalIntermediateCode; ++i) {
		console_out << _T("\t") << indexer << _T(" -> ") << vm_instToString(this->impl->intermediateCode[i]->OpCode);

		impl->codes.push_back(this->impl->intermediateCode[i]->OpCode);
		++indexer;

		if (this->impl->intermediateCode[i]->Opt != nullptr)
		{
			switch (this->impl->intermediateCode[i]->Opt->Type)
			{
			case OptVar::VARIABLE:
				impl->codes.push_back(this->impl->variables[((VariableOptVar*)this->impl->intermediateCode[i]->Opt)->Data]);
				console_out << _T(" ") << impl->codes[impl->codes.size() - 1];
				break;

			case OptVar::METHOD:
				impl->codes.push_back(this->impl->methods[((MethodOptVar*)this->impl->intermediateCode[i]->Opt)->Data]);
				console_out << _T(" ") << impl->codes[impl->codes.size() - 1];
				break;

			case OptVar::INT:
				impl->codes.push_back(((IntOptVar*)this->impl->intermediateCode[i]->Opt)->Data);
				console_out << _T(" ") << impl->codes[impl->codes.size() - 1];
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
	this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::iSTORE, new IntOptVar(static_cast<int>(this->impl->variables[ast->Name]))));
	this->impl->opCodeIndex += 2;
}

void VmBackend::visit(BlockAst* ast)
{
    //if data == 123 then { data = 111 } else {data = 999}
    size_t totalBlock = ast->Blocks.get()->size();
    for (size_t i = 0; i < totalBlock; ++i) {
        getData(ast->Blocks->at(i));
    }
}

void VmBackend::visit(IfStatementAst* ast)
{
	this->getData(ast->ControlOpt);
	auto lastOperator = this->impl->intermediateCode[this->impl->intermediateCode.size() - 1]->OpCode;

	OpcodeItem* condition = nullptr; 
	switch (lastOperator)
	{
	case vm_inst::iEQ:
		condition = new OpcodeItem(vm_inst::iIF_EQ, new IntOptVar(0));
		this->impl->intermediateCode.erase(this->impl->intermediateCode.begin() + (this->impl->intermediateCode.size() - 1));
		--this->impl->opCodeIndex;
		break;

	default:
		condition = new OpcodeItem(vm_inst::iJIF, new IntOptVar(0));
		break;
	}
    
	this->impl->intermediateCode.push_back(condition);
	this->impl->opCodeIndex += 2;
	this->getData(ast->True);
	((IntOptVar*)condition->Opt)->Data = this->impl->opCodeIndex;

	if (ast->False != nullptr)
	{
		((IntOptVar*)condition->Opt)->Data += 2;
		auto* trueStmt = new OpcodeItem(vm_inst::iJMP, new IntOptVar(0));
		this->impl->intermediateCode.push_back(trueStmt);
		this->getData(ast->False);
		((IntOptVar*)trueStmt->Opt)->Data = this->impl->opCodeIndex;
	}
}

void VmBackend::visit(FunctionDefinetionAst* ast)
{
    auto* jpmAddress = new OpcodeItem(vm_inst::iJMP);
    this->impl->opCodeIndex += 2;
    this->impl->intermediateCode.push_back(jpmAddress);
    bool definedFunc = this->impl->methods.find(ast->Name) != this->impl->methods.end();
    size_t oldMethodName = this->impl->methods[ast->Name];
    
	this->impl->methods[ast->Name] = this->impl->opCodeIndex;
    size_t totalParameter = ast->Args.size();
    for (size_t i = 0; i < totalParameter; ++i) {
        switch (i) {
            case 0:
                //this->impl->iexntermediateCode.push_back(new OpcodeItem(vm_inst::iLOAD_0));
                this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::iSTORE_0));
                this->impl->opCodeIndex += 1;
                break;
                // func test(a,b) return a + b test(10,20)
            case 1:
                //this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::iLOAD_1));
                this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::iSTORE_1));
                this->impl->opCodeIndex += 1;
                break;
                
            case 2:
                //this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::iLOAD_2));
                this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::iSTORE_2));
                this->impl->opCodeIndex += 1;
                break;
                
            case 3:
                //this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::iLOAD_3));
                this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::iSTORE_3));
                this->impl->opCodeIndex += 1;
                break;
                
            case 4:
                //this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::iLOAD_4));
                this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::iSTORE_4));
                this->impl->opCodeIndex += 1;
                break;
                
            default:
                //this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::iLOAD, new IntOptVar(i)));
                this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::iSTORE, new IntOptVar(i)));
                this->impl->opCodeIndex += 2;
                break;
        }
    }
    
    ast->Body->accept(this);
    jpmAddress->Opt = new IntOptVar(this->impl->opCodeIndex);
    
    if (definedFunc)
    {
        this->impl->codes[oldMethodName] = vm_inst::iJMP;
        this->impl->codes[oldMethodName + 1] = this->impl->opCodeIndex;
    }
}

void VmBackend::visit(ForStatementAst* ast) { }
void VmBackend::visit(VariableAst* ast) { }

void VmBackend::visit(PrimativeAst* ast) {
	this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::iPUSH, new IntOptVar(ast->Value->Integer)));
	this->impl->opCodeIndex += 2;
}

void VmBackend::visit(ControlAst* ast) 
{ 
	getData(ast->Left);
	getData(ast->Right);
	switch (ast->Op)
	{
	case EQUAL:
		this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::iEQ));
		break;

	case NOT_EQUAL:
		this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::iSUB));
		break;

	case GREATOR:
		this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::iGT));
		break;

	case LOWER:
		this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::iLT));
		break;

	case GREATOR_EQUAL:
		this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::iGTE));
		break;

	case LOWER_EQUAL:
		this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::iLTE));
		break;

	case OR:
		this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::iOR));
		break;

	case AND:
		this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::iAND));
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
			this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::iADD));
			break;

		case MINUS:
			this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::iSUB));
			break;

		case MULTIPLICATION:
			this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::iMUL));
			break;

		case DIVISION:
			this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::iDIV));
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
//if data == 123 then { data = 111 } else {data = 999}

void VmBackend::visit(StructAst* ast) { }
void VmBackend::visit(ReturnAst* ast) 
{ 
	if (ast->Data != nullptr)
		this->getData(ast->Data);

	this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::iRETURN));
	++this->impl->opCodeIndex;
}

void VmBackend::visit(ParenthesesGroupAst* ast) { }
void VmBackend::visit(FunctionCallAst* ast)
{
    size_t totalParameters = ast->Args.size();
    for (size_t i = 0; i < totalParameters; ++i)
        getData(ast->Args[i]);
    
    this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::iCALL, new IntOptVar(static_cast<int>(this->impl->methods[ast->Function]))));
    this->impl->opCodeIndex += 2;
}

void VmBackend::visit(UnaryAst* ast) { }
void VmBackend::visit(ExprStatementAst* ast)
{
	ast->Expr->accept(this);
}
