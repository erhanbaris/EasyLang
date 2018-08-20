#include "Exceptions.h"
#include "VmBackend.h"
#include "System.h"
#include "Vm.h"

#include <array>

vm_gc::_init vm_gc::_initializer;

namespace {
    std::array<vm_inst, 18> LOCAL_LOADS
    {
        vm_inst::OPT_LOAD,
        vm_inst::OPT_LOAD_0,
        vm_inst::OPT_LOAD_1,
        vm_inst::OPT_LOAD_2,
        vm_inst::OPT_LOAD_3,
        vm_inst::OPT_LOAD_4,
        vm_inst::OPT_LOAD_5,
        vm_inst::OPT_LOAD_6,
        vm_inst::OPT_LOAD_7,
        vm_inst::OPT_LOAD_8,
        vm_inst::OPT_LOAD_9,
        vm_inst::OPT_LOAD_10,
        vm_inst::OPT_LOAD_11,
        vm_inst::OPT_LOAD_12,
        vm_inst::OPT_LOAD_13,
        vm_inst::OPT_LOAD_14,
        vm_inst::OPT_LOAD_15,
        vm_inst::OPT_LOAD_16
    };

    std::array<vm_inst, 18> GLOBAL_LOADS
    {
        vm_inst::OPT_GLOAD,
        vm_inst::OPT_GLOAD_0,
        vm_inst::OPT_GLOAD_1,
        vm_inst::OPT_GLOAD_2,
        vm_inst::OPT_GLOAD_3,
        vm_inst::OPT_GLOAD_4,
        vm_inst::OPT_GLOAD_5,
        vm_inst::OPT_GLOAD_6,
        vm_inst::OPT_GLOAD_7,
        vm_inst::OPT_GLOAD_8,
        vm_inst::OPT_GLOAD_9,
        vm_inst::OPT_GLOAD_10,
        vm_inst::OPT_GLOAD_11,
        vm_inst::OPT_GLOAD_12,
        vm_inst::OPT_GLOAD_13,
        vm_inst::OPT_GLOAD_14,
        vm_inst::OPT_GLOAD_15,
        vm_inst::OPT_GLOAD_16
    };

    std::array<vm_inst, 18> LOCAL_STORES
    {
        vm_inst::OPT_STORE,
        vm_inst::OPT_STORE_0,
        vm_inst::OPT_STORE_1,
        vm_inst::OPT_STORE_2,
        vm_inst::OPT_STORE_3,
        vm_inst::OPT_STORE_4,
        vm_inst::OPT_STORE_5,
        vm_inst::OPT_STORE_6,
        vm_inst::OPT_STORE_7,
        vm_inst::OPT_STORE_8,
        vm_inst::OPT_STORE_9,
        vm_inst::OPT_STORE_10,
        vm_inst::OPT_STORE_11,
        vm_inst::OPT_STORE_12,
        vm_inst::OPT_STORE_13,
        vm_inst::OPT_STORE_14,
        vm_inst::OPT_STORE_15,
        vm_inst::OPT_STORE_16
    };

    std::array<vm_inst, 18> GLOBAL_STORES
    {
        vm_inst::OPT_GSTORE,
        vm_inst::OPT_GSTORE_0,
        vm_inst::OPT_GSTORE_1,
        vm_inst::OPT_GSTORE_2,
        vm_inst::OPT_GSTORE_3,
        vm_inst::OPT_GSTORE_4,
        vm_inst::OPT_GSTORE_5,
        vm_inst::OPT_GSTORE_6,
        vm_inst::OPT_GSTORE_7,
        vm_inst::OPT_GSTORE_8,
        vm_inst::OPT_GSTORE_9,
        vm_inst::OPT_GSTORE_10,
        vm_inst::OPT_GSTORE_11,
        vm_inst::OPT_GSTORE_12,
        vm_inst::OPT_GSTORE_13,
        vm_inst::OPT_GSTORE_14,
        vm_inst::OPT_GSTORE_15,
        vm_inst::OPT_GSTORE_16
    };
}

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
    char_type * Data{nullptr};
    size_t Length;
    ByteOptVar() { Type = INT; }
    ByteOptVar(char_type * data, size_t length) { Type = INT; Data = data; Length = length; }
    ByteOptVar(int data)
    {
        Type = INT;
        vm_int_t i;
        i.Int = data;
        Data = new char_type[4];
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
        Data = new char_type[8];
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
        Data = new char_type[Length];
        for (size_t i = 0; i < Length; i++)
            Data[i] = data[i];
    }

    ByteOptVar(bool data)
    {
        Type = BOOL;
        Data = new char_type[1];
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
    string_type Name;
    int Index;
};

class MethodInfo {
public:
    string_type Function;
    int Index;
    size_t ArgsCount;
};

