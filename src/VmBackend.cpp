#include "Exceptions.h"
#include "VmBackend.h"
#include "System.h"
#include "Vm.h"

#define INC_OPCODE() ++this->impl->opCodeIndex;
#define ADD_OPCODE(num) this->impl->opCodeIndex += num ;

//#define INC_OPCODE() {++this->impl->opCodeIndex; console_out << _T("OPCODE : ") << this->impl->opCodeIndex << _T(" ") << __LINE__ << '\n';}
//#define ADD_OPCODE(num) {this->impl->opCodeIndex += num ; console_out << _T("OPCODE : ") << this->impl->opCodeIndex << _T(" ") << __LINE__ << '\n';}

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
        DOUBLE,
        BOOL,
		STRING
	} Type;

	virtual size_t Size() = 0;
};

class ByteOptVar : public OptVar
{
public:
    char * Data{nullptr};
    size_t Length;
    ByteOptVar() { Type = INT; }
    ByteOptVar(char * data, size_t length) { Type = INT; Data = data; Length = length; }
    ByteOptVar(int data)
    {
        Type = INT;
        vm_int_t i = { .Int = data };
        Data = new char[2];
        Data[0] = i.Chars[0];
        Data[1] = i.Chars[1];
        Length = 2;
    }

    ByteOptVar(double data)
    {
        Type = DOUBLE;
        vm_double_t i = { .Double = data };
        Data = new char[8];
        Data[0] = i.Chars[0];
        Data[1] = i.Chars[1];
        Data[2] = i.Chars[2];
        Data[3] = i.Chars[3];
        Data[4] = i.Chars[4];
        Data[5] = i.Chars[5];
        Data[6] = i.Chars[6];
        Data[7] = i.Chars[7];
        Length = 8;
    }

    ByteOptVar(bool data)
    {
        Type = BOOL;
        Data = new char[1];
        Data[0] = data;
        Length = 1;
    }

	size_t Size() override {
		return Length + 1;
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

class VariableInfo {
public:
    Type Type;
    string_type Name;
    int Index;
};

class MethodInfo {
public:
    Type ReturnType;
    string_type Function;
    int Index;
	std::vector<Type> Args;
};

class VmBackendImpl
{
public:
    vm_system system;
	std::unordered_map<string_type, VariableInfo*>* variables;
	std::unordered_map<string_type, VariableInfo*>* globalVariables;
	std::vector<std::unordered_map<string_type, VariableInfo*>*> variablesList;
    std::unordered_map<string_type, MethodInfo*> methods;
    std::vector<char> codes;
	bool dumpOpcode;

	std::vector<OpcodeItem*> intermediateCode;
	int opCodeIndex;
    size_t inClassCounter;
	size_t inFunctionCounter;

	VmBackendImpl()
	{
        inClassCounter = 0;
		inFunctionCounter = 0;
		opCodeIndex = 0;
		dumpOpcode = false;
		globalVariables = new std::unordered_map<string_type, VariableInfo*>();
		variables = new std::unordered_map<string_type, VariableInfo*>();
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
			new OpcodeItem(vm_inst::OPT_STORE, new ByteOptVar((int)index));
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
			new OpcodeItem(vm_inst::OPT_GSTORE, new ByteOptVar((int)index));
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
			new OpcodeItem(vm_inst::OPT_LOAD, new ByteOptVar((int)index));
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
			new OpcodeItem(vm_inst::OPT_GLOAD, new ByteOptVar((int)index));
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

void VmBackend::addConvertOpcode(Type from, Type to)
{
	if (from == Type::INT && to == Type::DOUBLE)
		this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::OPT_I2D));
	else if (to == Type::INT && from == Type::DOUBLE)
		this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::OPT_D2I));
	else if (from == Type::INT && to == Type::BOOL)
		this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::OPT_I2B));
	else if (to == Type::INT && from == Type::BOOL)
		this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::OPT_B2I));
	else if (from == Type::DOUBLE && to == Type::BOOL)
		this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::OPT_D2B));
	else if (to == Type::DOUBLE && from == Type::BOOL)
		this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::OPT_B2D));

	INC_OPCODE();
}

