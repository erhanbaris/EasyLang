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
		INT,
		STRING
	} Type;

	virtual size_t Size() = 0;
};

class ByteOptVar : public OptVar
{
public:
    char const * Data{nullptr};
    size_t Length;
    ByteOptVar() { Type = INT; }
    ByteOptVar(char const * data, size_t length) { Type = INT; Data = data; Length = length; }
	size_t Size() override {
		return Length + 1;
	}
};

class IntOptVar : public OptVar
{
public:
	int Data;
	IntOptVar() { Type = INT; }
	IntOptVar(int data) { Type = INT; Data = data; }
	size_t Size() override {
		return 3;
	}
};

class MethodOptVar : public OptVar
{
public:
	string_type Data;
	MethodOptVar() { Type = METHOD; }
	MethodOptVar(string_type data) { Type = METHOD; Data = data; }
	size_t Size() override {
		return 3;
	}
};

class VariableOptVar : public OptVar
{
public:
	string_type Data;
	VariableOptVar() { Type = VARIABLE; }
	VariableOptVar(string_type data) { Type = VARIABLE; Data = data; }
	size_t Size() override {
		return 3;
	}
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
	std::unordered_map<string_type, size_t>* variables;
	std::unordered_map<string_type, size_t>* globalVariables;
	std::vector<std::unordered_map<string_type, size_t>*> variablesList;
    std::unordered_map<string_type, size_t> methods;
    std::vector<char> codes;
	bool dumpOpcode;

	std::vector<OpcodeItem*> intermediateCode;
	size_t opCodeIndex;
    size_t inClassCounter;
	size_t inFunctionCounter;

	VmBackendImpl()
	{
        inClassCounter = 0;
		inFunctionCounter = 0;
		opCodeIndex = 0;
		dumpOpcode = false;
		globalVariables = new std::unordered_map<string_type, size_t>();
		variables = new std::unordered_map<string_type, size_t>();
		variablesList.push_back(variables);
	}

	inline OpcodeItem* generateStore(size_t index)
	{
		switch (index)
		{
		case 0:
			return new OpcodeItem(vm_inst::OPT_STORE_0);

		case 1:
			return new OpcodeItem(vm_inst::OPT_STORE_1);

		case 2:
			return new OpcodeItem(vm_inst::OPT_STORE_2);

		case 3:
			return new OpcodeItem(vm_inst::OPT_STORE_3);

		case 4:
			return new OpcodeItem(vm_inst::OPT_STORE_4);

		default:
			new OpcodeItem(vm_inst::OPT_STORE, new IntOptVar(index));
		}

		return nullptr;
	}

	inline OpcodeItem* generateGlobalStore(size_t index)
	{
		switch (index)
		{
		case 0:
			return new OpcodeItem(vm_inst::OPT_GSTORE_0);

		case 1:
			return new OpcodeItem(vm_inst::OPT_GSTORE_1);

		case 2:
			return new OpcodeItem(vm_inst::OPT_GSTORE_2);

		case 3:
			return new OpcodeItem(vm_inst::OPT_GSTORE_3);

		case 4:
			return new OpcodeItem(vm_inst::OPT_GSTORE_4);

		default:
			new OpcodeItem(vm_inst::OPT_GSTORE, new IntOptVar(index));
		}

		return nullptr;
	}

	inline OpcodeItem* generateLoad(size_t index)
	{
		switch (index)
		{
		case 0:
			return new OpcodeItem(vm_inst::OPT_LOAD_0);

		case 1:
			return new OpcodeItem(vm_inst::OPT_LOAD_1);

		case 2:
			return new OpcodeItem(vm_inst::OPT_LOAD_2);

		case 3:
			return new OpcodeItem(vm_inst::OPT_LOAD_3);

		case 4:
			return new OpcodeItem(vm_inst::OPT_LOAD_4);

		default:
			new OpcodeItem(vm_inst::OPT_LOAD, new IntOptVar(index));
		}

		return nullptr;
	}