class VmBackendImpl
{
public:
    vm_system system;
    std::unordered_map<string_type, VariableInfo*>* variables;
    std::unordered_map<string_type, VariableInfo*>* globalVariables;
    std::vector<std::unordered_map<string_type, VariableInfo*>*> variablesList;
    std::unordered_map<string_type, MethodInfo*> methods;
    std::vector<char_type> codes;
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

    inline void generateStore(std::array<vm_inst, 18> storeTypes, std::vector<char_type> & opcodes, size_t index)
    {
        if (index > storeTypes.size() - 2)
        {
            opcodes.push_back(storeTypes[0]);
            vm_int_t len;
            len.Int = (int)index;
            opcodes.push_back(len.Chars[3]);
            opcodes.push_back(len.Chars[2]);
            opcodes.push_back(len.Chars[1]);
            opcodes.push_back(len.Chars[0]);
        }
        else
            opcodes.push_back(storeTypes[index + 1]);
    }

    inline void generateLoad(std::array<vm_inst, 18> loadTypes, std::vector<char_type> & opcodes, size_t index)
    {
        if (index > loadTypes.size() - 2)
        {
            opcodes.push_back(loadTypes[0]);
            vm_int_t len;
            len.Int = (int)index;
            opcodes.push_back(len.Chars[3]);
            opcodes.push_back(len.Chars[2]);
            opcodes.push_back(len.Chars[1]);
            opcodes.push_back(len.Chars[0]);
        }
        else
            opcodes.push_back(loadTypes[index + 1]);
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


void VmBackend::Compile(std::vector<char_type> & pOpcode)
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
    Value lastItem = impl->system.getObject();

    if (IS_NUM(lastItem))
    {
        double num = valueToNumber(lastItem);

        if (std::isinf(num) || std::isnan(num))
            result = new PrimativeValue("");
        else
        {
            if (trunc(num) == num)
                result = new PrimativeValue((int)num);
            else
                result = new PrimativeValue(num);
        }
    }
    else if (IS_BOOL(lastItem))
    {
        result = new PrimativeValue(AS_BOOL(lastItem));
    }
    else if (lastItem == NULL_VAL)
    {
        result = new PrimativeValue();
    }
    else
    {
        vm_object* obj = AS_OBJ(lastItem);
        switch (obj->Type)
        {
            case vm_object::vm_object_type::EMPTY:
                result = new PrimativeValue();
                break;

            case vm_object::vm_object_type::ARRAY:
                console_out << _T("(ARRAY) Size: ") << static_cast<vm_array*>(obj->Pointer)->Indicator << '\n';
                break;

            case vm_object::vm_object_type::STR:
                char_type* str = static_cast<char_type*>(obj->Pointer);
                result = new PrimativeValue(string_type(str));
                break;
        }
    }

    

    return result;
}

void VmBackend::Execute(std::vector<char_type> const & opcodes)
{
    impl->system.execute(const_cast<char_type*>(&opcodes[0]), opcodes.size(), 0);
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

VmBackend::VmBackend()
{
    impl = new VmBackendImpl;
    Scope::GlobalScope = new Scope;

    std::unordered_map<string_type, VmMethod>::iterator methodItEnd = System::SystemMethods.end();
    std::unordered_map<string_type, VmMethod>::iterator methodIt = System::SystemMethods.begin();

    for(; methodIt != methodItEnd; ++methodIt)
    {
        this->impl->system.addMethod((*methodIt).first, (*methodIt).second);
    }
}

void VmBackend::visit(AssignmentAst* ast)
{
    std::unordered_map<string_type, VariableInfo*>* variables = nullptr;
    if (this->impl->inFunctionCounter == 0 || this->impl->globalVariables->find(ast->Name) != this->impl->globalVariables->end())
        variables = this->impl->globalVariables; 
    else
        variables = this->impl->variables;

    auto* varInfo = new VariableInfo;
    if (variables->find(ast->Name) != variables->end())
        varInfo->Index = variables->find(ast->Name)->second->Index;
    else
        varInfo->Index = variables->size();

    varInfo->Name = ast->Name;
    (*variables)[ast->Name] = varInfo;
    
    this->getAstItem(ast->Data);
    
    if (this->impl->inFunctionCounter == 0 || this->impl->globalVariables->find(ast->Name) != this->impl->globalVariables->end())
        this->impl->generateStore(GLOBAL_STORES, this->opcodes, static_cast<int>((*this->impl->globalVariables)[ast->Name]->Index));
    else
        this->impl->generateStore(LOCAL_STORES, this->opcodes, static_cast<int>((*this->impl->variables)[ast->Name]->Index));
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
    vm_inst lastOperator = (vm_inst)this->opcodes[this->opcodes.size() - 1];

    OpcodeItem* condition = nullptr;
    switch (lastOperator)
    {
    case vm_inst::OPT_EQ:
        this->opcodes.erase(this->opcodes.begin() + (this->opcodes.size() - 1));
        this->opcodes.push_back(vm_inst::OPT_IF_EQ);
        break;

    default:
        this->opcodes.push_back(vm_inst::OPT_JIF);
        break;
    }

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

    this->opcodes.push_back(vm_inst::OPT_METHOD_DEF);
    
    auto* functionName = ast->Name.c_str();
    vm_int_t i;
    i.Int = ast->Name.size();
    this->opcodes.push_back(i.Chars[3]);
    this->opcodes.push_back(i.Chars[2]);
    this->opcodes.push_back(i.Chars[1]);
    this->opcodes.push_back(i.Chars[0]);

    for (int j = 0; j < i.Int; ++j)
        opcodes.push_back(functionName[j]);

    if (this->impl->methods.find(_T("::") + ast->Name) != this->impl->methods.end())
    {
        size_t oldMethodOrderNumber = this->impl->methods[_T("::") + ast->Name]->Index;
        this->impl->codes[oldMethodOrderNumber] = vm_inst::OPT_JMP;
        // fix : this->impl->codes[oldMethodOrderNumber + 1] = static_cast<char>(this->impl->opCodeIndex);
    }

    MethodInfo* methodInfo = new MethodInfo;
    methodInfo->Index = this->opcodes.size();


    methodInfo->Function = ast->Name;
    methodInfo->ArgsCount = ast->Args.size();
    this->impl->methods[_T("::") + ast->Name] = methodInfo;
    size_t totalParameter = ast->Args.size();

    // Todo : Save variable information
    for (size_t i = 0; i < totalParameter; ++i) {

        auto* varInfo = new VariableInfo;
        varInfo->Index = i;
        varInfo->Name = ast->Args[i]->Name;

        (*this->impl->variables)[ast->Args[i]->Name] = varInfo;

        this->impl->generateStore(LOCAL_STORES, this->opcodes, i);

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

    (*impl->variables)[ast->Variable] = varInfo;

    this->getAstItem(ast->Start);
    //this->opcodes.push_back(vm_inst::OPT_STORE_0);
    
    size_t forPoint = this->opcodes.size();
    //this->opcodes.push_back(vm_inst::OPT_LOAD_0);
    this->getAstItem(ast->End);
    this->opcodes.push_back(vm_inst::OPT_LTE);
   
    this->opcodes.push_back(vm_inst::OPT_JIF);
    size_t funcDeclPoint = this->opcodes.size();
    this->opcodes.push_back(0);
    this->opcodes.push_back(0);
    this->opcodes.push_back(0);
    this->opcodes.push_back(0);
    
    this->getAstItem(ast->Repeat);
    
    //this->opcodes.push_back(vm_inst::OPT_LOAD_0);
    this->opcodes.push_back(vm_inst::OPT_INC);
    //this->opcodes.push_back(vm_inst::OPT_STORE_0);
    
    
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
        this->impl->generateLoad(GLOBAL_LOADS, this->opcodes, index);
    }
    else if (this->impl->variables->find(ast->Value) != this->impl->variables->end())
    {
        size_t index = (*this->impl->variables)[ast->Value]->Index;
        this->impl->generateLoad(LOCAL_LOADS, this->opcodes, index);
    }
    else
        throw ParseError(ast->Value + _T(" Not Found"));
}

void VmBackend::visit(PrimativeValue* value) {
    switch (value->Type)
    {
    case PrimativeValue::Type::PRI_ARRAY:
    {
        if (value != nullptr)
        {
            size_t arrayLength = value->Array->size();
            for (int i = arrayLength - 1; i >= 0; --i)
            {
                visit(value->Array->at(i));
            }

            if (arrayLength > 0)
            {
                this->opcodes.push_back(vm_inst::OPT_INITARRAY);
                vm_int_t i;
                i.Int = arrayLength;
                this->opcodes.push_back(i.Chars[3]);
                this->opcodes.push_back(i.Chars[2]);
                this->opcodes.push_back(i.Chars[1]);
                this->opcodes.push_back(i.Chars[0]);
            }
            else 
                this->opcodes.push_back(vm_inst::OPT_INITEMPTYARRAY);
        }
    }
    break;

    case PrimativeValue::Type::PRI_DOUBLE:
    {
        if (value->Double == 0.0)
            this->opcodes.push_back(vm_inst::OPT_CONST_DOUBLE_0);
        else if (value->Double == 1.0)
            this->opcodes.push_back(vm_inst::OPT_CONST_DOUBLE_1);
        else {
            vm_double_t i;
            i.Double = value->Double;
            this->opcodes.push_back(vm_inst::OPT_CONST_DOUBLE);
            this->opcodes.push_back(i.Chars[7]);
            this->opcodes.push_back(i.Chars[6]);
            this->opcodes.push_back(i.Chars[5]);
            this->opcodes.push_back(i.Chars[4]);
            this->opcodes.push_back(i.Chars[3]);
            this->opcodes.push_back(i.Chars[2]);
            this->opcodes.push_back(i.Chars[1]);
            this->opcodes.push_back(i.Chars[0]);
        }
    }
    break;

    case PrimativeValue::Type::PRI_BOOL:
        switch (value->Bool)
        {
        case true:
            this->opcodes.push_back(vm_inst::OPT_CONST_BOOL_TRUE);
            break;

        case false:
            this->opcodes.push_back(vm_inst::OPT_CONST_BOOL_FALSE);
            break;
        }
        break;

    case PrimativeValue::Type::PRI_INTEGER:
        if (value->Integer == 0)
            this->opcodes.push_back(vm_inst::OPT_CONST_INT_0);
        else if (value->Integer == 1)
            this->opcodes.push_back(vm_inst::OPT_CONST_INT_1);
        else {
            this->opcodes.push_back(vm_inst::OPT_CONST_INT);
            vm_int_t i;
            i.Int = value->Integer;
            this->opcodes.push_back(i.Chars[3]);
            this->opcodes.push_back(i.Chars[2]);
            this->opcodes.push_back(i.Chars[1]);
            this->opcodes.push_back(i.Chars[0]);
        }
        break;

    case PrimativeValue::Type::PRI_STRING:
    {
        auto* text = value->String->c_str();
        vm_int_t i;
        i.Int = value->String->size();
        this->opcodes.push_back(vm_inst::OPT_CONST_STR);
        this->opcodes.push_back(i.Chars[3]);
        this->opcodes.push_back(i.Chars[2]);
        this->opcodes.push_back(i.Chars[1]);
        this->opcodes.push_back(i.Chars[0]);

        for (int j = 0; j < i.Int; ++j)
            opcodes.push_back(text[j]);
    }
    break;

    case PrimativeValue::Type::PRI_NULL:
        this->opcodes.push_back(vm_inst::OPT_EMPTY);
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

    case NOT_EQUAL:
        this->opcodes.push_back(vm_inst::OPT_NOT_EQ);
        break;

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
    getAstItem(ast->Left);
    getAstItem(ast->Right);


    switch (ast->Op)
    {
        case PLUS:
            this->opcodes.push_back(vm_inst::OPT_ADD);
            break;

        case MINUS:
            this->opcodes.push_back(vm_inst::OPT_SUB);
            break;

        case MULTIPLICATION:
            this->opcodes.push_back(vm_inst::OPT_MUL);
            break;

        case DIVISION:
            this->opcodes.push_back(vm_inst::OPT_DIV);
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
        this->opcodes.push_back(vm_inst::OPT_APPEND);
        break;

    case INDEXER:
        this->opcodes.push_back(vm_inst::OPT_INDEX);
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
    if (ast->Package == _T("core") && ast->Function == _T("dumpOpcode"))
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

    auto functionName = ast->Package + _T("::") + ast->Function;
    auto* function = this->impl->methods[functionName];
    if (function != nullptr)
    {
        auto funcArgCount = function->ArgsCount;
        auto astArgCount = ast->Args.size();
        if (funcArgCount != astArgCount)
            throw ParseError(_T("Argument type matched."));

        size_t totalParameters = ast->Args.size();
        for (size_t i = totalParameters; i > 0; --i)
        {
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
        //native method call
        size_t totalParameters = ast->Args.size();
        for (size_t i = totalParameters; i > 0; --i)
        {
            getAstItem(ast->Args[i - 1]);
        }
      

        this->opcodes.push_back(vm_inst::OPT_CALL_NATIVE);

        vm_int_t len;
        len.Int = static_cast<int>(functionName.size());
        opcodes.push_back(len.Chars[3]);
        opcodes.push_back(len.Chars[2]);
        opcodes.push_back(len.Chars[1]);
        opcodes.push_back(len.Chars[0]);

        for (int j = 0; j < len.Int; ++j)
            opcodes.push_back(functionName[j]);
    }

    // throw ParseError(_T("'") + ast->Function + _T("' Not Found"));
}

void VmBackend::visit(UnaryAst* ast)
{
    getAstItem(ast->Data);
    this->opcodes.push_back(vm_inst::OPT_NEG);
}

void VmBackend::visit(ExprStatementAst* ast)
{
    ast->Expr->accept(this);
}