Type VmBackend::operationResultType(Type from, Type to)
{
	if (from == Type::INT && to == Type::DOUBLE)
		return Type::DOUBLE;
	else if (to == Type::INT && from == Type::DOUBLE)
		return Type::DOUBLE;
	else if (to == Type::INT && from == Type::INT)
		return Type::INT;
	else if (from == Type::INT && to == Type::BOOL)
		return Type::INT;
	else if (to == Type::INT && from == Type::BOOL)
		return Type::INT;
	else if (to == Type::BOOL && from == Type::BOOL)
		return Type::BOOL;
	else if (from == Type::DOUBLE && to == Type::BOOL)
		return Type::DOUBLE;
	else if (to == Type::DOUBLE && from == Type::BOOL)
		return Type::DOUBLE;
	else if (to == Type::DOUBLE && from == Type::DOUBLE)
		return Type::DOUBLE;
}

Type VmBackend::detectType(Ast* ast)
{
	if (ast == nullptr)
		return Type::EMPTY;

	switch (ast->GetType())
	{
		case EASY_AST_TYPE::UNARY:
		{
			auto* unary = static_cast<UnaryAst*>(ast);
			return detectType(unary->Data);
		}

		case EASY_AST_TYPE::PRIMATIVE:
		{
			auto type = static_cast<PrimativeAst*>(ast)->Value->Type;
			switch (type)
			{

				case PrimativeValue::Type::PRI_INTEGER:
					return Type::INT;

				case PrimativeValue::Type::PRI_DOUBLE:
					return Type::DOUBLE;

				case PrimativeValue::Type::PRI_STRING:
					return Type::STRING;

				case PrimativeValue::Type::PRI_BOOL:
					return Type::BOOL;

				case PrimativeValue::Type::PRI_ARRAY:
					return Type::ARRAY;

				case PrimativeValue::Type::PRI_DICTIONARY:
					return Type::DICTIONARY;

				case PrimativeValue::Type::PRI_NULL:
					return Type::EMPTY;
			}
		}

		case EASY_AST_TYPE::RETURN:
			return Type::EMPTY;

		case EASY_AST_TYPE::PARENTHESES_BLOCK:
            return detectType(static_cast<ParenthesesGroupAst*>(ast)->Data);

		case EASY_AST_TYPE::EXPR_STATEMENT:
			return detectType(static_cast<ExprStatementAst*>(ast)->Expr);
			break;

		case EASY_AST_TYPE::VARIABLE:
        {
            auto* var = static_cast<VariableAst*>(ast);
            std::unordered_map<string_type, VariableInfo*>* variables = nullptr;

            if (this->impl->inFunctionCounter > 0)
                variables = this->impl->variables;
            else
                variables = this->impl->globalVariables;

            if (variables->find(var->Value) == variables->end())
                throw ParseError(_T("'") + var->Value + _T("' Not Found"));

			return variables->find(var->Value)->second->Type;
        }
			break;

		case EASY_AST_TYPE::ASSIGNMENT:
			return Type::EMPTY;
			break;

		case EASY_AST_TYPE::BLOCK:
			return Type::EMPTY;
			break;

		case EASY_AST_TYPE::FUNCTION_DECLERATION:
			//static_cast<FunctionDefinetionAst*>(ast)->accept(this);
			break;

		case EASY_AST_TYPE::FUNCTION_CALL:
        {
            auto* call = static_cast<FunctionCallAst*>(ast);
            return this->impl->methods[call->Package + _T("::") + call->Function]->ReturnType;
        }
			break;

		case EASY_AST_TYPE::IF_STATEMENT:
			return Type::EMPTY;

		case EASY_AST_TYPE::FOR:
			return Type::EMPTY;

		case EASY_AST_TYPE::BINARY_OPERATION:
		{
			auto binary = static_cast<BinaryAst*>(ast);
			Type rightType =  detectType(binary->Right);
			Type leftType =  detectType(binary->Left);

            return operationResultType(rightType, leftType);
		}
			break;

		case EASY_AST_TYPE::STRUCT_OPERATION:
			//static_cast<StructAst*>(ast)->accept(this);
			break;

		case EASY_AST_TYPE::CONTROL_OPERATION:
			//static_cast<ControlAst*>(ast)->accept(this);
			break;

		case EASY_AST_TYPE::NONE:
			return Type::EMPTY;
	}

	return Type::EMPTY;
}