	inline OpcodeItem* generateGlobalLoad(size_t index)
	{
		switch (index)
		{
		case 0:
			return new OpcodeItem(vm_inst::OPT_GLOAD_0);

		case 1:
			return new OpcodeItem(vm_inst::OPT_GLOAD_1);

		case 2:
			return new OpcodeItem(vm_inst::OPT_GLOAD_2);

		case 3:
			return new OpcodeItem(vm_inst::OPT_GLOAD_3);

		case 4:
			return new OpcodeItem(vm_inst::OPT_GLOAD_4);

		default:
			new OpcodeItem(vm_inst::OPT_GLOAD, new IntOptVar(index));
		}

		return nullptr;
	}

	~VmBackendImpl()
	{
		delete variables;
		delete globalVariables;
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
		getData(unary->Data);
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


void VmBackend::Compile(std::vector<char> & opcode)
{
	size_t totalAst = temporaryAsts.size();
	for (int i = 0; i < totalAst; ++i) {
		getData(temporaryAsts[i]);
	}

	temporaryAsts.clear();
	this->Generate(opcode);

	size_t totalIntermediateCode = this->impl->intermediateCode.size();
	for (size_t i = 0; i < totalIntermediateCode; ++i) {
		if (this->impl->intermediateCode[i]->Opt != nullptr)
			delete this->impl->intermediateCode[i]->Opt;

		delete this->impl->intermediateCode[i];
	}

	this->impl->intermediateCode.clear();
}

PrimativeValue* VmBackend::Execute()
{
	PrimativeValue* result = nullptr;
    size_t codeStart = impl->codes.size();
	
	this->Compile(this->impl->codes);
	impl->system.execute(&impl->codes[0], impl->codes.size(), codeStart);	
	auto data = impl->system.getUInt();

	result = new PrimativeValue((int)data);
	console_out << result->Describe() << '\n';
	return result;
}

void VmBackend::Execute(std::vector<char> const & opcodes)
{
	impl->system.execute(const_cast<char*>(&opcodes[0]), opcodes.size(), 0);
	auto data = impl->system.getUInt();

	auto* result = new PrimativeValue((int)data);
	console_out << result->Describe() << '\n';
	delete result;
}

VmBackend::~VmBackend()
{
	delete impl;
	delete Scope::GlobalScope;
}
// data = (123 * 23) + 123 - 2
VmBackend::VmBackend()
{
	impl = new VmBackendImpl;
	Scope::GlobalScope = new Scope;
}

void VmBackend::Generate(std::vector<char> & opcodes)
{
	size_t indexer = 0;
	impl->intermediateCode.push_back(new OpcodeItem(vm_inst::OPT_HALT));
	++this->impl->opCodeIndex;
    size_t totalIntermediateCode = this->impl->intermediateCode.size();
    
	for (int i = 0; i < totalIntermediateCode; ++i) {
		opcodes.push_back(this->impl->intermediateCode[i]->OpCode);
		++indexer;

		if (this->impl->intermediateCode[i]->Opt != nullptr)
		{
			switch (this->impl->intermediateCode[i]->Opt->Type)
			{
			case OptVar::VARIABLE:
				opcodes.push_back((*this->impl->variables)[((VariableOptVar*)this->impl->intermediateCode[i]->Opt)->Data]);
				break;

			case OptVar::METHOD:
				opcodes.push_back(this->impl->methods[((MethodOptVar*)this->impl->intermediateCode[i]->Opt)->Data]);
				break;

			case OptVar::INT:
				opcodes.push_back(((IntOptVar*)this->impl->intermediateCode[i]->Opt)->Data);
				break;
			}

			++indexer;
		}
	}
}

void VmBackend::visit(AssignmentAst* ast)
{
	std::unordered_map<string_type, size_t>* variables = nullptr;
	if (this->impl->inFunctionCounter > 0)
		variables = this->impl->variables;
	else 
		variables = this->impl->globalVariables;

	if (variables->find(ast->Name) == variables->end())
		(*variables)[ast->Name] = variables->size();


	this->getData(ast->Data);
	if (this->impl->inFunctionCounter > 0)
		this->impl->intermediateCode.push_back(this->impl->generateStore(static_cast<int>((*this->impl->variables)[ast->Name])));
	else 
		this->impl->intermediateCode.push_back(this->impl->generateGlobalStore(static_cast<int>((*this->impl->globalVariables)[ast->Name])));

	if (this->impl->intermediateCode[this->impl->intermediateCode.size() - 1]->Opt == nullptr)
		++this->impl->opCodeIndex;
	else 
		this->impl->opCodeIndex += 2;
}

void VmBackend::visit(BlockAst* ast)
{
    //if data == 123 then { data = 111 } else {data = 999}
    size_t totalBlock = ast->Blocks.size();
    for (size_t i = 0; i < totalBlock; ++i) {
        getData(ast->Blocks.at(i));
    }
}

void VmBackend::visit(IfStatementAst* ast)
{
	this->getData(ast->ControlOpt);
	auto lastOperator = this->impl->intermediateCode[this->impl->intermediateCode.size() - 1]->OpCode;

	OpcodeItem* condition = nullptr; 
	switch (lastOperator)
	{
	case vm_inst::OPT_EQ:
		condition = new OpcodeItem(vm_inst::OPT_IF_EQ, new IntOptVar(0));
		this->impl->intermediateCode.erase(this->impl->intermediateCode.begin() + (this->impl->intermediateCode.size() - 1));
		--this->impl->opCodeIndex;
		break;

	default:
		condition = new OpcodeItem(vm_inst::OPT_JIF, new IntOptVar(0));
		break;
	}
    
	this->impl->intermediateCode.push_back(condition);
	this->impl->opCodeIndex += 2;
	this->getData(ast->True);
	((IntOptVar*)condition->Opt)->Data = this->impl->opCodeIndex;

	if (ast->False != nullptr)
	{
		((IntOptVar*)condition->Opt)->Data += 2;
		auto* trueStmt = new OpcodeItem(vm_inst::OPT_JMP, new IntOptVar(0));
		this->impl->intermediateCode.push_back(trueStmt);
		this->getData(ast->False);
		((IntOptVar*)trueStmt->Opt)->Data = this->impl->opCodeIndex;
	}
}

void VmBackend::visit(FunctionDefinetionAst* ast)
{
	++this->impl->inFunctionCounter;
    this->impl->variablesList.push_back(new std::unordered_map<string_type, size_t>());
    impl->variables = impl->variablesList[impl->variablesList.size() - 1];
    
    auto* jpmAddress = new OpcodeItem(vm_inst::OPT_JMP);
    this->impl->opCodeIndex += 2;
    this->impl->intermediateCode.push_back(jpmAddress);

	if (this->impl->methods.find(ast->Name) != this->impl->methods.end())
	{
		size_t oldMethodOrderNumber = this->impl->methods[ast->Name];
		this->impl->codes[oldMethodOrderNumber] = vm_inst::OPT_JMP;
		this->impl->codes[oldMethodOrderNumber + 1] = this->impl->opCodeIndex;
	}
    
	this->impl->methods[ast->Name] = this->impl->opCodeIndex;
    size_t totalParameter = ast->Args.size();
    for (size_t i = 0; i < totalParameter; ++i) {
        (*this->impl->variables)[ast->Args[i]->Name] = i;
	
		auto* opCode = this->impl->generateStore(i);
		this->impl->intermediateCode.push_back(opCode);

		if (opCode->Opt != nullptr)
			this->impl->opCodeIndex += 2;
		else
			++this->impl->opCodeIndex;
    }
    
    ast->Body->accept(this);
    jpmAddress->Opt = new IntOptVar(this->impl->opCodeIndex);
    
    this->impl->variablesList.erase(impl->variablesList.begin() + (impl->variablesList.size() - 1));
	--this->impl->inFunctionCounter;
    delete impl->variables;
}

void VmBackend::visit(ForStatementAst* ast) { }
void VmBackend::visit(VariableAst* ast)
{
    if (this->impl->inFunctionCounter > 0 && this->impl->variables->find(ast->Value) == this->impl->variables->end())
        throw ParseError(ast->Value + _T(" Not Found"));

	if (this->impl->inFunctionCounter == 0 && this->impl->globalVariables->find(ast->Value) == this->impl->globalVariables->end())
		throw ParseError(ast->Value + _T(" Not Found"));
    
	if (this->impl->inFunctionCounter == 0)
	{
		size_t index = (*this->impl->globalVariables)[ast->Value];
		auto* opCode = this->impl->generateGlobalLoad(index);
		this->impl->intermediateCode.push_back(opCode);
		
		if (opCode->Opt != nullptr)
			this->impl->opCodeIndex += 2; 
		else 
			++this->impl->opCodeIndex;
	}
	else
	{
		size_t index = (*this->impl->globalVariables)[ast->Value];
		auto* opCode = this->impl->generateLoad(index);
		this->impl->intermediateCode.push_back(opCode);

		if (opCode->Opt != nullptr)
			this->impl->opCodeIndex += 2;
		else
			++this->impl->opCodeIndex;
	}
}

void VmBackend::visit(PrimativeAst* ast) {
	switch (ast->Value->Type)
	{
	case PrimativeValue::Type::PRI_INTEGER:
		this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::OPT_PUSH, new IntOptVar(ast->Value->Integer)));
		break;

	case PrimativeValue::Type::PRI_STRING:
    {
        const char* text = ast->Value->String->c_str();
        this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::OPT_PUSH, new ByteOptVar(text, strlen(text))));
    }
		break;
		

	default:
		break;
	}

	this->impl->opCodeIndex += 2;
}

