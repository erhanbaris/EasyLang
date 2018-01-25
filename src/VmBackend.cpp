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
        DOUBLE,
        BOOL,
		STRING,
		POSITION
	} Type;

	virtual size_t Size() = 0;
};

class CurrentPositionVar : public OptVar
{
public:
	CurrentPositionVar() { Type = POSITION; }
	size_t Size() override {
		return 4;
	}
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
		vm_int_t i;
		i.Int = data;
        Data = new char[4];
        Data[0] = i.Chars[0];
        Data[1] = i.Chars[1];
		Data[2] = i.Chars[2];
		Data[3] = i.Chars[3];
        Length = 4;
    }

    ByteOptVar(double data)
    {
        Type = DOUBLE;
		vm_double_t i;
		i.Double = data;
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

	ByteOptVar(string_type const & data)
	{
		Length = data.size();
		Type = STRING;
		Data = new char[Length];
		for (size_t i = 0; i < Length; i++)
			Data[i] = data[i];
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

	size_t inClassCounter;
	size_t inFunctionCounter;

	VmBackendImpl()
	{
		inFunctionCounter = 0;
		dumpOpcode = false;
		globalVariables = new std::unordered_map<string_type, VariableInfo*>();
		variables = new std::unordered_map<string_type, VariableInfo*>();
		variablesList.push_back(variables);
	}

	inline OpcodeItem* generateStore(std::vector<char> & opcodes, size_t index)
	{
		switch (index)
		{
		case 0:
			opcodes.push_back(vm_inst::OPT_STORE_0);
			break;
		case 1:
			opcodes.push_back(vm_inst::OPT_STORE_1);
			break;
		case 2:
			opcodes.push_back(vm_inst::OPT_STORE_2);
			break;
		case 3:
			opcodes.push_back(vm_inst::OPT_STORE_3);
			break;
		case 4:
			opcodes.push_back(vm_inst::OPT_STORE_4);
			break;
		default:
			opcodes.push_back(vm_inst::OPT_STORE);
            vm_int_t len;
            len.Int = (int)index;
            opcodes.push_back(len.Chars[3]);
            opcodes.push_back(len.Chars[2]);
            opcodes.push_back(len.Chars[1]);
            opcodes.push_back(len.Chars[0]);
			break;
		}

		return nullptr;
	}

	inline OpcodeItem* generateGlobalStore(std::vector<char> & opcodes, size_t index)
	{
		switch (index)
		{
		case 0:
			opcodes.push_back(vm_inst::OPT_GSTORE_0);
            break;

		case 1:
			opcodes.push_back(vm_inst::OPT_GSTORE_1);
            break;

		case 2:
			opcodes.push_back(vm_inst::OPT_GSTORE_2);
            break;

		case 3:
			opcodes.push_back(vm_inst::OPT_GSTORE_3);
            break;

		case 4:
			opcodes.push_back(vm_inst::OPT_GSTORE_4);
            break;

		default:
            opcodes.push_back(vm_inst::OPT_GSTORE);
            vm_int_t len;
            len.Int = (int)index;
            opcodes.push_back(len.Chars[3]);
            opcodes.push_back(len.Chars[2]);
            opcodes.push_back(len.Chars[1]);
            opcodes.push_back(len.Chars[0]);
            break;
		}

		return nullptr;
	}

	inline OpcodeItem* generateLoad(std::vector<char> & opcodes, size_t index)
	{
		switch (index)
		{
		case 0:
			opcodes.push_back(vm_inst::OPT_LOAD_0);
            break;

		case 1:
			opcodes.push_back(vm_inst::OPT_LOAD_1);
            break;

		case 2:
			opcodes.push_back(vm_inst::OPT_LOAD_2);
            break;

		case 3:
			opcodes.push_back(vm_inst::OPT_LOAD_3);
            break;

		case 4:
			opcodes.push_back(vm_inst::OPT_LOAD_4);
            break;

		default:
            opcodes.push_back(vm_inst::OPT_LOAD);
            vm_int_t len;
            len.Int = (int)index;
            opcodes.push_back(len.Chars[3]);
            opcodes.push_back(len.Chars[2]);
            opcodes.push_back(len.Chars[1]);
            opcodes.push_back(len.Chars[0]);
            break;
		}

		return nullptr;
	}

	inline OpcodeItem* generateGlobalLoad(std::vector<char> & opcodes, size_t index)
	{
		switch (index)
		{
		case 0:
			opcodes.push_back(vm_inst::OPT_GLOAD_0);
            break;

		case 1:
			opcodes.push_back(vm_inst::OPT_GLOAD_1);
            break;

		case 2:
			opcodes.push_back(vm_inst::OPT_GLOAD_2);
            break;

		case 3:
			opcodes.push_back(vm_inst::OPT_GLOAD_3);
            break;

		case 4:
			opcodes.push_back(vm_inst::OPT_GLOAD_4);
            break;

		default:
            opcodes.push_back(vm_inst::OPT_GLOAD);
            vm_int_t len;
            len.Int = (int)index;
            opcodes.push_back(len.Chars[3]);
            opcodes.push_back(len.Chars[2]);
            opcodes.push_back(len.Chars[1]);
            opcodes.push_back(len.Chars[0]);
            break;
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
		this->opcodes.push_back(vm_inst::OPT_I2D);
	else if (to == Type::INT && from == Type::DOUBLE)
		this->opcodes.push_back(vm_inst::OPT_D2I);
	else if (from == Type::INT && to == Type::BOOL)
		this->opcodes.push_back(vm_inst::OPT_I2B);
	else if (to == Type::INT && from == Type::BOOL)
		this->opcodes.push_back(vm_inst::OPT_B2I);
	else if (from == Type::DOUBLE && to == Type::BOOL)
		this->opcodes.push_back(vm_inst::OPT_D2B);
	else if (to == Type::DOUBLE && from == Type::BOOL)
		this->opcodes.push_back(vm_inst::OPT_B2D);
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
            if (this->impl->variables->find(var->Value) != this->impl->variables->end())
                return this->impl->variables->find(var->Value)->second->Type;
            
            if (this->impl->globalVariables->find(var->Value) != this->impl->globalVariables->end())
                return this->impl->globalVariables->find(var->Value)->second->Type;
            
            throw ParseError(_T("'") + var->Value + _T("' Not Found"));
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
			return Type::BOOL;
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


void VmBackend::Compile(std::vector<char> & pOpcode)
{
	opcodes.assign(pOpcode.begin(), pOpcode.end());

	size_t totalAst = temporaryAsts.size();
	for (int i = 0; i < totalAst; ++i) {
		getAstItem(temporaryAsts[i]);
	}
	this->opcodes.push_back(vm_inst::OPT_HALT);

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
PrimativeValue* VmBackend::Execute()
{
	PrimativeValue* result = nullptr;
    size_t codeStart = impl->codes.size();
	
	this->Compile(this->impl->codes);
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
				result = new PrimativeValue(string_type(static_cast<char*>(lastItem->Pointer)));
				break;
		}

	if (result != nullptr)
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

vm_object* print(vm_system* vm)
{
    auto* item = vm->getObject();
    if (item != nullptr)
		switch (item->Type) {
			case vm_object::vm_object_type::BOOL:
				console_out << item->Bool << '\n';
				break;

			case vm_object::vm_object_type::INT:
				console_out << item->Int << '\n';
				break;

			case vm_object::vm_object_type::DOUBLE:
				console_out << item->Double << '\n';
				break;

			case vm_object::vm_object_type::STR:
				console_out << static_cast<char*>(item->Pointer) << '\n';
				break;
		}
    
    return nullptr;
}

vm_object* readLine(vm_system* vm)
{
    string_type text;
    std::getline(console_in, text);
    
    vm_object* returnValue = new vm_object(text);
    return returnValue;
}

VmBackend::VmBackend()
{
	impl = new VmBackendImpl;
    Scope::GlobalScope = new Scope;
    this->impl->system.addMethod(_T("io::print"), print);
    this->impl->system.addMethod(_T("io::readLine"), readLine);
}

void VmBackend::visit(AssignmentAst* ast)
{
	std::unordered_map<string_type, VariableInfo*>* variables = nullptr;
	if (this->impl->inFunctionCounter == 0 || this->impl->globalVariables->find(ast->Name) != this->impl->globalVariables->end())
		variables = this->impl->globalVariables; 
	else
		variables = this->impl->variables;

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
    
    
    
    if (this->impl->inFunctionCounter == 0 || this->impl->globalVariables->find(ast->Name) != this->impl->globalVariables->end())
		this->impl->generateGlobalStore(this->opcodes, static_cast<int>((*this->impl->globalVariables)[ast->Name]->Index)); 
	else
		this->impl->generateStore(this->opcodes, static_cast<int>((*this->impl->variables)[ast->Name]->Index));
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
	/*auto lastOperator = this->opcodes[this->opcodes.size() - 1]->OpCode;

	OpcodeItem* condition = nullptr;
	switch (lastOperator)
	{
	case vm_inst::OPT_EQ:
		condition = new OpcodeItem(vm_inst::OPT_IF_EQ);
		this->opcodes.erase(this->opcodes.begin() + (this->opcodes.size() - 1));
		break;

	default:
		condition = new OpcodeItem(vm_inst::OPT_JIF);
		break;
	}*/

	this->opcodes.push_back(vm_inst::OPT_JIF);
    size_t ifPoint = this->opcodes.size();
    size_t jumpPoint = 0;
    vm_int_t i;
    this->opcodes.push_back(0);
    this->opcodes.push_back(0);
    this->opcodes.push_back(0);
    this->opcodes.push_back(0);
	this->getAstItem(ast->True);

	if (ast->False != nullptr)
	{
		this->opcodes.push_back(vm_inst::OPT_JMP);
		
        size_t elsePoint = this->opcodes.size();
        this->opcodes.push_back(0);
        this->opcodes.push_back(0);
        this->opcodes.push_back(0);
        this->opcodes.push_back(0);
        
        jumpPoint = this->opcodes.size();
		this->getAstItem(ast->False);

		i.Int = this->opcodes.size() - (elsePoint + 4);
		this->opcodes[elsePoint] = i.Chars[3];
		this->opcodes[elsePoint + 1] = i.Chars[2];
		this->opcodes[elsePoint + 2] = i.Chars[1];
		this->opcodes[elsePoint + 3] = i.Chars[0];
	}
    else
        jumpPoint = this->opcodes.size();
    
    i.Int = jumpPoint - (ifPoint + 4);
    this->opcodes[ifPoint] = i.Chars[3];
    this->opcodes[ifPoint + 1] = i.Chars[2];
    this->opcodes[ifPoint + 2] = i.Chars[1];
    this->opcodes[ifPoint + 3] = i.Chars[0];
}

void VmBackend::visit(FunctionDefinetionAst* ast)
{
	++this->impl->inFunctionCounter;
    this->impl->variablesList.push_back(new std::unordered_map<string_type, VariableInfo*>());
    impl->variables = impl->variablesList[impl->variablesList.size() - 1];
	
	size_t funcDeclPoint = this->opcodes.size();
    this->opcodes.push_back(vm_inst::OPT_JMP);
    this->opcodes.push_back(0);
    this->opcodes.push_back(0);
    this->opcodes.push_back(0);
    this->opcodes.push_back(0);

	this->opcodes.push_back(vm_inst::OPT_METHOD);
	
	auto* functionName = ast->Name.c_str();
	vm_int_t i;
	i.Int = ast->Name.size();
	this->opcodes.push_back(i.Chars[3]);
	this->opcodes.push_back(i.Chars[2]);
	this->opcodes.push_back(i.Chars[1]);
	this->opcodes.push_back(i.Chars[0]);

	for (int j = 0; j < i.Int; ++j)
		opcodes.push_back(functionName[(i.Int - j) - 1]);

	if (this->impl->methods.find(_T("::") + ast->Name) != this->impl->methods.end())
	{
		size_t oldMethodOrderNumber = this->impl->methods[_T("::") + ast->Name]->Index;
		this->impl->codes[oldMethodOrderNumber] = vm_inst::OPT_JMP;
		// fix : this->impl->codes[oldMethodOrderNumber + 1] = static_cast<char>(this->impl->opCodeIndex);
	}

    MethodInfo* methodInfo = new MethodInfo;
    methodInfo->Index = this->opcodes.size();
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

		this->impl->generateStore(this->opcodes, i);
		methodInfo->Args.push_back(varInfo->Type);
    }

    ast->Body->accept(this);
    
    i.Int = this->opcodes.size() - (funcDeclPoint + 5);
    this->opcodes[funcDeclPoint + 1] = i.Chars[3];
    this->opcodes[funcDeclPoint + 2] = i.Chars[2];
    this->opcodes[funcDeclPoint + 3] = i.Chars[1];
    this->opcodes[funcDeclPoint + 4] = i.Chars[0];
    
    this->impl->variablesList.erase(impl->variablesList.begin() + (impl->variablesList.size() - 1));
	--this->impl->inFunctionCounter;
    delete impl->variables;
}

void VmBackend::visit(ForStatementAst* ast)
{
    ++this->impl->inFunctionCounter;
    this->impl->variablesList.push_back(new std::unordered_map<string_type, VariableInfo*>());
    impl->variables = impl->variablesList[impl->variablesList.size() - 1];

    auto* varInfo = new VariableInfo;
    varInfo->Index = impl->variables->size();
    varInfo->Name = ast->Variable;
    varInfo->Type = Type::INT;
    
    (*impl->variables)[ast->Variable] = varInfo;

    this->getAstItem(ast->Start);
    this->opcodes.push_back(vm_inst::OPT_STORE_0);
    
    size_t forPoint = this->opcodes.size();
    this->opcodes.push_back(vm_inst::OPT_LOAD_0);
    this->getAstItem(ast->End);
    this->opcodes.push_back(vm_inst::OPT_LTE);
   
    this->opcodes.push_back(vm_inst::OPT_JIF);
    size_t funcDeclPoint = this->opcodes.size();
    this->opcodes.push_back(0);
    this->opcodes.push_back(0);
    this->opcodes.push_back(0);
    this->opcodes.push_back(0);
    
    this->getAstItem(ast->Repeat);
    
    this->opcodes.push_back(vm_inst::OPT_LOAD_0);
    this->opcodes.push_back(vm_inst::OPT_INC);
    this->opcodes.push_back(vm_inst::OPT_STORE_0);
    
    
    this->opcodes.push_back(vm_inst::OPT_JMP);
    vm_int_t i;
    i.Int = forPoint - (this->opcodes.size() + 4);
    
    this->opcodes.push_back(i.Chars[3]);
    this->opcodes.push_back(i.Chars[2]);
    this->opcodes.push_back(i.Chars[1]);
    this->opcodes.push_back(i.Chars[0]);
    
    i.Int = this->opcodes.size() - (funcDeclPoint + 4);
    this->opcodes[funcDeclPoint] = i.Chars[3];
    this->opcodes[funcDeclPoint + 1] = i.Chars[2];
    this->opcodes[funcDeclPoint + 2] = i.Chars[1];
    this->opcodes[funcDeclPoint + 3] = i.Chars[0];
    
    
    
    this->impl->variablesList.erase(impl->variablesList.begin() + (impl->variablesList.size() - 1));
    --this->impl->inFunctionCounter;
    delete impl->variables;
}

void VmBackend::visit(VariableAst* ast)
{
	if (this->impl->globalVariables->find(ast->Value) != this->impl->globalVariables->end())
	{
		size_t index = (*this->impl->globalVariables)[ast->Value]->Index;
		this->impl->generateGlobalLoad(this->opcodes, index);
	}
	else if (this->impl->variables->find(ast->Value) != this->impl->variables->end())
	{
		size_t index = (*this->impl->variables)[ast->Value]->Index;
		this->impl->generateLoad(this->opcodes, index);
	}
    else
        throw ParseError(ast->Value + _T(" Not Found"));
}

void VmBackend::visit(PrimativeValue* value) {
	switch (value->Type)
	{
	case PrimativeValue::Type::PRI_ARRAY:
	{
		this->opcodes.push_back(vm_inst::OPT_INITARRAY);
		if (value != nullptr)
		{
			size_t arrayLength = value->Array->size();
			for (size_t i = 0; i < arrayLength; ++i)
			{
				visit(value->Array->at(i));
				this->opcodes.push_back(vm_inst::OPT_aPUSH);
			}
		}
	}
	break;

	case PrimativeValue::Type::PRI_DOUBLE:
	{
		if (value->Double == 0.0)
			this->opcodes.push_back(vm_inst::OPT_dPUSH_0);
		else if (value->Double == 1.0)
			this->opcodes.push_back(vm_inst::OPT_dPUSH_1);
		else if (value->Double == 2.0)
			this->opcodes.push_back(vm_inst::OPT_dPUSH_2);
		else if (value->Double == 3.0)
			this->opcodes.push_back(vm_inst::OPT_dPUSH_3);
		else if (value->Double == 4.0)
			this->opcodes.push_back(vm_inst::OPT_dPUSH_4);
		else
		{
			vm_double_t i;
			i.Double = value->Double;
			this->opcodes.push_back(vm_inst::OPT_dPUSH);
			this->opcodes.push_back(i.Chars[7]);
			this->opcodes.push_back(i.Chars[6]);
			this->opcodes.push_back(i.Chars[5]);
			this->opcodes.push_back(i.Chars[4]);
			this->opcodes.push_back(i.Chars[3]);
			this->opcodes.push_back(i.Chars[2]);
			this->opcodes.push_back(i.Chars[1]);
		}
	}
	break;

	case PrimativeValue::Type::PRI_BOOL:
		switch (value->Bool)
		{
		case true:
			this->opcodes.push_back(vm_inst::OPT_bPUSH_1);
			break;

		case false:
			this->opcodes.push_back(vm_inst::OPT_bPUSH_0);
			break;
		}
		break;

	case PrimativeValue::Type::PRI_INTEGER:
		switch (value->Integer)
		{
		case 0:
			this->opcodes.push_back(vm_inst::OPT_iPUSH_0);
			break;

		case 1:
			this->opcodes.push_back(vm_inst::OPT_iPUSH_1);
			break;

		case 2:
			this->opcodes.push_back(vm_inst::OPT_iPUSH_2);
			break;

		case 3:
			this->opcodes.push_back(vm_inst::OPT_iPUSH_3);
			break;

		case 4:
			this->opcodes.push_back(vm_inst::OPT_iPUSH_4);
			break;

		default:
			this->opcodes.push_back(vm_inst::OPT_iPUSH);
			vm_int_t i;
			i.Int = value->Integer;
			this->opcodes.push_back(i.Chars[3]);
			this->opcodes.push_back(i.Chars[2]);
			this->opcodes.push_back(i.Chars[1]);
			this->opcodes.push_back(i.Chars[0]);
			break;
		}
		break;

	case PrimativeValue::Type::PRI_STRING:
	{
		auto* text = value->String->c_str();
		vm_int_t i;
		i.Int = value->String->size();
		this->opcodes.push_back(vm_inst::OPT_sPUSH);
		this->opcodes.push_back(i.Chars[3]);
		this->opcodes.push_back(i.Chars[2]);
		this->opcodes.push_back(i.Chars[1]);
		this->opcodes.push_back(i.Chars[0]);

		for (int j = 0; j < i.Int; ++j)
			opcodes.push_back(text[(i.Int - j) - 1]);
	}
	break;


	default:
		break;
	}
}

void VmBackend::visit(PrimativeAst* ast) {
	this->visit(ast->Value);
}

void VmBackend::visit(ControlAst* ast)
{
	getAstItem(ast->Left);
	getAstItem(ast->Right);

	switch (ast->Op)
	{
	case EQUAL:
		this->opcodes.push_back(vm_inst::OPT_EQ);
		break;

	//case NOT_EQUAL:
	//	this->opcodes.push_back(new OpcodeItem(vm_inst::opt_n));
	//	break;

	case GREATOR:
		this->opcodes.push_back(vm_inst::OPT_GT);
		break;

	case LOWER:
		this->opcodes.push_back(vm_inst::OPT_LT);
		break;

	case GREATOR_EQUAL:
		this->opcodes.push_back(vm_inst::OPT_GTE);
		break;

	case LOWER_EQUAL:
		this->opcodes.push_back(vm_inst::OPT_LTE);
		break;

	case OR:
		this->opcodes.push_back(vm_inst::OPT_OR);
		break;

	case AND:
		this->opcodes.push_back(vm_inst::OPT_AND);
		break;
	}
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
		{
			switch (binaryResultType)
			{
			case Type::INT:
				this->opcodes.push_back(vm_inst::OPT_iADD);
				break;

			case Type::DOUBLE:
				this->opcodes.push_back(vm_inst::OPT_dADD);
				break;

			case Type::BOOL:
				this->opcodes.push_back(vm_inst::OPT_bADD);
				break;
			}
		}
			break;

		case MINUS:
		{
			switch (binaryResultType)
			{
			case Type::INT:
				this->opcodes.push_back(vm_inst::OPT_iSUB);
				break;

			case Type::DOUBLE:
				this->opcodes.push_back(vm_inst::OPT_dSUB);
				break;

			case Type::BOOL:
				this->opcodes.push_back(vm_inst::OPT_bSUB);
				break;
			}
		}
			break;

		case MULTIPLICATION:
		{
			switch(binaryResultType)
			{
				case Type::INT:
					this->opcodes.push_back(vm_inst::OPT_iMUL);
					break;

				case Type::DOUBLE:
					this->opcodes.push_back(vm_inst::OPT_dMUL);
					break;

				case Type::BOOL:
					this->opcodes.push_back(vm_inst::OPT_bMUL);
					break;
			}
		}
			break;

		case DIVISION:
		{
			switch (binaryResultType)
			{
			case Type::INT:
				this->opcodes.push_back(vm_inst::OPT_iDIV);
				break;

			case Type::DOUBLE:
				this->opcodes.push_back(vm_inst::OPT_dDIV);
				break;

			case Type::BOOL:
				this->opcodes.push_back(vm_inst::OPT_bDIV);
				break;
			}
		}
			break;
	}
}

void VmBackend::visit(StructAst* ast) 
{
	getAstItem(ast->Target);
	getAstItem(ast->Source1);

	switch (ast->Op)
	{
	case APPEND:
		this->opcodes.push_back(vm_inst::OPT_aPUSH);
		break;

	case INDEXER:
		this->opcodes.push_back(vm_inst::OPT_aGET);
		break;

	default:
		break;
	}
}

void VmBackend::visit(ReturnAst* ast)
{
	if (ast->Data != nullptr)
		this->getAstItem(ast->Data);

	this->opcodes.push_back(vm_inst::OPT_RETURN);
}

void VmBackend::visit(ParenthesesGroupAst* ast) { }
void VmBackend::visit(FunctionCallAst* ast)
{
	if (ast->Package == _T("core") && ast->Function == _T("dumpopcode"))
	{
		impl->system.dumpOpcode(&impl->codes[0], impl->codes.size());
		return;
    }

	else if (ast->Package == _T("core") && ast->Function == _T("dump"))
	{
		impl->system.dump(&impl->codes[0], impl->codes.size());
		return;
	}

	else if (ast->Package == _T("core") && ast->Function == _T("dumpStack"))
	{
		impl->system.dumpStack();
		return;
	}

	else if (ast->Package == _T("core") && ast->Function == _T("dumpAll"))
	{
		impl->system.dumpOpcode(&impl->codes[0], impl->codes.size());
		impl->system.dump(&impl->codes[0], impl->codes.size());
		impl->system.dumpStack();
		return;
	}

	auto* function = this->impl->methods[ast->Package + _T("::") + ast->Function];
	if (function != nullptr)
    {
        if (function->Args.size() != ast->Args.size())
            throw ParseError(_T("Argument type matched."));

        size_t totalParameters = ast->Args.size();
        for (size_t i = totalParameters; i > 0; --i)
        {
            Type type = detectType(ast->Args[i - 1]);
            if (type != function->Args[i - 1])
                throw ParseError(_T("Argument not type matched."));

            getAstItem(ast->Args[i - 1]);
        }

        this->opcodes.push_back(vm_inst::OPT_CALL);
        vm_int_t len;
        len.Int = static_cast<int>(this->impl->methods[ast->Package + _T("::") + ast->Function]->Index) - (this->opcodes.size() + 4);
        opcodes.push_back(len.Chars[3]);
        opcodes.push_back(len.Chars[2]);
        opcodes.push_back(len.Chars[1]);
        opcodes.push_back(len.Chars[0]);
    }
    else
    {
        size_t totalParameters = ast->Args.size();
        for (size_t i = totalParameters; i > 0; --i)
        {
            Type type = detectType(ast->Args[i - 1]);
            getAstItem(ast->Args[i - 1]);
        }
      
        string_type funcName = ast->Package + _T("::") + ast->Function;

        this->opcodes.push_back(vm_inst::OPT_INVOKE);
        vm_int_t len;
        len.Int = static_cast<int>(funcName.size());
        opcodes.push_back(len.Chars[0]);

        for (int j = 0; j < len.Int; ++j)
            opcodes.push_back(funcName[(len.Int - j) - 1]);
    }

    // throw ParseError(_T("'") + ast->Function + _T("' Not Found"));
}

void VmBackend::visit(UnaryAst* ast)
{
	this->opcodes.push_back(vm_inst::OPT_NEG);
}

void VmBackend::visit(ExprStatementAst* ast)
{
	ast->Expr->accept(this);
}