PrimativeValue* VmBackend::getAstItem(Ast* ast)
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


void VmBackend::Compile(std::vector<char> & opcode)
{
	size_t totalAst = temporaryAsts.size();
	for (int i = 0; i < totalAst; ++i) {
		getAstItem(temporaryAsts[i]);
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

/*
 *
 * func carp(a:int):int return a * 10
 * func test(a:int, b:int):int return a + b
 *
 * */
PrimativeValue* VmBackend::Execute()
{
	PrimativeValue* result = nullptr;
    size_t codeStart = impl->codes.size();
	
	this->Compile(this->impl->codes);
	impl->system.execute(&impl->codes[0], impl->codes.size(), codeStart);	
	auto lastItem = impl->system.getObject();

	switch (lastItem.Type)
	{
		case vm_object::vm_object_type::INT:
			result = new PrimativeValue(lastItem.Int);
			break;

		case vm_object::vm_object_type::DOUBLE:
			result = new PrimativeValue(lastItem.Double);
			break;

		case vm_object::vm_object_type::BOOL:
			result = new PrimativeValue(lastItem.Bool);
			break;

		case vm_object::vm_object_type::EMPTY:
			result = new PrimativeValue();
			break;
	}


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
	INC_OPCODE();
    size_t totalIntermediateCode = this->impl->intermediateCode.size();
    
	for (int i = 0; i < totalIntermediateCode; ++i) {
		opcodes.push_back(this->impl->intermediateCode[i]->OpCode);
		++indexer;

		if (this->impl->intermediateCode[i]->Opt != nullptr)
		{
			switch (this->impl->intermediateCode[i]->Opt->Type)
			{
			case OptVar::VARIABLE:
				opcodes.push_back((*this->impl->variables)[((VariableOptVar*)this->impl->intermediateCode[i]->Opt)->Data]->Index);
				break;

			case OptVar::METHOD:
				opcodes.push_back(this->impl->methods[((MethodOptVar*)this->impl->intermediateCode[i]->Opt)->Data]->Index);
				break;

            case OptVar::BOOL:
			case OptVar::INT:
			case OptVar::DOUBLE:
			{
                ByteOptVar* byteOpt = (ByteOptVar*)this->impl->intermediateCode[i]->Opt;
                for (int j = 0; j < byteOpt->Length; ++j)
                    opcodes.push_back(byteOpt->Data[(byteOpt->Length - j) - 1]);
			}
				break;
			}

			++indexer;
		}
	}
}

void VmBackend::visit(AssignmentAst* ast)
{
	std::unordered_map<string_type, VariableInfo*>* variables = nullptr;
	if (this->impl->inFunctionCounter > 0)
		variables = this->impl->variables;
	else
		variables = this->impl->globalVariables;

	if (variables->find(ast->Name) == variables->end())
	{
		auto* varInfo = new VariableInfo;
		varInfo->Index = variables->size();
		varInfo->Name = ast->Name;

		if (ast->VariableType == EASY_KEYWORD_TYPE::KEYWORD_NONE)
			varInfo->Type = detectType(ast->Data);
		else {
			switch (ast->VariableType)
			{
				case TYPE_BOOL:
					varInfo->Type = Type::BOOL;
					break;

				case TYPE_INT:
					varInfo->Type = Type::INT;
					break;

				case TYPE_DOUBLE:
					varInfo->Type = Type::DOUBLE;
					break;

				case TYPE_STRING:
					varInfo->Type = Type::STRING;
					break;

				case TYPE_ARRAY:
					varInfo->Type = Type::ARRAY;
					break;

				case TYPE_DICTIONARY:
					varInfo->Type = Type::DICTIONARY;
					break;
			}
		}

		(*variables)[ast->Name] = varInfo;
	}
	else
	{
		VariableInfo* info = variables->find(ast->Name)->second;
		if (ast->VariableType != EASY_KEYWORD_TYPE::KEYWORD_NONE || info->Type != detectType(ast->Data))
			throw ParseError(_T("'") + ast->Name + _T("' Adready Defined"));
	}


	this->getAstItem(ast->Data);
	if (this->impl->inFunctionCounter > 0)
		this->impl->intermediateCode.push_back(this->impl->generateStore(static_cast<int>((*this->impl->variables)[ast->Name]->Index)));
	else
		this->impl->intermediateCode.push_back(this->impl->generateGlobalStore(static_cast<int>((*this->impl->globalVariables)[ast->Name]->Index)));

	if (this->impl->intermediateCode[this->impl->intermediateCode.size() - 1]->Opt == nullptr) {
		INC_OPCODE();
	}
	else
	{
		ADD_OPCODE(2);
	}
}

void VmBackend::visit(BlockAst* ast)
{
    //if data == 123 then { data = 111 } else {data = 999}
    size_t totalBlock = ast->Blocks.size();
    for (size_t i = 0; i < totalBlock; ++i) {
        getAstItem(ast->Blocks.at(i));
    }
}

void VmBackend::visit(IfStatementAst* ast)
{
	this->getAstItem(ast->ControlOpt);
	auto lastOperator = this->impl->intermediateCode[this->impl->intermediateCode.size() - 1]->OpCode;

	OpcodeItem* condition = nullptr;
	switch (lastOperator)
	{
	case vm_inst::OPT_EQ:
		condition = new OpcodeItem(vm_inst::OPT_IF_EQ);
		this->impl->intermediateCode.erase(this->impl->intermediateCode.begin() + (this->impl->intermediateCode.size() - 1));
		--this->impl->opCodeIndex;
		//console_out << _T("OPCODE : ") << this->impl->opCodeIndex << _T(" ") << __LINE__ << '\n';
		break;

	default:
		condition = new OpcodeItem(vm_inst::OPT_JIF);
		break;
	}

	this->impl->intermediateCode.push_back(condition);
	ADD_OPCODE(3);
	this->getAstItem(ast->True);
    condition->Opt = new ByteOptVar(this->impl->opCodeIndex);

	if (ast->False != nullptr)
	{
        /*vm_int_t conditionJumpAddress = { .Int = 0 };
        conditionJumpAddress.Chars[0] = ((ByteOptVar*)condition->Opt)->Data[0];
        conditionJumpAddress.Chars[1] = ((ByteOptVar*)condition->Opt)->Data[1];

        conditionJumpAddress.Int += 3;
        ((ByteOptVar*)condition->Opt)->Data[0] = conditionJumpAddress.Chars[0];
        ((ByteOptVar*)condition->Opt)->Data[1] = conditionJumpAddress.Chars[1];*/

		auto* trueStmt = new OpcodeItem(vm_inst::OPT_JMP);
		this->impl->intermediateCode.push_back(trueStmt);
		ADD_OPCODE(3);
		this->getAstItem(ast->False);
        trueStmt->Opt = new ByteOptVar(this->impl->opCodeIndex);
	}
}

void VmBackend::visit(FunctionDefinetionAst* ast)
{
	++this->impl->inFunctionCounter;
    this->impl->variablesList.push_back(new std::unordered_map<string_type, VariableInfo*>());
    impl->variables = impl->variablesList[impl->variablesList.size() - 1];

    auto* jpmAddress = new OpcodeItem(vm_inst::OPT_JMP);
	ADD_OPCODE(3);
    this->impl->intermediateCode.push_back(jpmAddress);

	if (this->impl->methods.find(_T("::") + ast->Name) != this->impl->methods.end())
	{
		size_t oldMethodOrderNumber = this->impl->methods[_T("::") + ast->Name]->Index;
		this->impl->codes[oldMethodOrderNumber] = vm_inst::OPT_JMP;
		this->impl->codes[oldMethodOrderNumber + 1] = static_cast<char>(this->impl->opCodeIndex);
	}

    MethodInfo* methodInfo = new MethodInfo;
    methodInfo->Index = static_cast<unsigned long>(this->impl->opCodeIndex);
    switch (ast->ReturnType)
    {
        case TYPE_BOOL:
            methodInfo->ReturnType = Type::BOOL;
            break;

        case TYPE_INT:
            methodInfo->ReturnType = Type::INT;
            break;

        case TYPE_DOUBLE:
            methodInfo->ReturnType = Type::DOUBLE;
            break;

        case TYPE_STRING:
            methodInfo->ReturnType = Type::STRING;
            break;

        case TYPE_ARRAY:
            methodInfo->ReturnType = Type::ARRAY;
            break;

        case TYPE_DICTIONARY:
            methodInfo->ReturnType = Type::DICTIONARY;
            break;
    }

    methodInfo->Function = ast->Name;
	this->impl->methods[_T("::") + ast->Name] = methodInfo;
    size_t totalParameter = ast->Args.size();

	// Todo : Save variable information
    for (size_t i = 0; i < totalParameter; ++i) {

        auto* varInfo = new VariableInfo;
        varInfo->Index = i;
        varInfo->Name = ast->Args[i]->Name;
        switch (ast->Args[i]->Type)
        {
            case TYPE_BOOL:
                varInfo->Type = Type::BOOL;
                break;

            case TYPE_INT:
                varInfo->Type = Type::INT;
                break;

            case TYPE_DOUBLE:
                varInfo->Type = Type::DOUBLE;
                break;

            case TYPE_STRING:
                varInfo->Type = Type::STRING;
                break;

            case TYPE_ARRAY:
                varInfo->Type = Type::ARRAY;
                break;

            case TYPE_DICTIONARY:
                varInfo->Type = Type::DICTIONARY;
                break;
        }

        (*this->impl->variables)[ast->Args[i]->Name] = varInfo;

		auto* opCode = this->impl->generateStore(i);
		this->impl->intermediateCode.push_back(opCode);
		methodInfo->Args.push_back(varInfo->Type);

		if (opCode->Opt != nullptr)
		{
			ADD_OPCODE(2);
		}
		else
			INC_OPCODE();
    }

    ast->Body->accept(this);
    jpmAddress->Opt = new ByteOptVar(this->impl->opCodeIndex);

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
		size_t index = (*this->impl->globalVariables)[ast->Value]->Index;
		auto* opCode = this->impl->generateGlobalLoad(index);
		this->impl->intermediateCode.push_back(opCode);

		if (opCode->Opt != nullptr)
		{
			ADD_OPCODE(2);
		}
		else
			INC_OPCODE();
	}
	else
	{
		size_t index = (*this->impl->variables)[ast->Value]->Index;
		auto* opCode = this->impl->generateLoad(index);
		this->impl->intermediateCode.push_back(opCode);

		if (opCode->Opt != nullptr)
		{
			ADD_OPCODE(2);
		}
		else
			INC_OPCODE();
	}
}

void VmBackend::visit(PrimativeAst* ast) {
	switch (ast->Value->Type)
	{
        case PrimativeValue::Type::PRI_DOUBLE:
        {
            vm_double_t doubleConvert = { .Double = ast->Value->Double };
            this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::OPT_dPUSH, new ByteOptVar(ast->Value->Double)));
			ADD_OPCODE(8);
        }
            break;

        case PrimativeValue::Type::PRI_BOOL:
            this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::OPT_bPUSH, new ByteOptVar(ast->Value->Bool)));
            break;

	case PrimativeValue::Type::PRI_INTEGER:
		this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::OPT_iPUSH, new ByteOptVar(ast->Value->Integer)));
		ADD_OPCODE(2)
		break;

	case PrimativeValue::Type::PRI_STRING:
    {
        // const char* text = ast->Value->String->c_str();
        // this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::OPT_sPUSH, new ByteOptVar(text, strlen(text))));
    }
		break;


	default:
		break;
	}

    /*if (this->impl->intermediateCode[this->impl->intermediateCode.size() - 1]->Opt != nullptr) {
		this->impl->opCodeIndex += this->impl->intermediateCode[this->impl->intermediateCode.size() - 1]->Opt->Size();
		console_out << _T("OPCODE : ") << this->impl->opCodeIndex << _T(" ") << __LINE__ << '\n';
	}*/

    INC_OPCODE();
}