void VmBackend::visit(ControlAst* ast) 
{ 
	getData(ast->Left);
	getData(ast->Right);
	switch (ast->Op)
	{
	case EQUAL:
		this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::OPT_EQ));
		break;

	//case NOT_EQUAL:
	//	this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::opt_n));
	//	break;

	case GREATOR:
		this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::OPT_GT));
		break;

	case LOWER:
		this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::OPT_LT));
		break;

	case GREATOR_EQUAL:
		this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::OPT_GTE));
		break;

	case LOWER_EQUAL:
		this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::OPT_LTE));
		break;

	case OR:
		this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::OPT_OR));
		break;

	case AND:
		this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::OPT_AND));
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
			this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::OPT_iADD));
			break;

		case MINUS:
			this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::OPT_iSUB));
			break;

		case MULTIPLICATION:
			this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::OPT_iMUL));
			break;

		case DIVISION:
			this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::OPT_iDIV));
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

	this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::OPT_RETURN));
	++this->impl->opCodeIndex;
}

void VmBackend::visit(ParenthesesGroupAst* ast) { }
void VmBackend::visit(FunctionCallAst* ast)
{
	if (ast->Package == "core" && ast->Function == "dumpopcode")
	{
		impl->system.dump(&impl->codes[0], impl->codes.size());
		return;
	}
    
	size_t totalParameters = ast->Args.size();
    for (size_t i = totalParameters; i > 0; --i)
        getData(ast->Args[i - 1]);
    
    this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::OPT_CALL, new IntOptVar(static_cast<int>(this->impl->methods[ast->Function]))));
    this->impl->opCodeIndex += 2;
}

void VmBackend::visit(UnaryAst* ast) 
{ 
	this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::OPT_NEG));
	++this->impl->opCodeIndex;
}

void VmBackend::visit(ExprStatementAst* ast)
{
	ast->Expr->accept(this);
}