void VmBackend::visit(ControlAst* ast)
{
	getAstItem(ast->Left);
	getAstItem(ast->Right);

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

	INC_OPCODE();
}

void VmBackend::visit(BinaryAst* ast)
{
	Type rightType =  detectType(ast->Right);
	Type leftType =  detectType(ast->Left);
	auto binaryResultType = detectType(ast);

	getAstItem(ast->Left);
	if (leftType != binaryResultType)
		addConvertOpcode(leftType, binaryResultType);

	getAstItem(ast->Right);
	if (rightType != binaryResultType)
		addConvertOpcode(rightType, binaryResultType);

	switch (ast->Op)
	{
		case PLUS:
			this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::OPT_iADD));
			break;

		case MINUS:
			this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::OPT_iSUB));
			break;

		case MULTIPLICATION:
		{
			switch(binaryResultType)
			{
				case Type::INT:
					this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::OPT_iMUL));
					break;

				case Type::DOUBLE:
					this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::OPT_dMUL));
					break;

				case Type::STRING:
					break;

				case Type::BOOL:
					this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::OPT_AND));
					break;

				default:
					break;
			}
		}
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

		default:
			break;
	}

	INC_OPCODE();
}
//if data == 123 then { data = 111 } else {data = 999}

void VmBackend::visit(StructAst* ast) { }
void VmBackend::visit(ReturnAst* ast)
{
	if (ast->Data != nullptr)
		this->getAstItem(ast->Data);

	this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::OPT_RETURN));
	INC_OPCODE();
}

void VmBackend::visit(ParenthesesGroupAst* ast) { }
void VmBackend::visit(FunctionCallAst* ast)
{
	if (ast->Package == "core" && ast->Function == "dumpopcode")
	{
		impl->system.dump(&impl->codes[0], impl->codes.size());
		return;
	}

	std::unordered_map<string_type, VariableInfo*>* variables = nullptr;

	if (this->impl->inFunctionCounter > 0)
		variables = this->impl->variables;
	else
		variables = this->impl->globalVariables;

	auto* function = this->impl->methods[ast->Package + _T("::") + ast->Function];
	if (function == nullptr)
		throw ParseError(_T("'") + ast->Function + _T("' Not Found"));

	if (function->Args.size() != ast->Args.size())
		throw ParseError("Argument type matched.");

	size_t totalParameters = ast->Args.size();
    for (size_t i = totalParameters; i > 0; --i)
	{
		Type type = detectType(ast->Args[i - 1]);
		if (type != function->Args[i - 1])
			throw ParseError("Argument not type matched.");

		getAstItem(ast->Args[i - 1]);
	}

    this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::OPT_CALL, new ByteOptVar(static_cast<int>(this->impl->methods[ast->Package + _T("::") + ast->Function]->Index))));
	ADD_OPCODE(3);

}

void VmBackend::visit(UnaryAst* ast)
{
	this->impl->intermediateCode.push_back(new OpcodeItem(vm_inst::OPT_NEG));
	INC_OPCODE();
}

void VmBackend::visit(ExprStatementAst* ast)
{
	ast->Expr->accept(this);
}
